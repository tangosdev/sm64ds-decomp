//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" {
extern int func_ov002_020e2be4(char *c);
extern int func_ov002_020e2b6c(char *c);
extern int Player_ScaleByCharFactor(char *c, int a);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *c, void *s);
extern void func_ov002_020bf2d8(char *c, int a);
extern void func_ov002_020e2ad0(char *c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char *c, u32 a, int b, int fx, u32 d);
extern int AngleDiff(int a, int b);
extern int func_ov002_020c19d0(char *c, int a, int b);
extern void func_ov002_020e25f0(char *c, int a);
extern int data_ov002_0210a59c[];
extern int data_ov002_0210a5a8[];
extern char data_ov002_02110214;
}

extern "C" int _ZN6Player12St_Jump_InitEv(char *c)
{
    int t;

    *(u8 *)(c + 0x71b) = 0;
    if (func_ov002_020e2be4(c) != 0)
        return 1;
    if (func_ov002_020e2b6c(c) != 0)
        return 1;

    *(u8 *)(c + 0x712) = 1;
    *(u8 *)(c + 0x70d) = 0;
    if (*(u16 *)(c + 0x6a8) == 0) {
        *(u8 *)(c + 0x6e1) = 0;
    } else {
        *(u8 *)(c + 0x6e1) = (*(u8 *)(c + 0x6e1) + 1) % 3;
    }

    if (*(int *)(c + 0x68c) > 0x1000)
        goto clear;
    t = (*(int *)(c + 0x358) != 0);
    if (t == false)
        goto skipclear;
clear:
    *(u8 *)(c + 0x6e1) = 0;
skipclear:

    if (*(u8 *)(c + 0x703) != 0)
        *(u8 *)(c + 0x6e1) = 0;

    if (*(u8 *)(c + 0x6e1) == 2 && *(u8 *)(c + 0x6ff) == 0) {
        if (*(int *)(c + 0x98) <= Player_ScaleByCharFactor(c, 0x14000))
            *(u8 *)(c + 0x6e1) = 0;
    }

    *(u8 *)(c + 0x6de) = 1;
    *(u8 *)(c + 0x6df) = 0;
    if (*(u8 *)(c + 0x6ff) == 1 && *(u8 *)(c + 0x6e1) == 2) {
        *(u8 *)(c + 0x6e3) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110214);
    } else {
        int v = data_ov002_0210a59c[*(u8 *)(c + 0x6e1)] + (*(int *)(c + 0x98) >> 2);
        if (*(u8 *)(c + 0x703) != 0) {
            func_ov002_020bf2d8(c, (int)(((long long)v * 0x1400 + 0x800) >> 12));
        } else {
            func_ov002_020bf2d8(c, v);
        }
        func_ov002_020e2ad0(c);
        if (*(u8 *)(c + 0x703) == 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_0210a5a8[*(u8 *)(c + 0x6e1)], 0x40000000, 0x1000, 0);
        } else {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x9c, 0x40000000, 0x1000, 0);
        }
        if (*(int *)(c + 0x98) > Player_ScaleByCharFactor(c, 0x64000)) {
            *(int *)(c + 0x98) = Player_ScaleByCharFactor(c, 0x64000);
        }
    }

    *(u8 *)(c + 0x6e5) = 0;
    if (*(int *)(c + 8) == 3) {
        if (*(u8 *)(c + 0x6e1) != 2)
            *(u8 *)(c + 0x6e5) = 1;
    }

    *(u8 *)(c + 0x6e3) = 0;
    *(u8 *)(c + 0x70c) = 0;
    *(u16 *)(c + 0x6cc) = 0;
    func_ov002_020e25f0(c, *(u8 *)(c + 0x6e1));

    *(int *)(c + 0x98) = (int)(((long long)*(int *)(c + 0x98) * 0xccc + 0x800) >> 12);

    if (AngleDiff(*(s16 *)(c + 0x94), *(s16 *)(c + 0x8e)) >= 0x4000) {
        *(int *)(((int)c + 0x98)) >>= 2;
    }

    if (func_ov002_020c19d0(c, 0x64, 0x32) != 0) {
        *(u16 *)(((int)c + 0x6ce)) |= 0x200;
        *(int *)(c + 0x98) = 0;
    }
    return 1;
}
