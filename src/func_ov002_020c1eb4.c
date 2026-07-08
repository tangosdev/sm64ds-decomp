//cpp
// NONMATCHING: base materialization / addressing (div=28). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vector3 { int x, y, z; };
struct Vector3_16f { short x, y, z; };
typedef int Fix12i;

extern "C" void Vec3_RotateYAndTranslate(Vector3 *out, const Vector3 *origin,
                                         int angle, const Vector3 *offset);
extern short SINE_TABLE[];

struct Sound {
    static void PlayBank0(unsigned int id, const Vector3 &v);
};

struct Player {
    struct State;
    void ChangeState(State &s);
};
extern Player::State _ZN6Player7ST_HURTE;

namespace Particle {
    struct Callback;
    struct System {
        static void New(unsigned int id, unsigned int a, Fix12i x, Fix12i y, Fix12i z,
                        const Vector3_16f *v, Callback *cb);
    };
}

extern "C" int func_ov002_020d91e0(char *thiz, int damage, int doPre);

struct Obj {
    char pad0[0x5c];
    Vector3 p5c;       /* 0x5c */
    char pad1[0x74 - 0x68];
    Vector3 p74;       /* 0x74 */
    char pad2[0x8c - 0x80];
    short p8c;         /* 0x8c */
    short p8e;         /* 0x8e */
    short p90;         /* 0x90 */
    char pad3[0x94 - 0x92];
    short p94;         /* 0x94 */
    char pad4[0x98 - 0x96];
    int p98;           /* 0x98 */
    char pad5[0x674 - 0x9c];
    int p674;          /* 0x674 */
    char pad6[0x6de - 0x678];
    unsigned char b6de; /* 0x6de */
    char pad7[0x6e3 - 0x6df];
    unsigned char b6e3; /* 0x6e3 */
};

extern "C" void func_ov002_020c1eb4(Obj *self, int dmg)
{
    Vector3 out;
    Vector3 offset;
    Vector3_16f vec;

    Sound::PlayBank0(0x13, self->p74);
    self->p8e = self->p94;
    self->p90 = 0;
    self->p8c = self->p90;
    self->b6e3 = 0;
    if (self->b6de) {
        unsigned char *q = (unsigned char *)self + 0x6e3;
        *q = *q + 2;
    }
    self->p8e = (short)(dmg * 2 - self->p8e);
    {
        unsigned char *p = (unsigned char *)self + 0x6e3;
        self->p94 = (short)(self->p8e + 0x8000);
        self->p98 = 0xa000;
        *p = *p | 0x20;
    }
    self->p674 = 0;
    ((Player *)self)->ChangeState(_ZN6Player7ST_HURTE);

    offset.x = 0;
    offset.y = 0x32000;
    offset.z = 0x28000;
    Vec3_RotateYAndTranslate(&out, &self->p5c, (short)(dmg + 0x8000), &offset);

    {
        int idx = ((unsigned short)dmg >> 4);
        vec.x = SINE_TABLE[idx * 2];
        vec.z = SINE_TABLE[idx * 2 + 1];
        vec.y = 0;
        Particle::System::New(0, 0xc8, out.x, out.y, out.z, &vec, 0);
    }
    func_ov002_020d91e0((char *)self, 0, 1);
}
