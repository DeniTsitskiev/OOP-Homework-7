#include <gtest/gtest.h>
#include "../include/combat_visitor.h"
#include "../include/arena.h"
#include "../include/factory.h"
#include "../include/console_observer.h"
#include "../include/file_observer.h"
#include <memory>
#include <fstream>

// Тесты боевой системы
TEST(CombatTest, KnightVsKnight) {
    CombatVisitor visitor;
    auto knight1 = NpcFactory::createNpc("Knight", "Knight1", 0, 0);
    auto knight2 = NpcFactory::createNpc("Knight", "Knight2", 10, 10);
    
    // Рыцарь не атакует рыцаря
    EXPECT_FALSE(visitor.canKill(knight1.get(), knight2.get()));
    EXPECT_FALSE(visitor.canKill(knight2.get(), knight1.get()));
}

TEST(CombatTest, KnightVsElf) {
    CombatVisitor visitor;
    auto knight = NpcFactory::createNpc("Knight", "Knight1", 0, 0);
    auto elf = NpcFactory::createNpc("Elf", "Elf1", 10, 10);
    
    // Рыцарь убивает эльфа
    EXPECT_TRUE(visitor.canKill(knight.get(), elf.get()));
    // Эльф убивает рыцаря
    EXPECT_TRUE(visitor.canKill(elf.get(), knight.get()));
}

TEST(CombatTest, KnightVsDruid) {
    CombatVisitor visitor;
    auto knight = NpcFactory::createNpc("Knight", "Knight1", 0, 0);
    auto druid = NpcFactory::createNpc("Druid", "Druid1", 10, 10);
    
    // Рыцарь не атакует друида
    EXPECT_FALSE(visitor.canKill(knight.get(), druid.get()));
    // Друид не атакует рыцаря
    EXPECT_FALSE(visitor.canKill(druid.get(), knight.get()));
}

TEST(CombatTest, DruidVsDruid) {
    CombatVisitor visitor;
    auto druid1 = NpcFactory::createNpc("Druid", "Druid1", 0, 0);
    auto druid2 = NpcFactory::createNpc("Druid", "Druid2", 10, 10);
    
    // Друид убивает друида
    EXPECT_TRUE(visitor.canKill(druid1.get(), druid2.get()));
    EXPECT_TRUE(visitor.canKill(druid2.get(), druid1.get()));
}

TEST(CombatTest, ElfVsDruid) {
    CombatVisitor visitor;
    auto elf = NpcFactory::createNpc("Elf", "Elf1", 0, 0);
    auto druid = NpcFactory::createNpc("Druid", "Druid1", 10, 10);
    
    // Эльф убивает друида
    EXPECT_TRUE(visitor.canKill(elf.get(), druid.get()));
    // Друид убивает друида (но не эльфа)
    EXPECT_FALSE(visitor.canKill(druid.get(), elf.get()));
}

TEST(CombatTest, ElfVsKnight) {
    CombatVisitor visitor;
    auto elf = NpcFactory::createNpc("Elf", "Elf1", 0, 0);
    auto knight = NpcFactory::createNpc("Knight", "Knight1", 10, 10);
    
    // Эльф убивает рыцаря
    EXPECT_TRUE(visitor.canKill(elf.get(), knight.get()));
}

TEST(CombatTest, ElfVsElf) {
    CombatVisitor visitor;
    auto elf1 = NpcFactory::createNpc("Elf", "Elf1", 0, 0);
    auto elf2 = NpcFactory::createNpc("Elf", "Elf2", 10, 10);
    
    // Эльф не атакует эльфа
    EXPECT_FALSE(visitor.canKill(elf1.get(), elf2.get()));
}

// Тесты боевого режима на арене
TEST(CombatTest, BattleOutOfRange) {
    Arena arena;
    auto observer = std::make_shared<ConsoleObserver>();
    arena.addObserver(observer);
    
    // NPC далеко друг от друга
    arena.addNpc(NpcFactory::createNpc("Knight", "Knight1", 0, 0));
    arena.addNpc(NpcFactory::createNpc("Druid", "Druid1", 400, 400));
    
    EXPECT_EQ(arena.getNpcCount(), 2);
    
    // Дальность боя 100 - NPC не достают друг до друга
    arena.startBattle(100.0);
    
    // Оба должны выжить
    EXPECT_EQ(arena.getNpcCount(), 2);
}

