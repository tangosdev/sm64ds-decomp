// @symbol func_ov006_02110244
/* NOT a Player method, despite the name. ov006, 0x02110244.
 *
 * This is the file that contaminated include/Player.h. It reads 0x08 and 0x0c
 * as a fix12 2D coordinate pair -- differencing them against another point and
 * feeding the result to a distance helper. On a real Player those offsets are
 * ActorBase's param1 and actorID/aliveState/shouldBeKilled, which cannot be a
 * position. It is some other ov006 class at a shared RAM address.
 *
 * Because it was the only evidence for Player.h's `unk_00c` being an s32, and
 * for `unk_030` and `mModelAnim1` existing at all, those fields can now be
 * dropped from that header.
 *
 * It was also the ONLY C translation unit including Player.h, so with this
 * detached the header can become C++-only and shed its #else branch -- the
 * dual-branch shape that silently split in #980 when a rename landed on one
 * side only.
 *
 * Kept under the mangled name because renaming the symbol is a config change;
 * this commit is src-only.
 */
#include "decl_common.h"

/* Local layout: only what this function actually touches. */
struct Owl {
    char pad_000[0x8];
    int  posX;              /* 0x08 -- fix12 */
    int  posY;              /* 0x0c -- fix12 */
    char pad_010[0x20];
    unsigned char active;   /* 0x30 */
    char pad_031[0x7];
    int  state;             /* 0x38 */
};

struct S { int a, b; };

extern int func_ov006_02114590(void *a0, struct S *p, struct S *q0, struct S *q1, struct S *q2);

int func_ov006_02110244(struct Owl *self, struct S *v) {
    struct S d;
    struct S p, t1, t2, t3;
    struct S p2, t4, t5, t6;
    int t;
    int r;
    int yv, xv;
    int yv2, xv2;

    if (self->active == 0)
        return 0;
    t = v->a;
    d.a = t;
    d.b = v->b;
    d.a -= self->posX;
    d.b -= self->posY;
    if (self->state <= 0) {
        t = self->posY;
        if (v->b >= t) {
            if (func_0203d5bc(&d.a) >= 0x190000)
                return 0;
            return func_0203d5bc(&d.a) >= 0xc4000;
        }
        if (v->a >= self->posX - 0x14000 && v->a < self->posX + 0x14000 &&
            v->b >= t - 0xc000) {
            if (v->a >= self->posX - 0x14000 && v->a < self->posX - 0xa000) {
                p.a = v->a;
                p.b = v->b;
                yv = self->posY - 0xc000;
                xv = self->posX - 0x14000;
                t1.a = xv;
                t1.b = yv;
                yv = self->posY - 0xc000;
                xv = self->posX - 0xa000;
                t2.a = xv;
                t2.b = yv;
                yv = self->posY - 0x1000;
                xv = self->posX - 0xa000;
                t3.a = xv;
                t3.b = yv;
                r = func_ov006_02114590(((char *)self), &p, &t1, &t2, &t3);
                if (r == 0) goto in1;
                return 0;
            in1:
                return 1;
            }
            if (v->a >= self->posX + 0xa000 && v->a < self->posX + 0x16000) {
                p2.a = v->a;
                p2.b = v->b;
                yv2 = self->posY - 0xc000;
                xv2 = self->posX + 0x14000;
                t4.a = xv2;
                t4.b = yv2;
                yv2 = self->posY - 0xc000;
                xv2 = self->posX + 0xa000;
                t5.a = xv2;
                t5.b = yv2;
                yv2 = self->posY - 0x1000;
                xv2 = self->posX + 0xa000;
                t6.a = xv2;
                t6.b = yv2;
                r = func_ov006_02114590(((char *)self), &p2, &t4, &t5, &t6);
                if (r == 0) goto in2;
                return 0;
            in2:
                return 1;
            }
            return 0;
        }
        return 0;
    }
    if (func_0203d5bc(&d.a) < 0x100000)
        return 0;
    return func_0203d5bc(&d.a) < 0x190000;
}
