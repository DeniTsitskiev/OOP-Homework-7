#include <gtest/gtest.h>
#include "../include/factory.h"
#include "../include/knight.h"
#include "../include/druid.h"
#include "../include/elf.h"
#include <memory>

// Тесты фабрики
TEST(FactoryTest, CreateKnight) {
    auto knight = NpcFactory::createNpc("Knight", "TestKnight", 100, 200);
    
    ASSERT_NE(knight, nullptr);
    EXPECT_EQ(knight->getType(), "Knight");
    EXPECT_EQ(knight->getName(), "TestKnight");
    EXPECT_EQ(knight->getX(), 100);
    EXPECT_EQ(knight->getY(), 200);
}

TEST(FactoryTest, CreateDruid) {
    auto druid = NpcFactory::createNpc("Druid", "TestDruid", 150, 250);
    
    ASSERT_NE(druid, nullptr);
    EXPECT_EQ(druid->getType(), "Druid");
    EXPECT_EQ(druid->getName(), "TestDruid");
}

TEST(FactoryTest, CreateElf) {
    auto elf = NpcFactory::createNpc("Elf", "TestElf", 50, 75);
    
    ASSERT_NE(elf, nullptr);
    EXPECT_EQ(elf->getType(), "Elf");
    EXPECT_EQ(elf->getName(), "TestElf");
}

TEST(FactoryTest, CreateInvalidType) {
    EXPECT_THROW({
        auto npc = NpcFactory::createNpc("Dragon", "TestDragon", 100, 100);
    }, std::invalid_argument);
}

// Тесты загрузки из строки
TEST(FactoryTest, CreateFromStringKnight) {
    std::string line = "Knight Lancelot 100 200";
    auto knight = NpcFactory::createFromString(line);
    
    ASSERT_NE(knight, nullptr);
    EXPECT_EQ(knight->getType(), "Knight");
    EXPECT_EQ(knight->getName(), "Lancelot");
    EXPECT_EQ(knight->getX(), 100);
    EXPECT_EQ(knight->getY(), 200);
}

TEST(FactoryTest, CreateFromStringDruid) {
    std::string line = "Druid Merlin 300 400";
    auto druid = NpcFactory::createFromString(line);
    
    ASSERT_NE(druid, nullptr);
    EXPECT_EQ(druid->getType(), "Druid");
    EXPECT_EQ(druid->getName(), "Merlin");
    EXPECT_EQ(druid->getX(), 300);
    EXPECT_EQ(druid->getY(), 400);
}

TEST(FactoryTest, CreateFromStringElf) {
    std::string line = "Elf Legolas 50 75";
    auto elf = NpcFactory::createFromString(line);
    
    ASSERT_NE(elf, nullptr);
    EXPECT_EQ(elf->getType(), "Elf");
    EXPECT_EQ(elf->getName(), "Legolas");
}

TEST(FactoryTest, CreateFromStringInvalidFormat) {
    std::string line = "Knight Lancelot";  // Нет координат
    EXPECT_THROW({
        auto npc = NpcFactory::createFromString(line);
    }, std::runtime_error);
}

TEST(FactoryTest, CreateFromStringInvalidType) {
    std::string line = "Wolf Wolfie 100 200";
    EXPECT_THROW({
        auto npc = NpcFactory::createFromString(line);
    }, std::invalid_argument);
}

TEST(FactoryTest, CreateFromStringEmptyLine) {
    std::string line = "";
    EXPECT_THROW({
        auto npc = NpcFactory::createFromString(line);
    }, std::runtime_error);
}
