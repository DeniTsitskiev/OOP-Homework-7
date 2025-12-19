#pragma once
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <chrono>
#include "npc.h"
#include "combat_visitor.h"

struct MovementTask {
    std::string npc1_name;
    std::string npc2_name;
};

class GameEngine {
    public:
        GameEngine(int width = 100, int height = 100);
        ~GameEngine();

        // Добавление NPC
        void addNpc(std::unique_ptr<Npc> npc);
        void createRandomNpcs(int count);

        // Запуск симуляции на N секунд
        void runSimulation(int durationSeconds);

        // Получить информацию о выживших
        std::vector<std::string> getSurvivors() const;

        // Печать карты
        void printMap() const;

    private:
        int width_;
        int height_;

        // Синхронизация доступа
        mutable std::shared_mutex npcs_mutex_;
        mutable std::mutex cout_mutex_;
        std::mutex movement_queue_mutex_;

        // Хранилище NPC
        std::map<std::string, std::unique_ptr<Npc>> npcs_;

        // Очередь боевых задач
        std::queue<MovementTask> movement_tasks_;

        // Флаг для остановки потоков
        std::atomic<bool> running_;

        // Потоки
        std::thread movement_thread_;
        std::thread combat_thread_;
        std::thread display_thread_;

        // Методы для потоков
        void movementThreadFunc();
        void combatThreadFunc();
        void displayThreadFunc();

        // Таблица скоростей и дальностей атаки
        struct NpcStats {
            int movement_distance;
            int kill_distance;
        };

        NpcStats getStats(const std::string& type) const;

        // Вспомогательные методы
        void processMovement();
        void processMovement(Npc* npc);
        void detectAndQueueCombats();
        void processCombat(const MovementTask& task);
};
