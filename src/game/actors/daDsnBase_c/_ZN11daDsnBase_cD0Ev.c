// @symbol _ZN11daDsnBase_cD0Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int _ZTV11daDsnBase_c[];
extern int _ZTV10dBgActor_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV11daDsnBase_c */
extern void _ZN15TextureSequenceD1Ev(void *);
extern void *data_020a0eac;
/* daDsnBase_c::~daDsnBase_c() [D0, deleting], slot 17. Same reasoning as
   the D1 file above: the class's own destructor stays inline, but the
   vtable still needs a real deleting-destructor entry, spelled directly
   under its mangled name. */
int *_ZN11daDsnBase_cD0Ev(int *t)
{
    t[0] = (int)_ZTV11daDsnBase_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x338);
    _ZN15TextureSequenceD1Ev((char *)t + 0x324);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN10dBgW_KcMbgD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
