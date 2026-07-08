//cpp
struct Sound { static int PlaySub(unsigned int, unsigned int, unsigned int, int, bool); };
extern signed char LEVEL_ID;
extern int MESSAGE_SOUND_VOLUME_LSL_12;

extern "C" void func_ov002_020c1234(char* self);
void func_ov002_020c1234(char* self) {
    unsigned int a1, a2;
    int fx;
    if (LEVEL_ID != 0xc && LEVEL_ID != 0x2e) return;
    switch (*(int*)(self + 0x66c)) {
    case 0xa:
        a1 = 0x2d;
        a2 = 0x28;
        fx = (MESSAGE_SOUND_VOLUME_LSL_12 >> 12 > 0x28) ? 0x8b33 : 0xd1e;
        *(unsigned char*)(self + 0x71c) = 1;
        break;
    case 0xb:
        *(unsigned char*)(self + 0x71c) = 1;
        a1 = 0;
        a2 = 0x7f;
        fx = 0x8b33;
        break;
    default:
        fx = 0x1451;
        a1 = 0x7f;
        a2 = 0;
        break;
    }
    if (*(unsigned char*)(self + 0x71c) == 0) return;
    if (Sound::PlaySub(0x35, a1, a2, fx, 0) == 0) return;
    if (*(int*)(self + 0x66c) == 0xa) return;
    if (*(int*)(self + 0x66c) != 0xb) *(unsigned char*)(self + 0x71c) = 0;
}
