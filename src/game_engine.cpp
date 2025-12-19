#include "../include/game_engine.h"
#include "../include/factory.h"
#include <iostream>
#include <random>
#include <cmath>
#include <iomanip>
#include <algorithm>

GameEngine::GameEngine(int width, int height)
    : width_(width), height_(height), running_(false) {}

GameEngine::~GameEngine() {
    running_ = false;
    if (movement_thread_.joinable()) movement_thread_.join();
    if (combat_thread_.joinable()) combat_thread_.join();
    if (display_thread_.joinable()) display_thread_.join();
}

GameEngine::NpcStats GameEngine::getStats(const std::string& type) const {
    if (type == "Knight") return {30, 10};
    if (type == "Druid") return {10, 10};
    if (type == "Elf") return {10, 50};
    return {0, 0};
}

void GameEngine::addNpc(std::unique_ptr<Npc> npc) {
    std::unique_lock<std::shared_mutex> lock(npcs_mutex_);
    npcs_[npc->getName()] = std::move(npc);
}

void GameEngine::createRandomNpcs(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> x_dist(0, width_ - 1);
    std::uniform_int_distribution<> y_dist(0, height_ - 1);

    std::vector<std::string> types = {
        "Knight", "Druid", "Elf"
    };

    std::uniform_int_distribution<> type_dist(0, types.size() - 1);

    for (int i = 0; i < count; ++i) {
        std::string type = types[type_dist(gen)];
        std::string name = type + "_" + std::to_string(i);
        int x = x_dist(gen);
        int y = y_dist(gen);

        try {
            auto npc = NpcFactory::createNpc(type, name, x, y);
            addNpc(std::move(npc));
        } catch (const std::exception& e) {
            std::cerr << "Error creating NPC: " << e.what() << std::endl;
        }
    }
}

void GameEngine::movementThreadFunc() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        processMovement();
        detectAndQueueCombats();
    }
}

void GameEngine::processMovement() {
    std::vector<std::string> npc_names;
    
    {
        std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
        for (const auto& [name, npc] : npcs_) {
            if (npc && npc->isAlive()) {
                npc_names.push_back(name);
            }
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dir_dist(0, 3); // 4 направления

    for (const auto& name : npc_names) {
        std::unique_lock<std::shared_mutex> lock(npcs_mutex_);
        auto it = npcs_.find(name);
        if (it != npcs_.end() && it->second) {
            processMovement(it->second.get());
        }
    }
}

void GameEngine::processMovement(Npc* npc) {
    if (!npc || !npc->isAlive()) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dir_dist(0, 3); // 4 направления
    std::uniform_int_distribution<> dist_dist(1, getStats(npc->getType()).movement_distance);

    int direction = dir_dist(gen);
    int distance = dist_dist(gen);

    int new_x = npc->getX();
    int new_y = npc->getY();

    switch (direction) {
        case 0: new_x = std::min(width_ - 1, new_x + distance); break; // Right
        case 1: new_x = std::max(0, new_x - distance); break; // Left
        case 2: new_y = std::min(height_ - 1, new_y + distance); break; // Down
        case 3: new_y = std::max(0, new_y - distance); break; // Up
    }

    npc->setX(new_x);
    npc->setY(new_y);
}

void GameEngine::detectAndQueueCombats() {
    std::vector<std::string> names;
    
    {
        std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
        for (const auto& [name, npc] : npcs_) {
            if (npc && npc->isAlive()) {
                names.push_back(name);
            }
        }
    }

    CombatVisitor visitor;

    for (size_t i = 0; i < names.size(); ++i) {
        for (size_t j = i + 1; j < names.size(); ++j) {
            std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
            auto npc1_it = npcs_.find(names[i]);
            auto npc2_it = npcs_.find(names[j]);

            if (npc1_it != npcs_.end() && npc2_it != npcs_.end() &&
                npc1_it->second && npc2_it->second &&
                npc1_it->second->isAlive() && npc2_it->second->isAlive()) {

                double distance = npc1_it->second->distanceTo(*npc2_it->second);
                int kill_dist_1 = getStats(npc1_it->second->getType()).kill_distance;
                int kill_dist_2 = getStats(npc2_it->second->getType()).kill_distance;

                if (distance <= std::max(kill_dist_1, kill_dist_2)) {
                    if (visitor.canKill(npc1_it->second.get(), npc2_it->second.get()) ||
                        visitor.canKill(npc2_it->second.get(), npc1_it->second.get())) {
                        std::lock_guard<std::mutex> task_lock(movement_queue_mutex_);
                        movement_tasks_.push({names[i], names[j]});
                    }
                }
            }
        }
    }
}

void GameEngine::combatThreadFunc() {
    CombatVisitor visitor;

    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        MovementTask task;
        {
            std::lock_guard<std::mutex> lock(movement_queue_mutex_);
            if (movement_tasks_.empty()) continue;
            task = movement_tasks_.front();
            movement_tasks_.pop();
        }

        processCombat(task);
    }
}

void GameEngine::processCombat(const MovementTask& task) {
    std::unique_lock<std::shared_mutex> lock(npcs_mutex_);

    auto npc1_it = npcs_.find(task.npc1_name);
    auto npc2_it = npcs_.find(task.npc2_name);

    if (npc1_it == npcs_.end() || npc2_it == npcs_.end()) return;
    if (!npc1_it->second || !npc2_it->second) return;
    if (!npc1_it->second->isAlive() || !npc2_it->second->isAlive()) return;

    CombatVisitor visitor;

    bool npc1_attacks = visitor.canKill(npc1_it->second.get(), npc2_it->second.get());
    bool npc2_attacks = visitor.canKill(npc2_it->second.get(), npc1_it->second.get());

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dice(1, 6);

    if (npc1_attacks) {
        int npc1_attack = dice(gen);
        int npc2_defense = dice(gen);

        if (npc1_attack > npc2_defense) {
            npc2_it->second->kill();
            {
                std::lock_guard<std::mutex> cout_lock(cout_mutex_);
                std::cout << "[COMBAT] " << npc1_it->second->getName()
                         << " killed " << npc2_it->second->getName() << std::endl;
            }
        }
    }

    if (npc2_attacks && npc1_it->second->isAlive()) {
        int npc2_attack = dice(gen);
        int npc1_defense = dice(gen);

        if (npc2_attack > npc1_defense) {
            npc1_it->second->kill();
            {
                std::lock_guard<std::mutex> cout_lock(cout_mutex_);
                std::cout << "[COMBAT] " << npc2_it->second->getName()
                         << " killed " << npc1_it->second->getName() << std::endl;
            }
        }
    }
}

void GameEngine::displayThreadFunc() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        printMap();
    }
}

