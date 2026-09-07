//cpp
#include "dBgCh_Actr.h"

struct dCc_c;
struct dActor_c {
    static dActor_c* FindWithID(unsigned int id);
    void LandingDust(bool b);
    void UpdatePos(dCc_c* cyl);
};
struct Player {
    void JumpIntoBooCage(Vector3& v);
};
struct dEnemyBase_c {
    void UpdateWMClsn(dBgCh_Actr& w, unsigned int f);
};
struct dCc_c {
    void Clear();
    void Update();
};
extern "C" unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int x, int y, int z, const void* v, void* cb);
extern "C" int _ZN5Sound15PlaySecretSoundEP8dActor_cPt(void* a, unsigned short* t);
extern "C" void func_ov063_021169c4(char* c);

struct daTBasket_c {
    int Behavior();
};

int daTBasket_c::Behavior()
{
    char* self = (char*)this;
    int r4 = 0;
    int r6 = 1;

    if (*(unsigned char*)(self + 0x37e) == 0)
        r6 = _ZN5Sound15PlaySecretSoundEP8dActor_cPt(self, (unsigned short*)(self + 0x37c));

    if (((dBgCh_Actr*)(self + 0x144))->JustHitGround()) {
        int v = *(int*)(self + 0xa8);
        *(int*)(self + 0xa8) = (-v) >> 1;
        ((dActor_c*)self)->LandingDust(0);
    } else if (((dBgCh_Actr*)(self + 0x144))->IsOnGround()) {
        r4 = 1;
        /* secret non-zero skips timer gate; secret zero requires timer > 0x3c */
        if (r6 != 0 || *(unsigned short*)(self + 0x300 + 0x7c) > 0x3c) {
            unsigned int id = *(unsigned int*)(self + 0x134);
            if (id != 0) {
                dActor_c* a = dActor_c::FindWithID(id);
                if (a != 0) {
                    if ((*(unsigned int*)(self + 0x130) & 0x400000) != 0)
                        ((Player*)a)->JumpIntoBooCage(*(Vector3*)(self + 0x5c));
                }
            }
        }
    }

    if (r4 == 0) {
        int z = *(int*)(self + 0x64);
        int y = *(int*)(self + 0x60);
        int x = *(int*)(self + 0x5c);
        unsigned int pid = *(unsigned int*)(self + 0x378);
        *(unsigned int*)(self + 0x378) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            pid, 0x119, x, y + 0x64000, z, 0, 0);
    }

    ((dActor_c*)self)->UpdatePos(0);
    ((dEnemyBase_c*)self)->UpdateWMClsn(*(dBgCh_Actr*)(self + 0x144), 0);
    func_ov063_021169c4(self);
    ((dCc_c*)(self + 0x110))->Clear();
    ((dCc_c*)(self + 0x110))->Update();
    return 1;
}
