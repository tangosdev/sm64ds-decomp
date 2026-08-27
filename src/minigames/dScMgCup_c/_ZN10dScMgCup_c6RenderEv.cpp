//cpp
// @symbol _ZN10dScMgCup_c6RenderEv
#include "dScMgCup_c.h"
/* dScMgCup_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable at ov006 0x0213c154, the second of the two slots
 * where the table differs from dScMgSingle3DBase_c's. The old file carried no
 * `recovered name:` comment at all, only the func_ov006_ address.
 *
 * Draws the three cups back to front: the bubble sort orders the indices by the
 * per-cup depth at 0x53ec, and the render loop then walks them in that order.
 *
 * WAS A C99 FILE, and every declaration below therefore has to move inside
 * `extern "C"` -- in C++ these names would otherwise mangle and the link would
 * come up short. The `extern` on the two arrays is load-bearing for the same
 * reason the tree's notes give: without it, a variable declaration inside
 * `extern "C" {}` is a DEFINITION and collides with the delinked gap object. */

extern "C" {
struct P8 { int a; int b; };
struct S8 { void *a; int b; };

void func_ov006_020debb4(char *a, int b);
void func_ov006_020deed8(int a0, void *a1, int a2, int a3, int a4, int a5);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020b2574(int arg0, int arg1);
void func_ov004_020b1e34(void *a, int b, int c, int d);
void func_ov006_020c29dc(char *c);

extern struct S8 *data_ov006_0213c0d8[];
extern char data_ov006_02139df4[];
}

s32 dScMgCup_c::Render()
{
    char *c = (char *)this;
    int list[3];
    int i;
    int n;
    int j;
    int x;
    int y;
    int k;
    int v;

    for (i = 0; i < 3; i++) {
        list[i] = i;
    }

    for (n = 3; n > 1; n--) {
        for (j = 0; j < n - 1; j++) {
            y = list[j + 1];
            x = list[j];
            if (*(int*)(c + x * 8 + 0x53ec) < *(int*)(c + y * 8 + 0x53ec)) {
                list[j] = y;
                list[j + 1] = x;
            }
        }
    }

    for (k = 0; k < 3; k++) {
        v = list[k];
        func_ov006_020debb4(c + 0x50e8, (char)v);
        func_ov006_020deed8((int)c,
            data_ov006_0213c0d8[*(int*)(c + v * 4 + 0x5434)][((int*)c + k)[0x1510]].a,
            *(int*)(c + v * 8 + 0x53e8),
            ((struct P8*)c + v)[0xa7d].b,
            *(int*)(c + v * 4 + 0x540c),
            *(unsigned char*)(c + v + 0x5462));
    }

    if (*(int*)(c + 0x5418) == 4 || *(int*)(c + 0x5418) == 5) {
        Hud_RenderSprite(data_ov006_02139df4, 0x92, 0x20,
            (*(unsigned char*)(c + 0x5468) == 2) ? 4 : -1, -1);
    }

    func_ov004_020b2574(*(int*)(c + 0xa8), 1);
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);
    func_ov006_020c29dc(c + 0x4f38);
    return 1;
}
