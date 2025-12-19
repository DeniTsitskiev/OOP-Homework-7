#pragma once
#include "visitor.h"
#include "npc.h"

class CombatVisitor : public Visitor {
    public:
        bool canKill(Npc* attacker, Npc* defender);

        void visit(Knight&) override {}
        void visit(Druid&) override {}
        void visit(Elf&) override {}

    private:
        bool knightVs(const std::string& defenderType);
        bool druidVs(const std::string& defenderType);
        bool elfVs(const std::string& defenderType);
};