#include "types.h"
#include "nitro/fx/fx.h"
#include "dExtModel_c.h"
#include "Ov007Work.h"
/* AddVec3 still defines its vector as its own struct Vec3;
   declared as defined, the call casts. */
struct Vec3;
extern void AddVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc02c, only the slot read here. */
typedef struct Ov007Model {
    struct ModelComponents *model; /* 0x00 */
} Ov007Model;

extern void *data_ov007_0210342c;

void func_ov007_020b2454(Vector3 *out, Vector3 *r1) {
    int w;
    struct Ov007Work *m;
    Matrix4x3 *in;
    Vector3 *base;
    Vector3 b;
    Vector3 a;

    m = (struct Ov007Work *)data_ov007_0210342c;
    /* bone 0's transform */
    in = m->model->model->transforms;
    a.z = 0;
    a.y = 0;
    a.x = 0;
    base = &in->t;
    /* figureIndex is only ever 0 or 1 here; the ROM has no default arm either, so
       any other value would read w uninitialised. */
    switch (m->figureIndex) {
    case 0:
        a.y = FX_MUL((-in->r.m[4]), 0x8543);
        w = 0xbb8;
        r1->x = r1->x - 0x5000;
        break;
    case 1:
        a.y = FX_MUL(in->r.m[1], 0x987b);
        w = 0x960;
        r1->x = r1->x - 0xa000;
        break;
    }
    AddVec3((struct Vec3 *)base, (struct Vec3 *)&a, (struct Vec3 *)&b);
    out->x = base->x;
    out->y = (int)(((long long)(0x1000 - w) * b.y + (long long)w * base->y) >> 12);
}
