//cpp
extern "C" void DeathTable_ClearBit(int id);

class dActor_c {
public:
    char pad[0xce];
    short deathTableId;

    void UntrackInDeathTable();
};

void dActor_c::UntrackInDeathTable()
{
    DeathTable_ClearBit(deathTableId);
}
