#include "../include/combat_visitor.h"

bool CombatVisitor::canKill(Npc* attacker, Npc* defender) {
    if (attacker->getType() == "Knight") {
        return knightVs(defender->getType());
    } else if (attacker->getType() == "Druid") {
        return druidVs(defender->getType());
    } else if (attacker->getType() == "Elf") {
        return elfVs(defender->getType());
    }
    return false;
}

bool CombatVisitor::knightVs(const std::string& defenderType) {
    return (defenderType == "Elf");
}

bool CombatVisitor::druidVs(const std::string& defenderType) {
    return (defenderType == "Druid");
}

bool CombatVisitor::elfVs(const std::string& defenderType) {
    return (defenderType == "Druid" || defenderType == "Knight");
}