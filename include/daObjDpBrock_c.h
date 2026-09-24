#ifndef DAOBJDPBROCK_C_H
#define DAOBJDPBROCK_C_H

#include "types.h"

/* DP_BROCK, a pyramid step block that sinks and rises on a 100-frame cycle.
 * RTTI names the class directly: _ZTS14daObjDpBrock_c and
 * _ZTI14daObjDpBrock_c at ov025 0x021138dc and 0x021138d0, and the typeinfo
 * names the vtable at 0x02113914 as this class's. The factory allocates
 * fBase_c::operator new(932) == 0x3a4. The tree called the class PyramidStep
 * until the rename.
 *
 * Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys this class's own Model at 0x320,
 * the dBgW_KcMbg at 0x124 and the Model at 0xd4 before chaining to dActor_c.
 *
 * On top of dBgActor_c's model and collider it carries a second model and a
 * second collider matrix of its own, kept in step by the two helpers in
 * src/actors/daObjDpBrock_c.cpp.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "Model.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daObjDpBrock_c : dBgActor_c {
    u8  pad_31e[0x2];
    /* The class's own model. NOT "mModel": dBgActor_c's inherited
       Model at 0xd4 already owns that name. */
    Model mStepModel;                 /* 0x320 */
    u16 mStateTimer;                  /* 0x370 - frames in the current state */
    u8  mState;                       /* 0x372 - 0 sinking, 1 rising */
    u8  pad_373[0x1];
    /* Where this class's collider sits. InitResources hands it to
       dBgW_KcMbg::SetFile, and 0x374 + 0x30 = 0x3a4 is the factory's
       operator new size, so the matrix is the last member. */
    Matrix4x3 mClsnMat2;              /* 0x374 */

    /* --- vtable --- */
    /* INLINE ON PURPOSE. A destructor defined in the class body makes
       mwccarm 2004/b56 emit D1 then D0 and no D2, the cartridge's order
       (0x02111d40 then 0x02111d8c). It also moves this class's key function
       to Behavior, so the TU defining Behavior emits _ZTV14daObjDpBrock_c. */
    virtual ~daObjDpBrock_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Leaf operator new, the same form as include/daDkk_c.h. unsigned long,
       not unsigned int: the global ::operator new an implicit `new` would
       otherwise call is `_Znwm`. */
    static void *operator new(size_t size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjDpBrock_c_size_must_be_0x3a4[sizeof(daObjDpBrock_c) == 0x3a4 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJDPBROCK_C_H */
