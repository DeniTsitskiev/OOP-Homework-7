cmake_minimum_required(VERSION 3.10)
project(Laboratory_7)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

find_package(Threads REQUIRED)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.0
    TLS_VERIFY false
)

# Отключаем использование статической библиотеки и переключаем на динамическую
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SOURCES
    src/npc.cpp
    src/knight.cpp
    src/druid.cpp
    src/elf.cpp
    src/factory.cpp
    src/arena.cpp
    src/combat_visitor.cpp
    src/game_engine.cpp
)

add_library(${PROJECT_NAME}_lib ${SOURCES})
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_lib PRIVATE Threads::Threads)

add_executable(${PROJECT_NAME}_exe main.cpp)
target_link_libraries(${PROJECT_NAME}_exe PRIVATE ${PROJECT_NAME}_lib)

# Добавление тестов

# Тесты для NPC
add_executable(${PROJECT_NAME}_test_npc tests/test_npc.cpp)
target_link_libraries(${PROJECT_NAME}_test_npc PRIVATE ${PROJECT_NAME}_lib gtest_main)
add_test(NAME NpcTest COMMAND ${PROJECT_NAME}_test_npc)

add_executable(${PROJECT_NAME}_test_factory tests/test_factory.cpp)
target_link_libraries(${PROJECT_NAME}_test_factory PRIVATE ${PROJECT_NAME}_lib gtest_main)
add_test(NAME FactoryTest COMMAND ${PROJECT_NAME}_test_factory)

add_executable(${PROJECT_NAME}_test_arena tests/test_arena.cpp)
target_link_libraries(${PROJECT_NAME}_test_arena PRIVATE ${PROJECT_NAME}_lib gtest_main)
add_test(NAME ArenaTest COMMAND ${PROJECT_NAME}_test_arena)

# Тесты для Combat
add_executable(${PROJECT_NAME}_test_combat tests/test_combat.cpp)
target_link_libraries(${PROJECT_NAME}_test_combat PRIVATE ${PROJECT_NAME}_lib gtest_main)
add_test(NAME CombatTest COMMAND ${PROJECT_NAME}_test_combat)

# Тесты для загрузки файлов
add_executable(${PROJECT_NAME}_test_file_loading tests/test_file_loading.cpp)
target_link_libraries(${PROJECT_NAME}_test_file_loading PRIVATE ${PROJECT_NAME}_lib gtest_main)
add_test(NAME FileLoadingTest COMMAND ${PROJECT_NAME}_test_file_loading)

# Копируем тестовые файлы в директорию сборки
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_data_npcs.txt
    ${CMAKE_CURRENT_BINARY_DIR}/test_data_npcs.txt
    COPYONLY
)