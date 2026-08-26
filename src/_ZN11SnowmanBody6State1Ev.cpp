//cpp
#include "SnowmanBody.h"

extern "C" {
int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
    int player, void *actor, int msg, void *pos, int a, int b);
int _ZN6Player12GetTalkStateEv(void *player);
unsigned short DecIfAbove0_Short(unsigned short *timer);
}

int SnowmanBody::State1()
{
    char *c = (char *)this;
    int v[3];
    unsigned char *state;
    v[0] = *(int *)(c + 0x5c);
    int y = *(int *)(c + 0x60);
    v[1] = y;
    v[2] = *(int *)(c + 0x64);
    v[1] = y + 0x96000;
    switch (*(unsigned char *)(c + 0x3a2)) {
    case 0:
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                *(int *)(c + 0x390), c, 0xb0, v, 0, 0) == 0)
            break;
        state = (unsigned char *)(((int)c + 0x3a2));
        *state = *state + 1;
        break;
    case 1:
        if (_ZN6Player12GetTalkStateEv((void *)*(int *)(c + 0x390)) != -1)
            break;
        state = (unsigned char *)(((int)c + 0x3a2));
        *state = *state + 1;
        break;
    case 2:
        if (DecIfAbove0_Short((unsigned short *)(c + 0x3a0)) == 0)
            SetState(2);
        break;
    }
    return 1;
}
