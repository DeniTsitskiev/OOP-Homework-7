#include <gtest/gtest.h>
#include "../include/arena.h"
#include "../include/factory.h"
#include "../include/console_observer.h"
#include <memory>
#include <fstream>

// Тест загрузки реального файла с данными
TEST(FileLoadingTest, LoadRealFileAndVerify) {
    Arena arena;
    
    // Загружаем файл с тестовыми данными
    ASSERT_NO_THROW({
        arena.loadFromFile("test_data_npcs.txt");
    });
    
    // Проверяем количество загруженных NPC
    EXPECT_EQ(arena.getNpcCount(), 5);
}

TEST(FileLoadingTest, LoadAndPrintNpcs) {
    Arena arena;
    auto observer = std::make_shared<ConsoleObserver>();
    arena.addObserver(observer);
    
    std::cout << "\n=== Загрузка NPC из файла test_data_npcs.txt ===" << std::endl;
    
    arena.loadFromFile("test_data_npcs.txt");
    
    std::cout << "Загружено NPC: " << arena.getNpcCount() << std::endl;
    std::cout << "\nСписок загруженных NPC:" << std::endl;
    arena.printAllNpcs();
    
    EXPECT_EQ(arena.getNpcCount(), 5);
}

TEST(FileLoadingTest, LoadAndTestBattle) {
    Arena arena;
    auto observer = std::make_shared<ConsoleObserver>();
    arena.addObserver(observer);
    
    // Загружаем NPC
    arena.loadFromFile("test_data_npcs.txt");
    EXPECT_EQ(arena.getNpcCount(), 5);
    
    std::cout << "\n=== Тест боя с загруженными NPC ===" << std::endl;
    std::cout << "Начальное количество NPC: " << arena.getNpcCount() << std::endl;
    
    // Запускаем бой с большой дальностью
    std::cout << "\nЗапуск боя с дальностью 200 метров..." << std::endl;
    arena.startBattle(200.0);
    
    std::cout << "Количество выживших: " << arena.getNpcCount() << std::endl;
    std::cout << "\nВыжившие:" << std::endl;
    arena.printAllNpcs();
    
    // После боя должно остаться меньше NPC
    EXPECT_LT(arena.getNpcCount(), 5);
}

TEST(FileLoadingTest, LoadSaveAndReload) {
    std::string tempFile = "temp_test_file.txt";
    
    // Первая арена - загружаем и сохраняем
    {
        Arena arena1;
        arena1.loadFromFile("test_data_npcs.txt");
        size_t originalCount = arena1.getNpcCount();
        
        EXPECT_EQ(originalCount, 5);
        
        // Сохраняем в другой файл
        arena1.saveToFile(tempFile);
    }
    
    // Вторая арена - загружаем из сохранённого файла
    {
        Arena arena2;
        arena2.loadFromFile(tempFile);
        
        EXPECT_EQ(arena2.getNpcCount(), 5);
        
        std::cout << "\n=== Проверка сохранения/загрузки ===" << std::endl;
        std::cout << "Данные успешно сохранены и загружены обратно!" << std::endl;
        arena2.printAllNpcs();
    }
    
    // Удаляем временный файл
    std::remove(tempFile.c_str());
}

TEST(FileLoadingTest, VerifySpecificNpcData) {
    Arena arena;
    arena.loadFromFile("test_data_npcs.txt");
    
    // Проверяем что все 5 NPC загрузились
    EXPECT_EQ(arena.getNpcCount(), 5);
    
    // Создаём арену с дубликатом имени - должна выбросить исключение
    EXPECT_THROW({
        arena.addNpc(NpcFactory::createNpc("Knight", "Lancelot", 500, 500));
    }, std::invalid_argument);
}

TEST(FileLoadingTest, BattleWithLoadedNpcs) {
    Arena arena;
    auto observer = std::make_shared<ConsoleObserver>();
    arena.addObserver(observer);
    
    arena.loadFromFile("test_data_npcs.txt");
    
    std::cout << "\n=== Детальный тест боя ===" << std::endl;
    std::cout << "Загружено NPC: " << arena.getNpcCount() << std::endl;
    
    // Lancelot (100,200) и Legolas (50,75) - расстояние ~136 метров
    // Merlin (150,250) близко к Lancelot - ~71 метр
    
    std::cout << "\nБой с дальностью 100 метров (только близкие NPC):" << std::endl;
    arena.startBattle(100.0);
    
    std::cout << "\nВыжившие:" << std::endl;
    arena.printAllNpcs();
    std::cout << "Осталось NPC: " << arena.getNpcCount() << std::endl;
    
    EXPECT_GE(arena.getNpcCount(), 1);  // Хотя бы кто-то должен выжить
}
