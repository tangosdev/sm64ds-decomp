//cpp
// @symbol _ZN21MegaMushroomCreateTag8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MegaMushroomCreateTag.h"
typedef int Fix12i;

extern "C" Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);

struct fBase_c {
    void MarkForDestruction();
};

struct dActor_c : fBase_c {
    char pad0[0xc];
    unsigned short actorID;   /* 0xc */
    char pad1[0x5c - 0xe];
    Vector3 pos;              /* 0x5c */
    char pad2[0x108 - 0x68];
    unsigned char b108;       /* 0x108 */
    unsigned char b109;       /* 0x109 */
    unsigned char b10a;       /* 0x10a */
    unsigned char b10b;       /* 0x10b */
    unsigned char b10c;       /* 0x10c */
    char pad4[0x32c - 0x10d];
    dActor_c *p32c;              /* 0x32c */
    static dActor_c *FindWithActorID(unsigned int id, dActor_c *after);
};


int MegaMushroomCreateTag::Behavior()
{
    dActor_c *o;
    int isTarget = (int)(((dActor_c *)this)->actorID == 0x140);
    if (isTarget) {
        if (!((dActor_c *)this)->b10b) {
            o = dActor_c::FindWithActorID(0x1b, 0);
            while (o) {
                if (Vec3_Dist(&((dActor_c *)this)->pos, &o->pos) < 0x96000) {
                    ((dActor_c *)this)->b108 = 1;
                    o->p32c = ((dActor_c *)this);
                    ((dActor_c *)this)->b10b = 1;
                    return 1;
                }
                o = dActor_c::FindWithActorID(0x1b, o);
            }
            ((dActor_c *)this)->b10b = 1;
        }
    }
    if (!((dActor_c *)this)->b10a) {
        o = dActor_c::FindWithActorID(0x13f, 0);
        while (o) {
            if (((dActor_c *)this)->b109 == o->b109) ((dActor_c *)this)->b10a = 1;
            o = dActor_c::FindWithActorID(0x13f, o);
        }
        if (!((dActor_c *)this)->b10a) ((dActor_c *)this)->MarkForDestruction();
    }
    isTarget = (int)(((dActor_c *)this)->actorID == 0x140);
    if (isTarget) {
        if (((dActor_c *)this)->b108 == 1) {
            if (((dActor_c *)this)->b10c) func_ov002_020b4714(((dActor_c *)this));
        } else {
            func_ov002_020b47ec(((dActor_c *)this));
        }
    }
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}
