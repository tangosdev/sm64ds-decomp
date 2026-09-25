//cpp
// @symbol _ZN11MirrorLuigiD1Ev
/* Real compiler-spelled complete destructor. The empty body is intentional:
 * mwccarm synthesises the reverse member teardown and dActor_c base chain from
 * MirrorLuigi's declared object layout. objisolate retains this D1 contribution
 * and discards D0/D2 plus the compatibility-name vtable and RTTI passengers. */
#include "MirrorLuigi.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D1 NAME, AND MSVC ONLY MAKES ONE DESTRUCTOR.
 * It folds the Itanium D1/D0 pair into the single ??1MirrorLuigi that
 * src/_ZN11MirrorLuigiD0Ev.cpp compiles for the port, so this file cannot carry
 * the same definition (LNK2005). This arm spells the ROM's own D1 instead
 * (overlay 55, 0x021111a0, 0x58 bytes), step for step: the store of this
 * class's own table, __cxa_vec_cleanup over the two TextureSequences at +0x1b0
 * with their D1, then the ShadowModel at +0x188, the Model at +0x138 and the
 * ModelAnim at +0xd4, each by its flat ROM name, then the dActor_c base D2, and
 * no deallocation (slot 16's caller frees). Those are the calls the slot-16
 * thunk ml_d1 in hal/actor_classes_ov055.cpp made by hand, which forwards here
 * now. Nothing here reaches mwccarm: it builds the `#else` arm and emits the
 * ROM bytes it always emitted, and the object is byte-identical either way. */
extern "C" {
int __cxa_vec_cleanup(void *base, int n, int stride, void *dtor);
void _ZN15TextureSequenceD1Ev(void *p);
void _ZN11ShadowModelD1Ev(void *p);
void _ZN5ModelD1Ev(void *p);
void _ZN9ModelAnimD1Ev(void *p);
void _ZN8dActor_cD2Ev(void *p);
extern void *_ZTV11MirrorLuigi[];
}
extern "C" MirrorLuigi *_ZN11MirrorLuigiD1Ev(MirrorLuigi *thiz)
{
    char *t = (char *)thiz;
    *(void **)t = (void *)_ZTV11MirrorLuigi;  /* this class's own table */
    __cxa_vec_cleanup(t + 0x1b0, 2, 0x14, (void *)&_ZN15TextureSequenceD1Ev);
    _ZN11ShadowModelD1Ev(t + 0x188);
    _ZN5ModelD1Ev(t + 0x138);
    _ZN9ModelAnimD1Ev(t + 0xd4);
    _ZN8dActor_cD2Ev(t);                      /* the base, no deallocation */
    return thiz;
}
#else

MirrorLuigi::~MirrorLuigi()
{
}
#endif
