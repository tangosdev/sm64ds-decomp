//cpp
typedef int Fix12;
struct BCA_File;
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };

struct BlendModelAnim {
    int SetAnim(BCA_File& f, int a, int b, Fix12 spd, unsigned short t);
};
struct Actor {
    static Actor* FindWithID(unsigned int id);
    static unsigned int Spawn(unsigned int a, unsigned int b, const Vector3& pos,
                       const Vector3_16* rot, int e, int f);
};
struct ActorBase { void MarkForDestruction(); };
namespace Particle { struct System {
    static void NewSimple(unsigned int id, Fix12 x, Fix12 y, Fix12 z);
}; }

extern "C" void func_02012790(int);
extern "C" void func_02012694(unsigned int, void*);

struct Data { BCA_File* a; BCA_File* f; };
extern Data data_ov062_0211e10c;

struct Found {
    char p[0x9c];
    int f9c, fa0;
    char g[0xc8-0xa4];
    int fc8;
};

struct Obj {
    char p0[0x5c];
    Vector3 f5c;
    char g68[0x8c-0x68];
    Vector3_16 f8c;
    char g92[0x9c-0x92];
    int f9c, fa0, fa4, fa8, fac;   /* 9c,a0,a4,a8,ac */
    char gb0[0xcc-0xb0];
    signed char fcc;
    char g[0x334-0xcd];
    BlendModelAnim anim;
    char g2[0x43c-0x338];
    int f43c;
    char g3[0x444-0x440];
    short f444;
    char g3b[0x448-0x446];
    unsigned char f448;
    char g4[0x44c-0x449];
    unsigned int f44c;
    char g5[0x468-0x450];
    int f468;
    unsigned int f46c;
};

extern "C" int func_ov062_0211b930(Obj* o)
{
    Actor* found;
    o->anim.SetAnim(*data_ov062_0211e10c.f, 4, 0x40000000, 0x1000, 0);
    o->f43c = 1;
    if (o->f44c != 0 && (found = Actor::FindWithID(o->f44c)) != 0) {
        Found* f = (Found*)found;
        f->f9c = -0x2000;
        f->fa0 = -0x32000;
        f->fc8 = 0;
        if (o->f468 == 1) {
            ((ActorBase*)found)->MarkForDestruction();
            if (o->f448 != 2) {
                Actor::Spawn(0xb2, o->f46c | 0x40, o->f5c, &o->f8c, o->fcc, -1);
            } else {
                Actor::Spawn(0xb3, 0x10, o->f5c, &o->f8c, o->fcc, -1);
                o->f468 = 0;
            }
        } else {
            func_02012790(0xa);
        }
        o->f43c = 0;
        o->f44c = 0;
        o->f444 = 0x1e;
    }
    func_02012694(0xef, (char*)o + 0x74);
    Particle::System::NewSimple(0x7e, o->f5c.x, o->f5c.y, o->f5c.z);
    o->fa4 = 0;
    o->fa8 = 0;
    o->fac = 0;
    o->f9c = 0;
    return 1;
}