TEST(CombatTest, BattleInRange) {
    Arena arena;
    auto observer = std::make_shared<ConsoleObserver>();
    arena.addObserver(observer);
    
    // Рыцарь и эльф близко
    arena.addNpc(NpcFactory::createNpc("Knight", "Knight1", 100, 100));
    arena.addNpc(NpcFactory::createNpc("Elf", "Elf1", 110, 110));
    
    EXPECT_EQ(arena.getNpcCount(), 2);
    
    // Дальность боя 50 метров
    arena.startBattle(50.0);
    
    // Рыцарь и эльф убивают друг друга
    EXPECT_EQ(arena.getNpcCount(), 0);
}

TEST(CombatTest, MutualKill) {
    Arena arena;
    auto observer = std::make_shared<ConsoleObserver>();
    arena.addObserver(observer);
    
    // Рыцарь и эльф близко
    arena.addNpc(NpcFactory::createNpc("Knight", "Knight1", 100, 100));
    arena.addNpc(NpcFactory::createNpc("Elf", "Elf1", 105, 105));
    
    EXPECT_EQ(arena.getNpcCount(), 2);
    
    // Дальность боя 50 метров
    arena.startBattle(50.0);
    
    // Оба убивают друг друга
    EXPECT_EQ(arena.getNpcCount(), 0);
}

TEST(CombatTest, ComplexBattle) {
    Arena arena;
    auto observer = std::make_shared<ConsoleObserver>();
    arena.addObserver(observer);
    
    // Создаём группу NPC
    arena.addNpc(NpcFactory::createNpc("Knight", "Knight1", 100, 100));
    arena.addNpc(NpcFactory::createNpc("Knight", "Knight2", 110, 110));
    arena.addNpc(NpcFactory::createNpc("Druid", "Druid1", 105, 105));
    arena.addNpc(NpcFactory::createNpc("Elf", "Elf1", 115, 115));
    
    EXPECT_EQ(arena.getNpcCount(), 4);
    
    // Запускаем бой с большой дальностью
    arena.startBattle(100.0);
    
    // Рыцари убивают эльфов, эльфы убивают рыцарей и друидов, друиды убивают друидов
    // Должен остаться 0 или минимум NPC
    EXPECT_LE(arena.getNpcCount(), 2);
}

TEST(CombatTest, KnightsSurviveAlone) {
    Arena arena;
    auto observer = std::make_shared<ConsoleObserver>();
    arena.addObserver(observer);
    
    // Только рыцари
    arena.addNpc(NpcFactory::createNpc("Knight", "Knight1", 100, 100));
    arena.addNpc(NpcFactory::createNpc("Knight", "Knight2", 110, 110));
    arena.addNpc(NpcFactory::createNpc("Knight", "Knight3", 120, 120));
    
    EXPECT_EQ(arena.getNpcCount(), 3);
    
    arena.startBattle(100.0);
    
    // Рыцари не атакуют друг друга - все выживают
    EXPECT_EQ(arena.getNpcCount(), 3);
}

TEST(CombatTest, FileObserverLogging) {
    std::string logfile = "test_combat_log.txt";
    
    Arena arena;
    auto observer = std::make_shared<FileObserver>(logfile);
    arena.addObserver(observer);
    
    arena.addNpc(NpcFactory::createNpc("Knight", "Knight1", 100, 100));
    arena.addNpc(NpcFactory::createNpc("Elf", "Elf1", 110, 110));
    
    arena.startBattle(50.0);
    
    // Проверяем что файл создан
    std::ifstream file(logfile);
    ASSERT_TRUE(file.is_open());
    
    std::string line;
    bool hasContent = false;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            hasContent = true;
            break;
        }
    }
    file.close();
    
    EXPECT_TRUE(hasContent);
    
    // Удаляем тестовый файл
    std::remove(logfile.c_str());
}
