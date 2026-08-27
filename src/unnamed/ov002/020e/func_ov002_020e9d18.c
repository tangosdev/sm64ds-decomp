#include "types.h"
enum { false, true };

typedef struct Vec3 { int x, y, z; } Vec3;

extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 a, int vol);
extern int _ZN9Animation8FinishedEv(char *anim);
extern void _ZN9Animation7AdvanceEv(char *anim);
extern void func_ov002_020e7eb4(char *c);
extern void GiveVsStars(int idx, int n);
extern void func_ov002_020e8244(Vec3 *t, char *c);
extern void _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(char *self, Vec3 *vec, int n, u32 b, int t, int actor);
extern void func_ov002_020e8618(char *c);
extern void func_02012790(int n);
extern int _ZN6Player12Unk_020c9e5cEh(int self, u32 st);
extern void EndKuppaScript(void);
extern void func_ov002_020e8098(char *c);
extern int func_ov002_020e9630(char *c);

extern u8 data_0209f2d8;
extern s8 data_0209f310[];
extern u8 data_0209d684;

void func_ov002_020e9d18(char *c)
{
    u32 r2v;
    u32 b2;
    u32 st;
    Vec3 t1;
    Vec3 t2;
    int *src;

    src = (int *)(((int)*(int *)(c + 0x438) + 0x5c) & 0xFFFFFFFFFFFFFFFF);
    *(int *)(c + 0x5c) = src[0];
    *(int *)(c + 0x60) = src[1];
    *(int *)(c + 0x64) = src[2];
    *(s16 *)(c + 0x94) = *(s16 *)(*(int *)(c + 0x438) + 0x8e);
    if (*(int *)(c + 0x43c) == 9) {
        if (*(u8 *)(c + 0x4a1) < 0x78) {
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0, 0x7f000);
            (*(u8 *)(((int)c + 0x4a1) & 0xFFFFFFFFFFFFFFFF))++;
        } else {
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x40, 0xcb33);
        }
    }
    if (_ZN9Animation8FinishedEv(c + 0x35c) != 0) {
        (*(u16 *)(((int)c + 0x490) & 0xFFFFFFFFFFFFFFFF))++;
    } else {
        func_ov002_020e7eb4(c);
    }
    r2v = data_0209f2d8 == 1;
    if (r2v != false)
        goto modes;
    b2 = *(u16 *)(c + 0xc);
    b2 = b2 == 0xb3;
    if (b2 == false)
        goto big_else;
modes:
    {
        u32 tmp;
        u16 mode;
        tmp = *(u16 *)(c + 0xc);
        tmp = tmp == 0xb3;
        if (tmp != false)
            st = 1;
        else
            st = 0;
        mode = *(u16 *)(c + 0x490);
        if (mode == 1 && r2v == 0) {
            int idx = *(u8 *)(*(int *)(c + 0x438) + 0x6d8);
            if (data_0209f310[idx] == 4) {
                GiveVsStars(idx, 1);
                func_ov002_020e8244(&t1, c);
                _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(c, &t1, 5, st, 0, *(int *)(c + 0x438));
                func_ov002_020e8618(c);
            }
        } else if (mode == 5) {
            int idx2 = *(u8 *)(*(int *)(c + 0x438) + 0x6d8);
            if (data_0209f310[idx2] == 5 && r2v == false)
                goto end;
            if (r2v == false)
                GiveVsStars(idx2, 1);
            func_ov002_020e8244(&t2, c);
            _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(c, &t2,
                data_0209f310[*(u8 *)(*(int *)(c + 0x438) + 0x6d8)], st, 0,
                *(int *)(c + 0x438));
            func_ov002_020e8618(c);
        }
        goto end;
    }
big_else:
    {
        u32 st2;
        if (*(int *)(c + 0x43c) == 9) {
            st2 = 0x11;
LA:
        if (((u32)(*(u16 *)(c + 0x4a2) << 20) >> 30) == 0) {
            *(u16 *)(((int)c + 0x4a2) & 0xFFFFFFFFFFFFFFFF) =
                (*(u16 *)(((int)c + 0x4a2) & 0xFFFFFFFFFFFFFFFF) & ~0xc00) |
                ((data_0209d684 & 3) << 10);
            {
                u32 b3 = (u32)(*(u16 *)(c + 0x4a2) << 20) >> 30;
                if (b3 == 1) {
                    func_02012790(0x57);
                } else if (b3 != 2) {
                    *(u16 *)(((int)c + 0x4a2) & 0xFFFFFFFFFFFFFFFF) &= ~0xc00;
                } else {
                    func_02012790(0x5c);
                }
            }
        }
        *(u16 *)(c + 0x490) = 0xa;
        if (_ZN6Player12Unk_020c9e5cEh(*(int *)(c + 0x438), st2) == 0) {
            *(u8 *)(c + 0x49b) = 2;
            *(int *)(c + 0x440) = 0xb;
            *(u16 *)(c + 0x490) = 0;
            *(int *)(((int)*(int *)(c + 0x438) + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~0x4000000;
            EndKuppaScript();
        }
        _ZN9Animation7AdvanceEv(c + 0x35c);
        func_ov002_020e8098(c);
        return;
        }
        if (*(u8 *)(c + 0x49d) == 0 || func_ov002_020e9630(c) != 0) {
            st2 = 1;
            goto LA;
        }
        if (*(int *)(c + 0x444) == 9 || *(u8 *)(c + 0x49d) == 8)
            func_ov002_020e8618(c);
    }
end:
    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov002_020e8098(c);
}
