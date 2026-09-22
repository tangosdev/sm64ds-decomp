#ifndef DAOBJHSBILLBOARD_C_H
#define DAOBJHSBILLBOARD_C_H

#include "Model.h"
#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Slide decoration billboard. ov031 actors 302-305 share this one class.
 * ROM debug names: HS_MOON / HS_STAR / HS_Y_STAR / HS_B_STAR.
 * overlay_actors.md community names (not synonyms of those debug strings):
 * SLIDE_DECORATION_SILVER_STAR / YELLOW_STAR / ORANGE_SMILEY / BLUE_SMILEY.
 *
 * ROM-proven identity: _ZTI18daObjHsBillboard_c / _ZTS18daObjHsBillboard_c at
 * ov031:0x021118e8/0x021118f4 spell "18daObjHsBillboard_c"; the typeinfo base
 * is _ZTI8dActor_c. Size 0x128 is each factory's operator-new literal.
 * InitResources picks the SharedFilePtr in data_ov031_02111424 from actorID. */

struct daObjHsBillboard_c : dActor_c {
    u8    pad_0d0[0x4];
    Model mModel;          /* 0x0d4 */
    u8    mVariant;        /* 0x124 -- index into data_ov031_02111424 */

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
    ~daObjHsBillboard_c() {}   /* no slot */
#else
    virtual ~daObjHsBillboard_c() {}   /* D1 and D0 */
#endif

    virtual int InitResources();
    virtual int Render();
    virtual int CleanupResources();

    static void *operator new(size_t size);
};

inline void *daObjHsBillboard_c::operator new(size_t size)
{
    return _ZN7fBase_cnwEj((unsigned)size);
}

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjHsBillboard_c_size_must_be_0x128[
    sizeof(daObjHsBillboard_c) == 0x128 ? 1 : -1];
#endif

#endif /* DAOBJHSBILLBOARD_C_H */
