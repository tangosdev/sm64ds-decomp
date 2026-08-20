// @symbol _ZN10SlidingBoxD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
extern int _ZTV10dBgActor_c[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV13daSlide_Box_c; VT1 = _ZTV10dBgActor_c */
int *_ZN10SlidingBoxD1Ev(int *t)
{
    t[0] = (int)_ZTV13daSlide_Box_c;
    _ZN10dBgCh_ActrD1Ev((char *)t + 0x324);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN10dBgW_KcMbgD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
