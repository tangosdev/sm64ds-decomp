//cpp
struct Vector3 { int x, y, z; };
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

extern void func_ov002_020b4714(void *self);
extern void func_ov002_020b47ec(void *self);

extern "C" int func_ov002_020b4850(Actor *self)
{
    Actor *o;
    int isTarget = (int)(self->actorID == 0x140);
    if (isTarget) {
        if (!self->b10b) {
            o = Actor::FindWithActorID(0x1b, 0);
            while (o) {
                if (Vec3_Dist(&self->pos, &o->pos) < 0x96000) {
                    self->b108 = 1;
                    o->p32c = self;
                    self->b10b = 1;
                    return 1;
                }
                o = Actor::FindWithActorID(0x1b, o);
            }
            self->b10b = 1;
        }
    }
    if (!self->b10a) {
        o = Actor::FindWithActorID(0x13f, 0);
        while (o) {
            if (self->b109 == o->b109) self->b10a = 1;
            o = Actor::FindWithActorID(0x13f, o);
        }
        if (!self->b10a) self->MarkForDestruction();
    }
    isTarget = (int)(self->actorID == 0x140);
    if (isTarget) {
        if (self->b108 == 1) {
            if (self->b10c) func_ov002_020b4714(self);
        } else {
            func_ov002_020b47ec(self);
        }
    }
    self->clsn.Clear();
    self->clsn.Update();
    return 1;
}
