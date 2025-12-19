#pragma once
#include <string>
#include "npc.h"
#include <map>
#include <memory>
#include "observer.h"
#include <vector>

#define MAX_WIDTH 500
#define MAX_HEIGHT 500


class Arena {
    public:
        Arena(int width = MAX_WIDTH, int height = MAX_HEIGHT);

        // Добавление NPC на арену
        void addNpc(std::unique_ptr<Npc> npc);

        void createAndAddNpc(const std::string& type, 
                         const std::string& name, 
                         int x, int y);

        // Вывод информации обо всех NPC
        void printAllNpcs() const;

        // Получение количества NPC
        size_t getNpcCount() const;

        // Управление наблюдателями
        void addObserver(std::shared_ptr<Observer> observer);

        void removeObserver(std::shared_ptr<Observer> observer);

        // Управление боем
        void startBattle(double range);

        // Сохранение в файл
        void saveToFile(const std::string& filename) const;

        // Загрузка из файла
        void loadFromFile(const std::string& filename);

        // Очистка арены
        void clear();
    
    private:
        int width_;
        int height_;
        std::map<std::string, std::unique_ptr<Npc>> npcs_;

        std::vector<std::shared_ptr<Observer>> observers_;

        void notifyObservers(const std::string& event);
};