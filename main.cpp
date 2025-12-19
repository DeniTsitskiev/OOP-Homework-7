#include "include/game_engine.h"
#include <iostream>
#include <memory>

int main() {
    try {
        GameEngine engine(100, 100);
        engine.createRandomNpcs(50);
        engine.runSimulation(30);
        
        auto survivors = engine.getSurvivors();
        std::cout << "\nSurvivors: " << survivors.size() << "/50" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
