//cpp
struct Sound { static void PlayBank2_2D(unsigned int); };
extern "C" void func_02012790(int x);
extern "C" void func_ov004_020b5dd4(void);

extern "C" void func_ov006_020f5a64(char *c)
{
    int a, b;
    unsigned char *sa, *sb;
    if (*(unsigned char *)(c + 0x5406) < 2) return;
    a = *(unsigned char *)(c + 0x53f0);
    sa = (unsigned char *)(c + 0x51bc + a * 0x18);
    b = *(unsigned char *)(c + 0x53f1);
    if (*sa != 4) return;
    sb = (unsigned char *)(c + 0x51bc + b * 0x18);
    if (*sb != 4) return;
    if (*(unsigned char *)(c + 0x5000 + a * 0x18 + 0x1b8) ==
        *(unsigned char *)(c + 0x5000 + b * 0x18 + 0x1b8)) {
        *(unsigned char *)(c + 0x5000 + a * 0x18 + 0x1bb) = 0;
        *(unsigned char *)(c + 0x5000 + a * 0x18 + 0x1ba) = 0;
        *(unsigned char *)(c + 0x5000 + b * 0x18 + 0x1bb) = 0;
        *(unsigned char *)(c + 0x5000 + b * 0x18 + 0x1ba) = 0;
        func_02012790(0x26);
        Sound::PlayBank2_2D(0x13d);
        *(unsigned char *)(((int)c + 0x5405) & 0xFFFFFFFFFFFFFFFF) += 1;
        *(unsigned char *)(c + 0x5406) = 0;
    } else {
        func_02012790(0xe);
        Sound::PlayBank2_2D(0x13e);
        *(unsigned char *)(((int)c + 0x5408) & 0xFFFFFFFFFFFFFFFF) += 1;
        func_ov004_020b5dd4();
        if (*(unsigned char *)(c + 0x5408) < *(unsigned char *)(c + 0x5409)) {
            *sa = 5;
            *sb = 5;
            *(unsigned char *)(c + 0x5406) = 0;
        }
    }
}
