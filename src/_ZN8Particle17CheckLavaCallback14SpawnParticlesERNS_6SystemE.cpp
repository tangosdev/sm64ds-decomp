//cpp
struct Vector3 { int x, y, z; };
struct Actor;

struct RaycastGround {
    char pad0[0x14];
    unsigned int sub[(0x50 - 0x14) / 4];
    RaycastGround();
    ~RaycastGround();
    void SetObjAndPos(const Vector3 &pos, Actor *obj);
    int DetectClsn();
};

struct Node {
    Node *next;
    char p4[4];
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    char p20[0x2c - 0x20];
    unsigned short f2c;
    unsigned short f2e;
};

struct System;

extern "C" int func_02037e38(unsigned int *p);

namespace Particle {
struct System {};
struct SimpleCallback { void SpawnParticles(System &sys); };
struct CheckLavaCallback : SimpleCallback { void SpawnParticles(System &sys); };

void CheckLavaCallback::SpawnParticles(System &sys)
{
    SimpleCallback::SpawnParticles(sys);

    for (Node *n = *(Node **)((char *)&sys + 8); n != 0; n = n->next) {
        int sx = n->f14 + n->f8;
        int sy = n->f18 + n->fc;
        int sz = n->f1c + n->f10;
        RaycastGround rg;
        int vy = (sy << 3) + 0x12c000;
        Vector3 v;
        v.x = vy ? sx << 3 : sx << 3;
        v.y = vy;
        v.z = sz << 3;
        rg.SetObjAndPos(v, 0);
        if (rg.DetectClsn() == 0)
            goto Lac;
        if (func_02037e38(rg.sub) != 1) {
        Lac:
            n->f2e = n->f2c;
        } else {
            n->f18 = ((int)rg.sub[12] + 0x7000 >> 3) - n->fc;
        }
    }
}
}
