#include "../include/elf.h"
#include "../include/visitor.h"
#include <iostream>

Elf::Elf(int x, int y, const std::string& name)
    : Npc(x, y, kType, name) {}

const std::string Elf::kType = "Elf";

void Elf::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void Elf::printInfo() const {
    // Специфическая информация для эльфа
    std::cout << "Elf Info - Name: " << getName()
              << ", Position: (" << getX() << ", " << getY() << ")"
              << std::endl;
}

