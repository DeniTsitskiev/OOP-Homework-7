#include "../include/knight.h"
#include "../include/visitor.h"
#include <iostream>
#include <cmath>
#include <ostream>

Knight::Knight(int x, int y, const std::string& name)
    : Npc(x, y, kType, name) {}

const std::string Knight::kType = "Knight";

void Knight::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void Knight::printInfo() const {
    // Специфическая информация для рыцаря
    std::cout << "Knight Info - Name: " << getName()
              << ", Position: (" << getX() << ", " << getY() << ")"
              << std::endl;
}

