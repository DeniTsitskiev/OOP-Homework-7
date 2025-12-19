#include <gtest/gtest.h>
#include "../include/npc.h"
#include "../include/knight.h"
#include "../include/druid.h"
#include "../include/elf.h"
#include <memory>

// Тесты создания NPC
TEST(NpcTest, CreateKnight) {
    Knight knight(100, 200, "Lancelot");
    
    EXPECT_EQ(knight.getX(), 100);
    EXPECT_EQ(knight.getY(), 200);
    EXPECT_EQ(knight.getName(), "Lancelot");
    EXPECT_EQ(knight.getType(), "Knight");
}

TEST(NpcTest, CreateDruid) {
    Druid druid(150, 250, "Merlin");
    
    EXPECT_EQ(druid.getX(), 150);
    EXPECT_EQ(druid.getY(), 250);
    EXPECT_EQ(druid.getName(), "Merlin");
    EXPECT_EQ(druid.getType(), "Druid");
}

TEST(NpcTest, CreateElf) {
    Elf elf(50, 75, "Legolas");
    
    EXPECT_EQ(elf.getX(), 50);
    EXPECT_EQ(elf.getY(), 75);
    EXPECT_EQ(elf.getName(), "Legolas");
    EXPECT_EQ(elf.getType(), "Elf");
}

// Тест расстояния между NPC
TEST(NpcTest, DistanceCalculation) {
    Knight knight1(0, 0, "Knight1");
    Knight knight2(3, 4, "Knight2");
    
    // Расстояние должно быть 5 (теорема Пифагора: 3^2 + 4^2 = 25, sqrt(25) = 5)
    EXPECT_DOUBLE_EQ(knight1.distanceTo(knight2), 5.0);
}

TEST(NpcTest, DistanceSamePosition) {
    Elf elf1(100, 100, "Elf1");
    Elf elf2(100, 100, "Elf2");
    
    EXPECT_DOUBLE_EQ(elf1.distanceTo(elf2), 0.0);
}

TEST(NpcTest, DistanceSymmetric) {
    Druid druid(10, 20, "Druid1");
    Knight knight(50, 80, "Knight1");
    
    // Расстояние должно быть одинаковым в обе стороны
    EXPECT_DOUBLE_EQ(druid.distanceTo(knight), knight.distanceTo(druid));
}

// Тест координат на границах
TEST(NpcTest, BoundaryCoordinates) {
    Knight knight1(0, 0, "MinCorner");
    EXPECT_EQ(knight1.getX(), 0);
    EXPECT_EQ(knight1.getY(), 0);
    
    Knight knight2(500, 500, "MaxCorner");
    EXPECT_EQ(knight2.getX(), 500);
    EXPECT_EQ(knight2.getY(), 500);
}
