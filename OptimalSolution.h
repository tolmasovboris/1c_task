#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include "GameScene.h"

using std::string;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::queue;
using std::cout;
using std::endl;

class OptimalSolution
{
public:
    OptimalSolution(GameScene& scene)
        : gameScene(scene)
    {
        vertices = scene.getVertices();
        edges = scene.getEdges();
        adj = scene.getAdj();
        day_cost = scene.getDayCost();
        entrance = scene.getEntrance();
        current_pos = entrance;

        visited_rooms.insert(entrance);

        int idx = 0;
        for (auto pair : edges) {
            edge_to_idx[pair.first] = idx;
            idx++;
        }

        status.resize(edges.size(), false);
        known.resize(edges.size(), false);
    }

    void Solution()
    {
        while (true) {
            manualOrder();

            vector<string> step = optimalOrder();
            if (step.size() == 0) {
                cout << "> END" << endl;
                cout << gameScene.End() << endl;
                break;
            }

            makeOptimStep(step);
        }
    }

private:
    void manualOrder() {
        bool flag = true;
        while (flag == true) {
            flag = false;

            vector<PathInfo> look_res = gameScene.Look();
            for (int i = 0; i < look_res.size(); i++) {
                int id = edge_to_idx[look_res[i].edge_id];
                known[id] = true;
                status[id] = look_res[i].is_ok;
            }

            vector<string> path = findPathToUnexplored();
            if (path.size() > 0) {
                for (int i = 0; i < path.size(); i++) {
                    string edge_id = path[i];
                    cout << "> MOVE " << edge_id << endl;
                    gameScene.Move(edge_id);

                    Edge e = edges[edge_id];
                    if (e.u == current_pos) {
                        current_pos = e.v;
                    } else {
                        current_pos = e.u;
                    }
                    visited_rooms.insert(current_pos);
                }
                flag = true;
            }
        }
    }

    // Подсчет выгоды для конкретного vertex
    double calculateProfit(const string& name) {
        unordered_set<string> tempSet;
        return calculateProfitRec(name, tempSet);
    }

    double calculateProfitRec(string name, unordered_set<string>& tempSet) {
        if (tempSet.contains(name)) return 0.0;
        tempSet.insert(name);

        double cur_profit = 0.0;
        if (!visited_rooms.contains(name)) {
            cur_profit = vertices[name];
        }

        double next_profit = 0.0;

        for (int i = 0; i < adj[name].size(); i++) {
            string edge_id = adj[name][i];
            Edge e = edges[edge_id];

            string nxt = "";
            if (e.u == name) {
                nxt = e.v;
            } else {
                nxt = e.u;
            }

            if (tempSet.contains(nxt) || visited_rooms.contains(nxt)) {
                continue;
            }

            int id = edge_to_idx[edge_id];
            double nxt_value = calculateProfitRec(nxt, tempSet);

            if (known[id] == false) {
                // UNKNOWN
                double val1 = nxt_value;
                double val2 = nxt_value - day_cost;
                if (val2 < 0) val2 = 0;

                next_profit += e.probability * val1 + (1.0 - e.probability) * val2;
            } else if (status[id] == true) {
                // OK
                next_profit += nxt_value;
            } else {
                // BROKEN
                double val = nxt_value - day_cost;
                if (val < 0) val = 0;
                next_profit += val;
            }
        }

        tempSet.erase(name);
        return cur_profit + next_profit;
    }

    // Генирирует оптимальный шаг, пусто - если нужно остановиться
    vector<string> optimalOrder() {
        string best_id = "";
        double mx = 0.0;

        for (auto r : visited_rooms) {
            for (int i = 0; i < adj[r].size(); i++) {
                string edge_id = adj[r][i];
                int id = edge_to_idx[edge_id];

                if (known[id] == true && status[id] == false) {
                    Edge e = edges[edge_id];

                    string target = "";
                    if (e.u == r) {
                        target = e.v;
                    } else {
                        target = e.u;
                    }

                    if (!visited_rooms.contains(target)) {
                        double prof = calculateProfit(target);
                        double cost = prof - day_cost;

                        if (cost > mx) {
                            mx = cost;
                            best_id = edge_id;
                        }
                    }
                }
            }
        }

        if (best_id == "") {
            vector<string> empty_vec;
            return empty_vec;
        }

        vector<string> res;
        res.push_back("CLEAR");
        res.push_back(best_id);
        return res;
    }

    // Использовать сгенированный шаг
    void makeOptimStep(const vector<string>& step) {
        if (step.size() >= 2 && step[0] == "CLEAR") {
            string e_id = step[1];
            cout << "> CLEAR " << e_id << endl;
            gameScene.Clear(e_id);

            int id = edge_to_idx[e_id];
            known[id] = true;
            status[id] = true;
            current_pos = entrance;
        }
    }

    vector<string> findPathToUnexplored() {
        queue<string> q;
        unordered_map<string, string> p_edge;
        unordered_map<string, string> p_node;
        unordered_set<string> visited;

        q.push(current_pos);
        visited.insert(current_pos);

        string target = "";

        while (!q.empty()) {
            string curr = q.front();
            q.pop();

            if (!visited_rooms.contains(curr)) {
                target = curr;
                break;
            }

            bool has_unk = false;
            for (int i = 0; i < adj[curr].size(); i++) {
                string edge_id = adj[curr][i];
                int id = edge_to_idx[edge_id];
                if (known[id] == false) {
                    has_unk = true;
                    break;
                }
            }
            if (has_unk == true && curr != current_pos) {
                target = curr;
                break;
            }

            for (int i = 0; i < adj[curr].size(); i++) {
                string edge_id = adj[curr][i];
                int id = edge_to_idx[edge_id];
                if (known[id] == true && status[id] == true) {
                    Edge e = edges[edge_id];

                    string nxt = "";
                    if (e.u == curr) {
                        nxt = e.v;
                    } else {
                        nxt = e.u;
                    }

                    if (!visited.contains(nxt)) {
                        visited.insert(nxt);
                        p_edge[nxt] = edge_id;
                        p_node[nxt] = curr;
                        q.push(nxt);
                    }
                }
            }
        }

        if (target == "") {
            vector<string> empty_path;
            return empty_path;
        }

        vector<string> path;
        string curr = target;
        while (curr != current_pos) {
            path.push_back(p_edge[curr]);
            curr = p_node[curr];
        }
        reverse(path.begin(), path.end());
        return path;
    }

    GameScene& gameScene;
    int day_cost;
    string entrance;
    string current_pos;

    unordered_map<string, int> vertices;
    unordered_map<string, Edge> edges;
    unordered_map<string, vector<string>> adj;
    unordered_set<string> visited_rooms;

    unordered_map<string, int> edge_to_idx;
    vector<bool> status; // true - OK, false - BROKEN
    vector<bool> known;  // текущий статус
};