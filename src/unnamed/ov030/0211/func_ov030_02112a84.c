#include "types.h"
typedef struct P2 { int a, b; } P2;

typedef struct dBgPi {
    void *vtb;
    int s0, s1, s2, s3, s4;
    u16 f, g;
    int h, i, j;
} dBgPi;

extern void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *p);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *p);
extern void *func_0203567c(void *p);
extern u32 _ZNK5dBgPi9GetClsnIDEv(void *r);
extern void func_ov030_021141a8(void *a, int m);
extern void _ZN5dBgPiD1Ev(void *r);
extern int Vec3_Dist(void *a, void *b);
extern void _ZN9Animation7AdvanceEv(void *p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *a, void *c);
extern void func_ov030_02111bc4(void *a);
extern void _ZN5dCc_c5ClearEv(void *p);
extern void *data_02099368[];

int func_ov030_02112a84(char *a)
{
    dBgPi res;

    dBgCh_Actr_UpdateContinuous_Veneer(a + 0x194);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(a + 0x194) || _ZNK10dBgCh_Actr10IsOnGroundEv(a + 0x194)) {
        int b;
        u16 id;

        b = 0;
        *(int *)(a + 0xa8) = 0;
        id = *(u16 *)(a + 0xc);
        if (id == 0x10b)
            b = 1;
        if (b) {
            char *r = (char *)func_0203567c(a + 0x194);
            int *d = &res.s0;
            *(P2 *)d = *(P2 *)(r + 4);
            d[2] = *(int *)(r + 0xc);
            d[3] = *(int *)(r + 0x10);
            d[4] = *(int *)(r + 0x14);
            res.vtb = data_02099368;
            res.f = *(u16 *)(r + 0x18);
            res.g = *(u16 *)(r + 0x1a);
            res.h = *(int *)(r + 0x1c);
            res.i = *(int *)(r + 0x20);
            res.j = *(int *)(r + 0x24);
            if (_ZNK5dBgPi9GetClsnIDEv(&res) != 0xffffffff)
                func_ov030_021141a8(a, 9);
            else
                func_ov030_021141a8(a, *(int *)(a + 0x3b8));
            _ZN5dBgPiD1Ev(&res);
        } else {
            int t = (int)(id == 0x10c);
            if (t != 0) {
                if (Vec3_Dist(a + 0x380, a + 0x5c) < 0x514000
                    && *(int *)(a + 0x60) > *(int *)(a + 0x384) - 0x12c000) {
                    func_ov030_021141a8(a, *(int *)(a + 0x3b8));
                } else {
                    func_ov030_021141a8(a, 9);
                }
            }
        }
    }

    _ZN9Animation7AdvanceEv(a + 0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(a, a + 0x160);
    func_ov030_02111bc4(a);
    _ZN5dCc_c5ClearEv(a + 0x160);
    return 1;
}
