#pragma once
#include <string>

class Observer {
    public:
    virtual ~Observer() = default;

    // Метод уведомления об изменениях в NPC
    virtual void notify(const std::string& npcName) = 0;
};