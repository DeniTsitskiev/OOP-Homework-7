# Multi-stage build для оптимизации размера образа
# Этап 1: Сборка проекта
FROM alpine:latest AS builder

# Установка инструментов для сборки (Alpine использует apk)
RUN apk add --no-cache \
    g++ \
    gcc \
    cmake \
    make \
    git \
    linux-headers \
    musl-dev

# Установка рабочей директории
WORKDIR /app

# Копирование файлов проекта
COPY CMakeLists.txt ./
COPY main.cpp ./
COPY include/ ./include/
COPY src/ ./src/
COPY tests/ ./tests/

# Сборка проекта в Release режиме
RUN mkdir -p build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -j$(nproc)

# Этап 2: Финальный образ только с необходимыми файлами
FROM alpine:latest

# Установка только runtime зависимостей
RUN apk add --no-cache \
    libstdc++ \
    libgcc

# Установка рабочей директории
WORKDIR /app

COPY --from=builder /app/build/Laboratory_7_exe ./lab7_main
COPY --from=builder /app/build/Laboratory_7_test_arena ./tests/test_arena
COPY --from=builder /app/build/Laboratory_7_test_combat ./tests/test_combat
COPY --from=builder /app/build/Laboratory_7_test_npc ./tests/test_npc
COPY --from=builder /app/build/Laboratory_7_test_factory ./tests/test_factory
COPY --from=builder /app/build/Laboratory_7_test_file_loading ./tests/test_file_loading

RUN mkdir -p tests

# Копирование данных для тестов
COPY --from=builder /app/tests/test_data_npcs.txt ./tests/

# CMD по умолчанию - запуск основной программы (ЛР 7)
CMD ["./lab7_main"]
