#include "../include/npc.h"
#include <cmath>
#include <ostream>
#include <iostream>

Npc::Npc(int x, int y, const std::string& type, const std::string& name)
    : x_(x), y_(y), type_(type), name_(name), alive_(true) {}

int Npc::getX() const {
    return x_;
}

int Npc::getY() const {
    return y_;
}

std::string Npc::getType() const {
    return type_;
}

std::string Npc::getName() const {
    return name_;
}

bool Npc::isAlive() const {
    return alive_;
}

void Npc::setX(int x) {
    x_ = x;
}

void Npc::setY(int y) {
    y_ = y;
}

void Npc::kill() {
    alive_ = false;
}

double Npc::distanceTo(const Npc& other) const {
    int dx = x_ - other.x_;
    int dy = y_ - other.y_;
    return std::sqrt(dx * dx + dy * dy);
}

void Npc::printInfo() const {
    std::cout << this << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Npc& npc) {
    os << "NPC Type: " << npc.type_ << ", Name: " << npc.name_
       << ", Position: (" << npc.x_ << ", " << npc.y_ << ")"
       << ", Status: " << (npc.alive_ ? "Alive" : "Dead");
    return os;
}