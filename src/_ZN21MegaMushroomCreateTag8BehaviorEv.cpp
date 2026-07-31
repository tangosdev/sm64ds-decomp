//cpp
// @symbol _ZN21MegaMushroomCreateTag8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MegaMushroomCreateTag.h"
typedef int Fix12i;

extern "C" Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);

struct CylinderClsn {
    void Clear();
    void Update();
};

struct ActorBase {
    void MarkForDestruction();
};

struct Actor : ActorBase {
    char pad0[0xc];
    unsigned short actorID;   /* 0xc */
    char pad1[0x5c - 0xe];
    Vector3 pos;              /* 0x5c */
    char pad2[0xd4 - 0x68];
    CylinderClsn clsn;        /* 0xd4 */
    char pad2b[0x108 - 0xd5];
    unsigned char b108;       /* 0x108 */
    unsigned char b109;       /* 0x109 */
    unsigned char b10a;       /* 0x10a */
    unsigned char b10b;       /* 0x10b */
    unsigned char b10c;       /* 0x10c */
    char pad4[0x32c - 0x10d];
    Actor *p32c;              /* 0x32c */
    static Actor *FindWithActorID(unsigned int id, Actor *after);
};


int MegaMushroomCreateTag::Behavior()
{
    Actor *o;
    int isTarget = (int)(((Actor *)this)->actorID == 0x140);
    if (isTarget) {
        if (!((Actor *)this)->b10b) {
            o = Actor::FindWithActorID(0x1b, 0);
            while (o) {
                if (Vec3_Dist(&((Actor *)this)->pos, &o->pos) < 0x96000) {
                    ((Actor *)this)->b108 = 1;
                    o->p32c = ((Actor *)this);
                    ((Actor *)this)->b10b = 1;
                    return 1;
                }
                o = Actor::FindWithActorID(0x1b, o);
            }
            ((Actor *)this)->b10b = 1;
        }
    }
    if (!((Actor *)this)->b10a) {
        o = Actor::FindWithActorID(0x13f, 0);
        while (o) {
            if (((Actor *)this)->b109 == o->b109) ((Actor *)this)->b10a = 1;
            o = Actor::FindWithActorID(0x13f, o);
        }
        if (!((Actor *)this)->b10a) ((Actor *)this)->MarkForDestruction();
    }
    isTarget = (int)(((Actor *)this)->actorID == 0x140);
    if (isTarget) {
        if (((Actor *)this)->b108 == 1) {
            if (((Actor *)this)->b10c) func_ov002_020b4714(((Actor *)this));
        } else {
            func_ov002_020b47ec(((Actor *)this));
        }
    }
    ((Actor *)this)->clsn.Clear();
    ((Actor *)this)->clsn.Update();
    return 1;
}
