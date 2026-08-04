// @symbol func_ov006_021253bc
// recovered name: dScMgBSC_c_Render
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgBSC_c::Render - recovered from vtable slot identity */
extern void func_ov006_020c0aa8(void *a0);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b1bc8(void *a0, int a1, int a2, int a3);
extern void func_ov004_020b1e34(void *a0, int a1, int a2, int a3);
extern void func_ov006_020c1804(void *a0);


typedef struct { int x, y; } Pair;
typedef struct Obj {
    char _a[0x4660];
    char f4660;
    char _b[0x4f38 - 0x4661];
    char f4f38;
    char _c[0x51a8 - 0x4f39];
    Pair pos[2];
    int count;
    char _d[0x51ca - 0x51bc];
    unsigned char b0[2];
    unsigned char b1[2];
} Obj;

int func_ov006_021253bc(Obj *self) {
    int i;

    func_ov006_020c0aa8(&self->f4660);
    if (self->count >= 1) {
        for (i = 0; i < 2; i++) {
            if (self->b1[i] < 3) {
                Hud_RenderSprite(data_ov006_0213fe8c[self->b1[i]],
                                    self->pos[i].x >> 12,
                                    self->pos[i].y >> 12, -1, -1);
            } else {
                Hud_RenderSprite(
                    data_ov006_0213fe8c[self->b1[i] + self->b0[i] * 2],
                    self->pos[i].x >> 12,
                    self->pos[i].y >> 12, -1, -1);
            }
        }
    }
    func_ov004_020b6430();
    func_ov004_020b1bc8(self, 0xc, 0xc, 0);
    func_ov004_020b1e34(self, 0xe0, 0x14, 1);
    func_ov006_020c1804(&self->f4f38);
    return 1;
}
