//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" {
extern void func_ov002_020d94cc(char *c);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void *v);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void *v);
extern void _Z14ApproachLinearRiii(int *v, int a, int b);
extern u32 _ZNK6Player14GetBodyModelIDEjb(char *c, u32 a, bool b);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int f);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char *c, u32 a, int b, int d, u32 e);
extern int _ZN6Player12FinishedAnimEv(char *c);
extern void KillPlayer(void);
extern void func_ov002_020bedd4(char *c);

extern u8 data_ov002_02109db8[];
extern u32 data_ov002_0210a07c[];
extern u8 data_ov002_0211117c;
}

extern "C" int _ZN6Player15St_DeadHit_MainEv(char *c)
{
    u32 t;

    if (*(u8 *)(c + 0x6de) == 0) {
        if (*(u8 *)(c + 0x70c) == 0) {
            if (*(s32 *)(c + 0x640) < 0) {
                *(s32 *)(c + 0xa8) = -*(s32 *)(c + 0x640) / 3;
                if (*(s32 *)(c + 0xa8) <= 0x1000)
                    *(s32 *)(c + 0xa8) = 0;
            }
            t = *(u8 *)(c + 0x6e3) & 0xf0;
            if (t == 0 || t == 0x10)
                func_ov002_020d94cc(c);
            _ZN5Sound9PlayBank0EjRK7Vector3(*(u32 *)(c + 0x66c) + 0x50, c + 0x74);
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x11, c + 0x74);
            *(u8 *)(c + 0x70c) = 1;
        }
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0, 0x1800);
    }

    switch (*(u8 *)(c + 0x6e5)) {
    case 0:
        if (_ZNK9Animation12WillHitFrameEi(
                (void *)(((s32 *)(c + 0xdc))[_ZNK6Player14GetBodyModelIDEjb(c, *(s32 *)(c + 8) & 0xff, 0)] + 0x50),
                data_ov002_02109db8[*(u8 *)(c + 0x6e3) & 1]) != 0)
            *(u8 *)(c + 0x6e5) = 1;
        break;
    case 1:
        if (*(u8 *)(c + 0x6de) != 0) break;
        if (*(u8 *)(c + 0x70c) == 0) break;
        if (*(s32 *)(c + 0x98) != 0) break;
        _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_0210a07c[*(u8 *)(c + 0x6e3) & 1], 0x40000000, 0x1000, 0);
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0xb, c + 0x74);
        *(u8 *)(c + 0x6e5) = 2;
        break;
    case 2:
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            if (data_ov002_0211117c == 0) {
                KillPlayer();
                *(u8 *)(c + 0x6e5) = 3;
            }
        }
        break;
    }

    if (*(u8 *)(c + 0x6e5) != 1)
        func_ov002_020bedd4(c);
    *(s32 *)(c + 0x640) = *(s32 *)(c + 0xa8);
    return 1;
}