void GameEngine::printMap() const {
    std::lock_guard<std::mutex> cout_lock(const_cast<std::mutex&>(cout_mutex_));
    std::shared_lock<std::shared_mutex> data_lock(npcs_mutex_);

    // Создаём двумерный массив для карты
    std::vector<std::vector<char>> map(height_, std::vector<char>(width_, '.'));
    
    int alive_count = 0;
    std::vector<std::pair<std::string, std::pair<int, int>>> npcs_list;
    
    // Заполняем карту и считаем живых
    for (const auto& [name, npc] : npcs_) {
        if (npc && npc->isAlive()) {
            alive_count++;
            int x = npc->getX();
            int y = npc->getY();
            if (x >= 0 && x < width_ && y >= 0 && y < height_) {
                char symbol = npc->getType()[0];  // Первая буква типа
                if (map[y][x] == '.') {
                    map[y][x] = symbol;
                } else {
                    map[y][x] = '*';  // Звёздочка если несколько NPC на одной клетке
                }
            }
            npcs_list.push_back({name, {x, y}});
        }
    }

    // Выводим карту
    std::cout << "\n+====================================================================================================+\n";
    std::cout << "|                                    [MAP] GAME WORLD STATE                                         |\n";
    std::cout << "+----------------------------------------------------------------------------------------------------+\n";
    
    // Выводим шапку с номерами колонок (каждые 10)
    std::cout << "|   ";
    for (int x = 0; x < width_; x += 10) {
        std::cout << std::setw(9) << x;
    }
    std::cout << "|\n";
    
    // Выводим саму карту
    for (int y = 0; y < height_; y++) {
        std::cout << "| " << std::setw(2) << y << " ";
        for (int x = 0; x < width_; x++) {
            std::cout << map[y][x];
        }
        std::cout << " |\n";
    }
    
    std::cout << "+----------------------------------------------------------------------------------------------------+\n";
    std::cout << "| Legend: . = empty, * = multiple NPCs, Letter = NPC type (K=Knight, D=Druid, E=Elf, O=Orc, etc.)  |\n";
    std::cout << "| Alive NPCs: " << alive_count << "/" << npcs_.size();
    for (int i = alive_count; i < 12; i++) std::cout << " ";
    std::cout << "|\n";
    std::cout << "+====================================================================================================+\n";
    std::cout << std::flush;  // Принудительный сброс буфера для Docker
}

void GameEngine::runSimulation(int durationSeconds) {
    running_ = true;

    {
        std::lock_guard<std::mutex> cout_lock(cout_mutex_);
        std::cout << "=== Starting Game Simulation ===" << std::endl;
        std::cout << "Map size: " << width_ << " x " << height_ << std::endl;
        std::cout << "Duration: " << durationSeconds << " seconds" << std::endl;
    }

    movement_thread_ = std::thread(&GameEngine::movementThreadFunc, this);
    combat_thread_ = std::thread(&GameEngine::combatThreadFunc, this);
    display_thread_ = std::thread(&GameEngine::displayThreadFunc, this);

    std::this_thread::sleep_for(std::chrono::seconds(durationSeconds));

    running_ = false;

    movement_thread_.join();
    combat_thread_.join();
    display_thread_.join();

    {
        std::lock_guard<std::mutex> cout_lock(cout_mutex_);
        std::cout << "\n=== Simulation Ended ===" << std::endl;
        std::cout << "Survivors:" << std::endl;
        std::shared_lock<std::shared_mutex> data_lock(npcs_mutex_);
        for (const auto& [name, npc] : npcs_) {
            if (npc && npc->isAlive()) {
                std::cout << "  " << *npc << std::endl;
            }
        }
    }
}

std::vector<std::string> GameEngine::getSurvivors() const {
    std::vector<std::string> survivors;
    std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
    
    for (const auto& [name, npc] : npcs_) {
        if (npc && npc->isAlive()) {
            survivors.push_back(name);
        }
    }
    
    return survivors;
}
