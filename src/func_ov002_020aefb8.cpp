//cpp
#include "dBgCh_Actr.h"

struct dCc_c;
struct dActor_c {
    void UpdatePosWithHorzSpeedAndAng();
    void UpdatePosWithOnlySpeed(dCc_c*);
    short ReflectAngle(int, int, short);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *, int, int, short);

struct dEnemyBase_c : dActor_c {
    void UpdateWMClsn(dBgCh_Actr&, unsigned int);
};
extern "C" int Vec3_HorzLen(void*);

extern "C" void func_ov002_020aefb8(char* self);
void func_ov002_020aefb8(char* self) {
    int *px;
    int *pz;
    ((dActor_c*)self)->UpdatePosWithHorzSpeedAndAng();
    if (((dBgCh_Actr*)(self + 0x144))->IsOnGround()) {
        px = (int*)(int)(self + 0xa4);
        *px += *(int*)(self + 0xd4) * 0xa;
        pz = (int*)(int)(self + 0xac);
        *pz += *(int*)(self + 0xdc) * 0xa;
        if (((dBgCh_Actr*)(self + 0x144))->JustHitGround()) {
            *(int*)(self + 0xa8) = -(*(int*)(self + 0xa8) << 2) / 10;
        } else {
            *(int*)(self + 0xa8) = 0;
        }
        if (Vec3_HorzLen(self + 0xa4) > *(int*)(self + 0x98)) {
            *(int*)(self + 0x98) = Vec3_HorzLen(self + 0xa4);
            if (*(int*)(self + 0x98) >= 0xf000) *(int*)(self + 0x98) = 0xf000;
        }
    }
    ((dActor_c*)self)->UpdatePosWithOnlySpeed((dCc_c*)(self + 0x110));
    ((dEnemyBase_c*)self)->UpdateWMClsn(*(dBgCh_Actr*)(self + 0x144), 0);
    if (!((dBgCh_Actr*)(self + 0x144))->IsOnWall()) return;
    *(short*)(self + 0x94) = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s((dActor_c*)self, *(int*)(self + 0xe0), *(int*)(self + 0xe8), *(short*)(self + 0x94));
}
