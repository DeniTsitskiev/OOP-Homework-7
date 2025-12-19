#pragma once
#include <string>
#include "npc.h"

class Druid : public Npc {
    public:
        Druid(int x, int y, const std::string& name);

        void accept(Visitor& visitor) override;

        void printInfo() const override;

    private:
        static const std::string kType;
};

