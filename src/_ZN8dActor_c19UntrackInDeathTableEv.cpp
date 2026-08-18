//cpp
extern "C" void DeathTable_ClearBit(int id);

class Actor {
public:
    char pad[0xce];
    short deathTableId;

    void UntrackInDeathTable();
};

void Actor::UntrackInDeathTable()
{
    DeathTable_ClearBit(deathTableId);
}
