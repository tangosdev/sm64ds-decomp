//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vector3;

namespace Particle {
    struct EffectData;
    struct Acceleration { static void Func(EffectData&, char*, Vector3&); };
    struct Jitter { static void Func(EffectData&, char*, Vector3&); };
    struct Converge { static void Func(EffectData&, char*, Vector3&); };
    struct Turn { static void Func(EffectData&, char*, Vector3&); };
    struct LimitPlane { static void Func(EffectData&, char*, Vector3&); };
    struct RadiusConverge { static void Func(EffectData&, char*, Vector3&); };
}

typedef void (*BehaviorFunc)(Particle::EffectData&, char*, Vector3&);

struct Behavior {
    BehaviorFunc func;
    void* data;
};

struct PEntry {
    void* src;
    void* fa;
    void* fb;
    void* fc;
    void* fd;
    void* fe;
    Behavior* behaviors;
    u16 count;
};

struct SEntry {
    void* src;
    char pad4[8];
    int raw;
    u16 s10;
    u16 s12;
};

struct EData {
    void* (*alloc)(int);
    char pad4[0x18];
    PEntry* particles;
    SEntry* seconds;
    u16 numA;
    u16 numB;
};

struct Flags1 {
    unsigned f0_7 : 8;
    unsigned hasA : 1;
    unsigned hasB : 1;
    unsigned hasC : 1;
    unsigned hasD : 1;
    unsigned f12_15 : 4;
    unsigned hasE : 1;
    unsigned f17_23 : 7;
    unsigned hasAccel : 1;
    unsigned hasJitter : 1;
    unsigned hasConverge : 1;
    unsigned hasTurn : 1;
    unsigned hasLimitPlane : 1;
    unsigned hasRadiusConverge : 1;
    unsigned f30_31 : 2;
};

struct Flags2 {
    unsigned f0_3 : 4;
    unsigned val1 : 4;
    unsigned val2 : 4;
    unsigned rest : 20;
};

extern "C" {

void func_0204a17c(EData* t, char* d)
{
    int off = 0x20;
    int i;

    t->numA = *(u16*)(d + 8);
    t->numB = *(u16*)(d + 0xa);
    t->particles = (PEntry*)t->alloc(t->numA << 5);

    for (i = 0; i < t->numA; i++) {
        PEntry* p = (PEntry*)((char*)t->particles + i * 0x20);
        p->src = d + off;
        off += 0x38;
        Flags1 flags = *(Flags1*)p->src;

        if (flags.hasA) { p->fa = d + off; off += 0xc; }
        else p->fa = 0;

        if (flags.hasB) { p->fb = d + off; off += 0xc; }
        else p->fb = 0;

        if (flags.hasC) { p->fc = d + off; off += 8; }
        else p->fc = 0;

        if (flags.hasD) { p->fd = d + off; off += 0xc; }
        else p->fd = 0;

        if (flags.hasE) { p->fe = d + off; off += 0x14; }
        else p->fe = 0;

        p->count = flags.hasAccel + flags.hasJitter + flags.hasConverge
                 + flags.hasTurn + flags.hasLimitPlane + flags.hasRadiusConverge;

        if (p->count != 0) {
            p->behaviors = (Behavior*)t->alloc(p->count * 8);
            Behavior* b = p->behaviors;

            if (flags.hasAccel) {
                b->data = d + off;
                b->func = &Particle::Acceleration::Func;
                off += 8;
                b++;
            }
            if (flags.hasJitter) {
                b->data = d + off;
                b->func = &Particle::Jitter::Func;
                off += 8;
                b++;
            }
            if (flags.hasConverge) {
                b->data = d + off;
                b->func = &Particle::Converge::Func;
                off += 0x10;
                b++;
            }
            if (flags.hasTurn) {
                b->data = d + off;
                b->func = &Particle::Turn::Func;
                off += 4;
                b++;
            }
            if (flags.hasLimitPlane) {
                b->data = d + off;
                b->func = &Particle::LimitPlane::Func;
                off += 8;
                b++;
            }
            if (flags.hasRadiusConverge) {
                b->data = d + off;
                b->func = &Particle::RadiusConverge::Func;
                off += 0x10;
                b++;
            }
        } else {
            p->behaviors = 0;
        }
    }

    t->seconds = (SEntry*)t->alloc(t->numB * 0x14);

    for (i = 0; i < t->numB; i++) {
        SEntry* s = &t->seconds[i];
        char* rec = d + off;
        s->src = rec;
        Flags2* f2 = (Flags2*)(rec + 4);
        s->s10 = 1 << (f2->val1 + 3);
        s->s12 = 1 << (f2->val2 + 3);
        s->raw = *(int*)(rec + 4);
        off += *(int*)(rec + 0x1c);
    }
}

}
