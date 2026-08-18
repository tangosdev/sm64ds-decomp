//cpp
extern "C" int DeathTable_GetBit(int id);

class Actor {
public:
    char pad[0xce];
    short deathTableId;

    int GetBitInDeathTable();
};

int Actor::GetBitInDeathTable()
{
    return DeathTable_GetBit(deathTableId);
}
