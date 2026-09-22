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
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~daObjKb1Billboard_c() {}   /* no slot */
#else
    virtual ~daObjKb1Billboard_c() {}   /* D1 and D0 */
#endif

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Render();

    /* Leaf operator new until fBase_c can declare it (#2570). unsigned long
       is size_t on this ABI; unsigned int is illegal. */
    static void *operator new(size_t size);

private:
    /* Ownership, body, call site, and codegen are proven. The readable name is
     * inferred because no original symbol string survives. */
    void UpdateModelTransform();
};

inline void *daObjKb1Billboard_c::operator new(size_t size)
{
    return _ZN7fBase_cnwEj((unsigned)size);
}

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKb1Billboard_c_size_must_be_0x124[
    sizeof(daObjKb1Billboard_c) == 0x124 ? 1 : -1];
#endif

#endif /* DAOBJKB1BILLBOARD_C_H */
