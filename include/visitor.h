#pragma once

class Knight;
class Druid;
class Elf;

class Visitor {
    public:
        virtual ~Visitor() = default;

        // Посетитель к каждому типу NPC
        virtual void visit(Knight& knight) = 0;
        virtual void visit(Druid& druid) = 0;
        virtual void visit(Elf& elf) = 0;
};
