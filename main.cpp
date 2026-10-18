#include <iostream>
#include <unordered_map>
#include "GameScene.h"
#include "OptimalSolution.h"

using std::cout;
using std::string;
using std::unordered_map;

int main() {
    unordered_map<string, int> vertices = {
        {"ШАХТА", 0},
        {"ЗАЛ-А", 10},
        {"ЗАЛ-Б", 20},
        {"ЗАЛ-В", 12}
    };

    unordered_map<string, Edge> edges = {
        {"х1", Edge("х1", "ШАХТА", "ЗАЛ-А", 0.4)},
        {"х2", Edge("х2", "ЗАЛ-А", "ЗАЛ-Б", 0.9)},
        {"х3", Edge("х3", "ШАХТА", "ЗАЛ-В", 0.9)}
    };

    int day_cost = 4;
    string entrance = "ШАХТА";

    GameScene game(day_cost, entrance, vertices, edges);
    // Прогон 1
    cout << "========Прогон 1========\n";
    game.setCustomLevel({
        {"х1", true},
        {"х2", false},
        {"х3", true}
    });
    OptimalSolution solver(game);
    solver.Solution();
    // Прогон 2
    cout << "========Прогон 2========\n";
    game.setCustomLevel({
        {"х1", false},
        {"х2", true},
        {"х3", true}
    });
    OptimalSolution solver2(game);
    solver2.Solution();
    // Прогон 3
    game.setDayCost(50);
    cout << "========Прогон 3========\n";
    game.setCustomLevel({
        {"х1", false},
        {"х2", false},
        {"х3", false}
    });
    OptimalSolution solver3(game);
    solver3.Solution();
}