//cpp
#include "dBgCh_Actr.h"

struct dCc_c;
struct dActor_c { void UpdatePos(dCc_c *c); };
extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *p);
extern "C" dBgPi *_ZNK10dBgCh_Actr14GetFloorResultEv(const dBgCh_Actr *self);
extern "C" int func_02037e38(unsigned int *p);
extern "C" void func_ov071_0211f498(char *c);
extern "C" void Scuttlebug_SetState(char *c, int x);
extern "C" void func_ov071_0211f29c(char *c);
struct dCc_c2 { void Clear(); void Update(); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN5dCc_c5ClearEv(void *);
extern "C" void _ZN5dCc_c6UpdateEv(void *);


extern "C" int func_ov071_0211f7d4(dActor_c *self)
{
    char *s = (char*)self;
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(s + 0x194);
    *(short*)(s + 0x8c) = *(short*)(s + 0x8c) + 0x1000;
    if (((dBgCh_Actr*)(s + 0x194))->JustHitGround()) {
        if (func_02037e38((unsigned int*)((char*)_ZNK10dBgCh_Actr14GetFloorResultEv((dBgCh_Actr*)(s + 0x194)) + 4)) == 4) {
            func_ov071_0211f498(s);
        } else {
            *(int*)(s + 0xa8) = (*(int*)(s + 0xa8) * -0x3c) / 0x64;
        }
    } else if (((dBgCh_Actr*)(s + 0x194))->IsOnGround()) {
        dBgCh_Actr *wm = (dBgCh_Actr*)(s + 0x194);
        *(int*)(s + 0xa8) = 0;
        wm->ClearLimMovFlag();
        *(int *)(s + 0xb0) |= 1;
        short z = 0;
        short ang = *(short*)(s + 0x94);
        *(short*)(s + 0x8c) = z;
        *(short*)(s + 0x8e) = ang;
        *(short*)(s + 0x90) = z;
        Scuttlebug_SetState(s, 2);
    }
    self->UpdatePos((dCc_c*)(s + 0x160));
    func_ov071_0211f29c(s);
    _ZN5dCc_c5ClearEv((dCc_c2*)(s + 0x160));
    _ZN5dCc_c6UpdateEv((dCc_c2*)(s + 0x160));
    return 1;
}
