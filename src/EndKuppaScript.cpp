//cpp
struct ActorBase { void MarkForDestruction(); };
struct Actor : ActorBase { static Actor *FindWithID(unsigned int); };
namespace Sound { void UnsetPlayerVoiceGroup(); }

extern void func_02011c8c(void);
extern int GetSoundMode(void);
extern void SetSoundMode(int);

extern int RUNNING_KUPPA_SCRIPT;
extern int INTRO_CUTSCENE;
extern int data_02088fb8;
extern unsigned int KS_PLAYER_IDS[4];
extern unsigned char NUM_PLAYERS;
extern unsigned char KS_NUM_PLAYERS;
extern unsigned int CUTSCENE_OBJECT_IDS[0x10];
extern int NEXT_ACTOR_UPDATE_FLAGS;
extern unsigned char CURRENT_GAMEMODE;
extern int SAVED_KUPPA_SCRIPT;

extern "C" void EndKuppaScript(void)
{
    unsigned int *p;
    int i;
    int v = RUNNING_KUPPA_SCRIPT;
    if (v == 0)
        return;
    if (v == (int)&INTRO_CUTSCENE) {
        Sound::UnsetPlayerVoiceGroup();
    } else if (v == (int)&data_02088fb8) {
        func_02011c8c();
        SetSoundMode(GetSoundMode());
    }

    p = KS_PLAYER_IDS;
    for (i = 0; i < 4; i++) {
        Actor *a = Actor::FindWithID(*p);
        if (a != 0) {
            a->MarkForDestruction();
            *p = 0;
        }
        p++;
    }

    KS_NUM_PLAYERS = NUM_PLAYERS;

    p = CUTSCENE_OBJECT_IDS;
    for (i = 0; i < 0x10; i++) {
        Actor *a = Actor::FindWithID(*p);
        if (a != 0) {
            a->MarkForDestruction();
            *p = 0;
        }
        p++;
    }

    {
        int r3 = NEXT_ACTOR_UPDATE_FLAGS;
        unsigned char m = CURRENT_GAMEMODE;
        int flag;
        r3 = r3 & ~0x20000000;
        RUNNING_KUPPA_SCRIPT = 0;
        flag = (m == 2);
        NEXT_ACTOR_UPDATE_FLAGS = r3;
        if (!flag)
            return;
    }

    if (SAVED_KUPPA_SCRIPT == 0)
        CURRENT_GAMEMODE = 0;
}
