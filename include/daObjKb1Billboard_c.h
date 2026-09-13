#ifndef DAOBJKB1BILLBOARD_C_H
#define DAOBJKB1BILLBOARD_C_H

#include "types.h"
#include "Model.h"
#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* The orange-ball billboard (profile KB1_BILLBOARD / ORANGE_BALL_BILLBOARD(301)).
 *
 * ROM-proven class identity: _ZTI19daObjKb1Billboard_c and
 * _ZTS19daObjKb1Billboard_c at ov044:0x021115bc/0x021115c8. Direct dActor_c
 * subclass. Factory allocates 0x124: dActor_c 0xd0, four-byte gap, Model at
 * 0xd4.
 */
struct daObjKb1Billboard_c : dActor_c {
    u8    pad_0d0[0x4];
    Model mModel;          /* 0x0d4 */

    /* InitResources is the first out-of-line virtual/key function. The inline
     * destructor lets mwccarm emit the retail D1/D0 pair and class RTTI/vtable. */
    virtual ~daObjKb1Billboard_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Render();

    /* Leaf operator new until fBase_c can declare it (#2570). unsigned long
       is size_t on this ABI; unsigned int is illegal. */
    static void *operator new(unsigned long size);

private:
    /* Ownership, body, call site, and codegen are proven. The readable name is
     * inferred because no original symbol string survives. */
    void UpdateModelTransform();
};

inline void *daObjKb1Billboard_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj((unsigned)size);
}

typedef char daObjKb1Billboard_c_size_must_be_0x124[
    sizeof(daObjKb1Billboard_c) == 0x124 ? 1 : -1];

#endif /* DAOBJKB1BILLBOARD_C_H */
