#include "../include/druid.h"
#include "../include/visitor.h"
#include <iostream>

Druid::Druid(int x, int y, const std::string& name)
    : Npc(x, y, kType, name) {}

const std::string Druid::kType = "Druid";


void Druid::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void Druid::printInfo() const {
    // Специфическая информация для друида
    std::cout << "Druid Info - Name: " << getName()
              << ", Position: (" << getX() << ", " << getY() << ")"
              << std::endl;
}

