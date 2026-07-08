//cpp
struct C;
typedef int (C::*PMF)(void*);
extern PMF data_ov002_0211110c[];
extern int data_0208e430;
extern int MESSAGE_SOUND_VOLUME_LSL_12;
extern int MUSIC_VOLUME_LSL_12;

extern "C" {
void _ZN9ActorBase18MarkForDestructionEv(C* c);
void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int d, int f, bool g);

int func_ov002_020f94fc(char* cc)
{
    C* c = (C*)cc;
    int sel = *(int*)(cc + 8);
    int r = (c->*data_ov002_0211110c[sel])(cc + 0xdc);
    if (r == 0 && *(int*)(cc + 0xd4) == data_0208e430
        && (*(unsigned short*)(cc + 0xdc) <= 0xa || MESSAGE_SOUND_VOLUME_LSL_12 > 0x7f)) {
        goto skip;
    }
    _ZN9ActorBase18MarkForDestructionEv(c);
    if (data_0208e430 != 0x22) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(data_0208e430, 0x7f, 0, 0x7f000, 0);
    }
skip:
    if (*(int*)(cc + 8) != 6) {
        if (MUSIC_VOLUME_LSL_12 < *(int*)(cc + 0xd8)) *(unsigned short*)(cc + 0xdc) = *(unsigned short*)(cc + 0xde);
    }
    return 1;
}
}
