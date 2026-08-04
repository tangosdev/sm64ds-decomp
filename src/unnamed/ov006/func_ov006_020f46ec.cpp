//cpp
namespace Sound { void PlayBank2_2D(unsigned int); }
extern "C" void func_ov006_020f39c8(char *p);

extern "C" void func_ov006_020f46ec(char *c)
{
    int cnt;
    int i;
    char *p;
    if (*(unsigned short *)(c + 0x5300 + 0x22) != 0) {
        unsigned short *q = (unsigned short *)(c + 0x5322);
        *q = *q - 1;
        return;
    }
    if (*(unsigned char *)(c + 0x5337) >= *(unsigned short *)(c + 0x532a)) {
        *(int *)(c + 0x5314) = 4;
        *(unsigned short *)(c + 0x5322) = 0;
        return;
    }
    cnt = 0;
    i = 0;
    p = c;
    for (; i < 0xc; i++) {
        if (*(unsigned char *)(p + 0x51bb) != 0) {
            if (*(unsigned char *)(p + 0x51bc) == 2) {
                *(unsigned char *)(p + 0x51bc) = 3;
                cnt++;
            }
        }
        p += 0x18;
    }
    if (cnt <= 2)
        Sound::PlayBank2_2D(0x145);
    else
        Sound::PlayBank2_2D(0x146);
    *(int *)(c + 0x5318) = 3;
    func_ov006_020f39c8(c);
    *(unsigned short *)(c + 0x5322) = 0x20;
}
