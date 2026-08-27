//cpp
// @symbol _ZN10dScMgBSC_c6RenderEv
#include "decl_common.h"
#include "dScMgBSC_c.h"
/* dScMgBSC_c::Render -- vtable slot 9, ov006 0x021253bc.
 *
 * Named from the table: 0x021253bc is the word slot 9 of _ZTV10dScMgBSC_c
 * holds where its base's table holds something else, so it is this class's
 * own override of the virtual fBase_c declares.
 *
 * THE SHADOW STRUCT SURVIVES THE RENAME ON PURPOSE. Three of the four things
 * this function reads past 0x51a8 -- the two 8-byte score positions and the
 * two pairs of sprite indices at 0x51ca/0x51cc -- fall inside spans
 * include/dScMgBSC_c.h deliberately leaves as raw bytes: mArray's element
 * type is unevidenced (its ROM destructor is NullDestructor_0203d47c, i.e.
 * POD of unknown shape) and 0x51bc..0x51d0 is pad. Naming them would be a
 * layout claim this rename has no evidence for, so `Obj` stays as the
 * pre-migration file spelled it and `this` is cast to it. Only unk_51b8, the
 * one field of that tail the header does name, is spelled as itself. */

extern "C" {
void func_ov006_020c0aa8(void *camera);
void Hud_RenderSprite(void *sprite, int x, int y, int a3, int a4);
void func_ov004_020b1bc8(void *scene, int a1, int a2, int a3);
void func_ov004_020b1e34(void *scene, int a1, int a2, int a3);
void func_ov006_020c1804(void *table);
}

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

s32 dScMgBSC_c::Render()
{
    Obj *self = (Obj *)this;
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
