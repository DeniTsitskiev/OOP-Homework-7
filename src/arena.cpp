#include "../include/arena.h"
#include "../include/factory.h"
#include "../include/combat_visitor.h"
#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

Arena::Arena(int width, int height) {
    if (width > MAX_WIDTH || height > MAX_HEIGHT) {
        throw std::out_of_range("Arena size exceeds maximum limits.");
    }
    this->width_ = width;
    this->height_ = height;
}

void Arena::addNpc(std::unique_ptr<Npc> npc) {
    const std::string name  = npc->getName();

    if (npc->getX() < 0 || npc->getX() > width_ ||
        npc->getY() < 0 || npc->getY() > height_) {
        throw std::out_of_range("NPC position is out of arena bounds.");
    }

    if (npcs_.find(name) != npcs_.end()) {
        throw std::invalid_argument("NPC with this name already exists.");
    }
    npcs_[name] = std::move(npc);
}

void Arena::createAndAddNpc(const std::string& type, 
                            const std::string& name, 
                            int x, int y) {
    auto npc = NpcFactory::createNpc(type, name, x, y);
    addNpc(std::move(npc));
}

void Arena::printAllNpcs() const {
    for (const auto& pair : npcs_) {
        std::cout << *(pair.second) << std::endl;
    }
}

size_t Arena::getNpcCount() const {
    return npcs_.size();
}


void Arena::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }

    for (const auto& [name, npc] : npcs_) {
        file << npc->getType() << " "
             << npc->getName() << " "
             << npc->getX() << " "
             << npc->getY() << std::endl;
    }
}


void Arena::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        auto npc = NpcFactory::createFromString(line);

        addNpc(std::move(npc));
    }
}


void Arena::clear() {
    npcs_.clear();
}



void Arena::addObserver(std::shared_ptr<Observer> observer) {
    observers_.push_back(observer);
}

void Arena::removeObserver(std::shared_ptr<Observer> observer) {
    std::erase(observers_, observer);
}


void Arena::notifyObservers(const std::string& event) {
    for (auto& observer : observers_) {
        observer->notify(event);
    }
}


void Arena::startBattle(double range) {
    CombatVisitor visitor;
    std::vector<std::string> toRemove; // Имена NPC для удаления

    // Проверяем каждую пару NPC
    for (auto& [name1, npc1] : npcs_) {
        for (auto& [name2, npc2] : npcs_) {
            if (name1 == name2) continue; // Пропускаем самого себя
            if (name1 > name2) continue;  // Избегаем дублирования пар (A,B) и (B,A)

            // Проверяем расстояние
            if (npc1->distanceTo(*npc2) > range) continue;
            
            // Проверяем бой в обе стороны
            bool npc1KillsNpc2 = visitor.canKill(npc1.get(), npc2.get());
            bool npc2KillsNpc1 = visitor.canKill(npc2.get(), npc1.get());
            
            if (npc1KillsNpc2 && npc2KillsNpc1) {
                // Оба убивают друг друга
                std::string event = npc1->getName() + " (" + npc1->getType() + 
                                   ") and " + npc2->getName() + " (" + npc2->getType() + 
                                   ") killed each other";
                notifyObservers(event);
                toRemove.push_back(name1);
                toRemove.push_back(name2);
            } else if (npc1KillsNpc2) {
                // Только npc1 убивает npc2
                std::string event = npc1->getName() + " (" + npc1->getType() + 
                                   ") killed " + npc2->getName() + " (" + npc2->getType() + ")";
                notifyObservers(event);
                toRemove.push_back(name2);
            } else if (npc2KillsNpc1) {
                // Только npc2 убивает npc1
                std::string event = npc2->getName() + " (" + npc2->getType() + 
                                   ") killed " + npc1->getName() + " (" + npc1->getType() + ")";
                notifyObservers(event);
                toRemove.push_back(name1);
            }
        }
    }
    
    // Удаляем убитых NPC (убираем дубликаты)
    std::sort(toRemove.begin(), toRemove.end());
    toRemove.erase(std::unique(toRemove.begin(), toRemove.end()), toRemove.end());
    
    for (const auto& name : toRemove) {
        npcs_.erase(name);
    }
}