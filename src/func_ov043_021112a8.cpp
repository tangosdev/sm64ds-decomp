//cpp
// @symbol func_ov043_021112a8
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjKm1_Ukishima_c.h"
// @emits daObjKm1_Ukishima_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjKm1_Ukishima_c::Behavior - recovered from vtable slot identity */
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
int daObjKm1_Ukishima_c_Behavior(char *c)
{
    struct daObjKm1_Ukishima_c *self = (struct daObjKm1_Ukishima_c *)(void *)c;
    if (!DecIfAbove0_Byte((unsigned char *)(c + 0x31e))) {
        self->unk_31e = 0x3c;
        *(short *)(((int)c + 0x94)) = *(short *)(((int)c + 0x94)) + 0x4000;
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
}
