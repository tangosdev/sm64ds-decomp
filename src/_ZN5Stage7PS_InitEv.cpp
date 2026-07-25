//cpp
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;

extern "C" int SublevelToLevel(int i);

struct Sound {
    static void PauseMusic();
};

struct G2x {
    static void SetBlendBrightness(volatile u16 *reg, int a, short b);
};

extern u8 data_0209d454;
extern u8 data_0209d45c;
extern u8 data_0209f1ec;
extern u8 data_0209f210;
extern u8 data_0209f218;
extern u8 data_0209f22c;
extern u8 data_0209f230;
extern u8 data_0209f238;
extern u8 data_0209f240;
extern u8 data_0209f244;
extern u8 data_0209f248;
extern u8 data_0209f280;
extern u8 data_0209f29c;
extern u8 data_0209f2c8;
extern u8 data_0209f2cc;
extern u8 data_0209f2d8;
extern u8 data_0209f2e0;
extern u8 data_0209f2f0;
extern s8 data_0209f2f8;
extern u16 data_0209f300;
extern u8 data_0209f23c;
extern int data_0209fc68;

struct Stage {
    static void PS_Init();
};

void Stage::PS_Init()
{
    u8 sl;
    u8 sb;
    u8 r7;
    u8 r8;
    int r5;
    int r4;

    sb = data_0209d454;
    if (sb & 4) {
        data_0209d454 = sb & ~4;
        sb = data_0209d454;
        *(volatile unsigned int *)0x04001000 =
            (*(volatile unsigned int *)0x04001000 & ~0x1f00) | (sb << 8);
    }

    r7 = data_0209f218;
    r8 = data_0209d45c;
    sl = 0;

    data_0209f248 = 0;
    data_0209f2e0 = 0;
    data_0209f2c8 = 0;
    data_0209f230 = 0;
    data_0209f2f0 = 0;
    data_0209f210 = 0;
    data_0209f22c = 0;
    data_0209f240 = 0;
    data_0209f238 = 0;
    data_0209f2cc = 0;
    data_0209f300 = 0xf;
    data_0209f23c = 0;
    data_0209f244 = 0;
    data_0209f29c = 0;
    data_0209f280 = 0;

    r5 = r8 ^ 0x10;
    r4 = sb ^ 0x10;

    if (r7 != 0) {
        data_0209f1ec = 7;
    } else {
        if (data_0209f2d8 == 1)
            sl = 1;
        if (sl != 0 && data_0209fc68 == 0) {
            data_0209f1ec = 4;
        } else {
            if (SublevelToLevel(data_0209f2f8) == 0x1d)
                data_0209f1ec = 0;
            else
                data_0209f1ec = 2;
        }
    }

    Sound::PauseMusic();
    G2x::SetBlendBrightness((volatile u16 *)0x04000050, r5 | 0x20, ~6);
    G2x::SetBlendBrightness((volatile u16 *)0x04001050, r4 | 0x20, ~6);
}
