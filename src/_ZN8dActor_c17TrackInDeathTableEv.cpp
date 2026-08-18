//cpp
extern "C" void DeathTable_SetBit(int id);

class dActor_c {
public:
    char pad[0xce];
    short deathTableId;

    void TrackInDeathTable();
};

void dActor_c::TrackInDeathTable()
{
    DeathTable_SetBit(deathTableId);
}
