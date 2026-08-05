//cpp
struct ActorBase { void MarkForDestruction(); };
struct Actor : ActorBase { static Actor *FindWithID(unsigned int); };
namespace Sound { void UnsetPlayerVoiceGroup(); }

extern "C" {
extern void func_02011c8c(void);
extern int GetSoundMode(void);
extern void SetSoundMode(int);
}

extern int data_0209fc48;
extern int data_020890a0;
extern int data_02088fb8;
extern unsigned int data_0209b284[4];
extern unsigned char data_0209f21c;
extern unsigned char data_0209b270;
extern unsigned int data_0209b2a4[0x10];
extern int data_0209b454;
extern unsigned char data_0209f2d8;
extern int data_0209fc4c;

extern "C" void EndKuppaScript(void)
{
    unsigned int *p;
    int i;
    int v = data_0209fc48;
    if (v == 0)
        return;
    if (v == (int)&data_020890a0) {
        Sound::UnsetPlayerVoiceGroup();
    } else if (v == (int)&data_02088fb8) {
        func_02011c8c();
        SetSoundMode(GetSoundMode());
    }

    p = data_0209b284;
    for (i = 0; i < 4; i++) {
        Actor *a = Actor::FindWithID(*p);
        if (a != 0) {
            a->MarkForDestruction();
            *p = 0;
        }
        p++;
    }

    data_0209b270 = data_0209f21c;

    p = data_0209b2a4;
    for (i = 0; i < 0x10; i++) {
        Actor *a = Actor::FindWithID(*p);
        if (a != 0) {
            a->MarkForDestruction();
            *p = 0;
        }
        p++;
    }

    {
        int r3 = data_0209b454;
        unsigned char m = data_0209f2d8;
        int flag;
        r3 = r3 & ~0x20000000;
        data_0209fc48 = 0;
        flag = (m == 2);
        data_0209b454 = r3;
        if (!flag)
            return;
    }

    if (data_0209fc4c == 0)
        data_0209f2d8 = 0;
}
