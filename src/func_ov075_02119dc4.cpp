//cpp
// @symbol func_ov075_02119dc4
/* recovered: dScEntry_c wireless/VS teardown.
 *
 * Clears the two entry flags, folds the three per-slot ready bytes at
 * data_0209fc5d into a bitmask, then splits on whether the pending scene is
 * the VS one (6) or not. Both arms fade the top screen to black
 * (SetBlendBrightness with a negative amount) and hand off through
 * func_ov075_0211a148 or the sound/scene helpers.
 *
 * The materialized bool at the split (cmp/moveq/movne/cmp/beq) is the tell
 * that this TU is C++: in C mode mwccarm folds that away under every spelling.
 * See notes/mwccarm-codegen.md 6c and 6bg.
 */
#include "types.h"

extern "C" {

extern void func_ov075_02116028(int *p, int v);
extern void func_02030a78(void);
extern int GetOwnerLanguage(void);
extern void func_020338b0(int a, int b, int c, int d);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16 *p, u16 val, short amt);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int n);
extern void func_02012790(int x);
extern int func_0203d974(void);
extern int func_0203d9b4(void);
extern void func_ov075_0211a148(char *c, int a, int b);

}

extern u8 data_0209d454;
extern u8 data_0209fc58;
extern u8 data_0209fc5c;
extern u8 data_0209fc5d[];
extern int data_0209fc68;
extern u8 data_0209d45c;
extern int data_02086b58[];
extern int data_ov075_0211d71c[];
extern int data_ov075_0211d920[];
extern int data_ov075_0211d930[];

struct P2 { int a, b; };
struct P2Copy { int words[2]; };

static inline int IsVsMode() { return data_0209fc68 == 6; }

extern "C" void func_ov075_02119dc4(char *self, int arg)
{
    volatile struct P2 tmp;
    u8 mask;
    u8 *q;
    int i;
    u8 st;

    data_0209d454 = data_0209d454 & ~0x11;
    mask = 0;
    q = data_0209fc5d;
    for (i = 1; i < 4; i++) {
        if (q[0])
            mask = mask | (1 << i);
        q++;
    }
    st = data_0209fc58;
    if (IsVsMode()) {
        func_ov075_02116028(data_ov075_0211d71c, 5);
        func_02030a78();
        {
        struct P2 *p;
        int *ref;
        if ((st & 0x30) != 0 ||
            ((p = (struct P2 *)(self + 0x5c)), (ref = data_ov075_0211d920),
             p->a == ref[0] &&
             (p->b == ref[1] ||
              *(int *)(self + 0x5c) == 0))) {
            if (GetOwnerLanguage() == 3)
                func_020338b0(0xa5, 0xb0, 0x15, -1);
            else
                func_020338b0(0xc5, 0xb0, 0x15, -1);
        } else {
            func_020338b0(0xc5, 0xb0, 0x13, -1);
        }
        }
        tmp.a = data_02086b58[0];
        tmp.b = data_02086b58[1];
        *(int *)(self + 0x5c) = data_02086b58[0];
        *(int *)(self + 0x60) = data_02086b58[1];
        data_0209d45c = data_0209d45c & ~*(u8 *)(self + 0x287);
        if (*(u8 *)(self + 0x287)) {
            int t0 = tmp.a;
            int t1 = tmp.b;
            *(int *)(self + 0x64) = t0;
            *(int *)(self + 0x68) = t1;
        }
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x4000050, 0x1f, -6);
        *(volatile u16 *)0x4001050 = 0;
        _ZN5Sound22StopLoadedMusic_Layer1Ej(4);
        func_02012790(0x123);
        *(u8 *)(*(char **)(self + 0x50) + 0xf44) = 1;
    } else {
        func_ov075_02116028(data_ov075_0211d71c, 6);
        if ((func_0203d974() && data_0209fc5c == 0) ||
            (func_0203d9b4() && mask == 0)) {
            if (func_0203d974())
                func_020338b0(0x125, 0xb0, 0x1b, 0);
            else
                func_020338b0(0x125, 0xb0, 0x1a, -1);
            func_02030a78();
            func_ov075_0211a148(self, (int)data_ov075_0211d930, 0xb4);
            data_0209d45c = data_0209d45c & ~*(u8 *)(self + 0x287);
            if (*(u8 *)(self + 0x287))
                *(struct P2Copy *)(self + 0x64) = *(struct P2Copy *)data_02086b58;
            _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x4000050, 0x1f, -6);
            func_02012790(0x12a);
        } else {
            for (i = 0; i < 4; i++) {
                int bit = 1 << i;
                if (st & bit) {
                    func_020338b0(0x125, 0xb0, 0x1c, (s8)(u8)i);
                    data_0209fc58 = data_0209fc58 & ~bit;
                    func_ov075_0211a148(self, arg, 0x78);
                    func_02012790(0x12a);
                    break;
                }
            }
        }
    }
    data_0209d454 = data_0209d454 | 5;
    *(u8 *)(self + 0x280) = 0;
}
