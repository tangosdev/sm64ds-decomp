//cpp
// @symbol func_ov102_0214aa18
/* recovered: shared common types */
#include "common.h"
struct dCc_c { void Clear(); void Update(); };
struct dBgCh_Actr;
struct dActor_c { void UpdatePos(dCc_c *c); };
struct dEnemyBase_c { void UpdateWMClsn(dBgCh_Actr &w, unsigned int j); };
struct WithMeshClsn2 { int JustHitGround() const; int IsOnGround() const; };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZNK10dBgCh_Actr13JustHitGroundEv(void *);
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void *);

extern "C" void func_ov102_0214b53c(char *c);
extern "C" void func_ov102_0214ad40(char *c);
extern "C" void func_ov102_0214c0b8(char *c);
extern "C" void func_0200fc44(char *c, void *v, int x);



extern "C" int func_ov102_0214aa18(dActor_c *self)
{
    char *s = (char*)self;
    if (*(unsigned char*)(s + 0x3f5) == 2) {
        if (*(int*)(s + 0xc8) != 0) {
            func_ov102_0214b53c(s);
            return 1;
        }
        self->UpdatePos((dCc_c*)(s + 0x110));
        ((dEnemyBase_c*)self)->UpdateWMClsn(*(dBgCh_Actr*)(s + 0x144), 0);
        func_ov102_0214b53c(s);
        func_ov102_0214ad40(s);
        ((dCc_c*)(s + 0x110))->Clear();
        ((dCc_c*)(s + 0x110))->Update();
        if (_ZNK10dBgCh_Actr13JustHitGroundEv((WithMeshClsn2*)(s + 0x144))) {
            Vector3 v;
            v.x = *(int*)(s + 0x5c);
            v.y = *(int*)(s + 0x60);
            v.z = *(int*)(s + 0x64);
            func_0200fc44(s, &v, 1);
        }
        if (_ZNK10dBgCh_Actr10IsOnGroundEv((WithMeshClsn2*)(s + 0x144)) == 0)
            return 1;
        int *fl = (int*)(((int)s + 0x128));
        *fl = *fl & ~2;
        *(unsigned char*)(s + 0x3f5) = 3;
        func_ov102_0214c0b8(s);
        return 1;
    }
    return 0;
}
