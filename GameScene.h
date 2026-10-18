#pragma once
#include <format>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::string;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::format;

struct Edge {
    string id;
    string u;
    string v;
    double probability;
    bool active;

    Edge() {
        id = "";
        u = "";
        v = "";
        probability = 0.0;
        active = false;
    }

    Edge(string id, string u, string v, double p) {
        this->id = id;
        this->u = u;
        this->v = v;
        this->probability = p;
        this->active = false;
    }
};

// Look's answer
struct PathInfo {
    string edge_id;
    string target;
    bool is_ok;
};

class GameScene {
public:
    GameScene(int day_cost,
              string entrance,
              unordered_map<string, int> vertices,
              unordered_map<string, Edge> edges)
    {
        this->day_cost = day_cost;
        this->entrance = entrance;
        this->vertices = vertices;
        this->edges = edges;

        for (auto item : this->edges) {
            Edge e = item.second;
            adj[e.u].push_back(e.id);
            adj[e.v].push_back(e.id);
        }
        resetExpedition();
    }
    void setDayCost(int day_cost)
    {
        this->day_cost = day_cost;
    }

    void resetExpedition() {
        current_pos = entrance;
        days = 0;
        collected_profit = vertices[entrance];
        visited_rooms.clear();
        visited_rooms.insert(entrance);
    }

    void generateRandomLevel() {
        for (auto& pair : edges) {
            std::bernoulli_distribution d(pair.second.probability);
            pair.second.active = d(gen);
        }
        resetExpedition();
    }

    void setCustomLevel(unordered_map<string, bool> fixed_states) {
        for (auto pair : fixed_states) {
            if (edges.contains(pair.first)) {
                edges[pair.first].active = pair.second;
            }
        }
        resetExpedition();
    }

    vector<PathInfo> Look() {
        vector<PathInfo> res;
        for (int i = 0; i < adj[current_pos].size(); i++) {
            string edge_id = adj[current_pos][i];
            Edge e = edges[edge_id];

            string target = (e.u == current_pos) ? e.v : e.u;
            res.push_back({edge_id, target, e.active});
        }
        return res;
    }

    string Move(string path_id) {
        if (!edges.contains(path_id)) return "FAIL";

        Edge e = edges[path_id];
        if (e.u != current_pos && e.v != current_pos) {
            return "FAIL";
        }

        if (e.active == true) {
            if (e.u == current_pos) {
                current_pos = e.v;
            } else {
                current_pos = e.u;
            }

            if (!visited_rooms.contains(current_pos)) {
                visited_rooms.insert(current_pos);
                collected_profit = collected_profit + vertices[current_pos];
            }
            return "OK";
        }
        return "FAIL";
    }

    string Clear(string path_id) {
        if (!edges.contains(path_id)) return "FAIL";

        days = days + 1;
        edges[path_id].active = true;
        current_pos = entrance;
        return "OK";
    }

    string End() {
        int net = collected_profit - (days * day_cost);
        return format("RESULT {} {} {}", collected_profit, days, net);
    }

    int getDayCost() { return day_cost; }
    string getEntrance() { return entrance; }
    unordered_map<string, int> getVertices() { return vertices; }
    unordered_map<string, Edge> getEdges() { return edges; }
    unordered_map<string, vector<string>> getAdj() { return adj; }

private:
    int day_cost;
    string entrance;
    string current_pos;
    int days = 0;
    int collected_profit = 0;

    unordered_map<string, int> vertices;
    unordered_map<string, Edge> edges;
    unordered_map<string, vector<string>> adj;
    unordered_set<string> visited_rooms;

    std::random_device rd;
    std::mt19937 gen;
};