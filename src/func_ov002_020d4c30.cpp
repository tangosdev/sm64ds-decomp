//cpp
struct Vector3 { int x, y, z; };
typedef int Fix12i;

extern "C" unsigned int func_02022d00(unsigned int uniqueID, unsigned int effectID,
                                      Fix12i x, Fix12i y, Fix12i z, void *dir);
extern "C" void func_0201251c(int a, int b, int c, int d);
extern int data_0209f32c;
extern int data_0208e430;

struct Sound {
    static void PlayBank0(unsigned int id, const Vector3 &v);
};

namespace Particle {
    struct System {
        static void NewSimple(unsigned int id, Fix12i a, Fix12i b, Fix12i c);
    };
}

struct Obj {
    char pad0[0x5c];
    Fix12i p5c;     /* 0x5c */
    Fix12i p60;     /* 0x60 */
    Fix12i p64;     /* 0x64 */
    char pad1[0x74 - 0x68];
    Vector3 p74;    /* 0x74 */
    char pad2[0x98 - 0x80];
    int p98;        /* 0x98 */
    char pad3[0x628 - 0x9c];
    unsigned int p628; /* 0x628 */
    char pad4[0x66c - 0x62c];
    int p66c;       /* 0x66c */
    char pad5[0x68c - 0x670];
    int p68c;       /* 0x68c */
    char pad6[0x6e3 - 0x690];
    unsigned char b6e3; /* 0x6e3 */
    char pad7[0x6f9 - 0x6e4];
    unsigned char b6f9; /* 0x6f9 */
    char pad8[0x707 - 0x6fa];
    unsigned char b707; /* 0x707 */
};

extern "C" void func_ov002_020d4c30(Obj *self)
{
    int id;
    if (self->b707) {
        self->p628 = func_02022d00(self->p628, 0xde, self->p5c, data_0209f32c,
                                   self->p64, 0);
    }
    if (self->p68c > 0x32000) {
        Sound::PlayBank0(0xb3, self->p74);
    } else if (self->b6e3 >= 0x1e) {
        if (self->b6f9 == 0) {
            Sound::PlayBank0(self->p66c + 0x70, self->p74);
        } else {
            Sound::PlayBank0(0xa4, self->p74);
        }
    } else {
        if (self->b6f9 == 0) {
            func_0201251c(0, self->p66c + 0x20, (int)&self->p74, self->p98);
        } else {
            func_0201251c(0, 0xaa, (int)&self->p74, self->p98);
        }
    }
    if (data_0208e430 == 0x2e) return;
    id = -1;
    if (self->p66c != 6) {
        if (self->p66c != 8) {
            if (self->p66c == 9) id = 0xe6;
        } else id = 0xe4;
    } else id = 0xe5;
    if (id == -1) return;
    Particle::System::NewSimple(id, self->p5c, self->p60, self->p64);
}
