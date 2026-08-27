//cpp
extern "C" int DeathTable_GetBit(int id);

class dActor_c {
public:
    char pad[0xce];
    short deathTableId;

    int GetBitInDeathTable();
};

int dActor_c::GetBitInDeathTable()
{
    return DeathTable_GetBit(deathTableId);
}
