//cpp
extern "C" void DeathTable_SetBit(int id);

class Actor {
public:
    char pad[0xce];
    short deathTableId;

    void TrackInDeathTable();
};

void Actor::TrackInDeathTable()
{
    DeathTable_SetBit(deathTableId);
}
