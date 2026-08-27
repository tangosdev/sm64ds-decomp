//cpp
// @symbol func_020371fc
/* recovered: shared common types */
#include "common.h"

struct dActor_c;
struct dBgPi { char data[0x34]; };

struct dBgCh_Gnd {
    char pad0[0x10];
    dBgPi clsn;   /* 0x10 */
    int field44;       /* 0x44 */
    char pad48[0x50 - 0x48];
    dBgCh_Gnd();
    ~dBgCh_Gnd();
    void SetObjAndPos(const Vector3& pos, dActor_c* a);
    int DetectClsn();
};

struct dBgCh_Actr { void SetGroundFlag(); };

extern "C" bool _ZN6Player7IsInAirEv(void* p);
extern "C" void _ZN5dBgPiaSERKS_(dBgPi* d, const dBgPi* s);

extern "C" void func_020371fc(char* self);
void func_020371fc(char* self)
{
    if ((*(unsigned char*)(self + 0x90) & 1) == 0) return;
    if ((*(unsigned char*)(self + 0x90) & 8) == 0) return;
    if ((*(int*)(self + 0x10) & 0x100) == 0) return;
    if (*(int*)(self + 0x1b0) >= 0xf20) return;
    if (_ZN6Player7IsInAirEv(*(void**)(self + 0x14))) return;
    {
        Vector3 pos;
        Vector3* objpos = (Vector3*)(((int)*(char**)(self + 0x14) + 0x5c));
        dBgCh_Gnd rg;
        pos.x = objpos->x;
        pos.y = objpos->y + *(int*)(self + 0x18);
        pos.z = objpos->z;
        rg.SetObjAndPos(pos, *(dActor_c**)(self + 0x14));
        if (rg.DetectClsn() != 0) {
            int cy = rg.field44;
            int diff = objpos->y - cy;
            if (diff > 0 && diff < (*(int*)(self + 0x18) << 1)) {
                objpos->y = cy;
                *(unsigned char*)(((int)self + 0x90)) |= 4;
                _ZN5dBgPiaSERKS_((dBgPi*)(self + 0x94), &rg.clsn);
                ((dBgCh_Actr*)self)->SetGroundFlag();
            }
        }
    }
}
