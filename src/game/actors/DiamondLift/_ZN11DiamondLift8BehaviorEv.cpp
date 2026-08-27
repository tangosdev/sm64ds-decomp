//cpp
// @symbol _ZN11DiamondLift8BehaviorEv
#include "DiamondLift.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjKm1_Ukishima_c.h"
// recovered name: daObjKm1_Ukishima_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjKm1_Ukishima_c::Behavior - recovered from vtable slot identity */
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
s32 DiamondLift::Behavior() {
    char * c = (char *)this;
    struct daObjKm1_Ukishima_c *self = (struct daObjKm1_Ukishima_c *)(void *)c;
    if (!DecIfAbove0_Byte((unsigned char *)(c + 0x31e))) {
        self->unk_31e = 0x3c;
        *(short *)(((int)c + 0x94)) = *(short *)(((int)c + 0x94)) + 0x4000;
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    return 1;
}
}
