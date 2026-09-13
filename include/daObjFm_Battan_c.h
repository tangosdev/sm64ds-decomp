#ifndef DAOBJFM_BATTAN_C_H
#define DAOBJFM_BATTAN_C_H

#include "dBgActor_c.h"
#include "ShadowModel.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* ROM-proven class identity: _ZTI16daObjFm_Battan_c and
 * _ZTS16daObjFm_Battan_c at ov023:0x02111fa0/0x02111fac. ov023 is
 * SQUASHER(84) -- Shifting Sand Land's squasher, not a BoB/WF/BitDW actor. */
struct daObjFm_Battan_c : dBgActor_c {
    s16 mAngVelX;                 /* 0x31e */
    u16 mStateTimer;              /* 0x320 */
    u8 mState;                    /* 0x322 */
    ShadowModel mShadowModel;     /* 0x324 -- 1-byte alignment pad after mState is implicit */
    Matrix4x3 mShadowMat;         /* 0x34c */

    /* InitResources is deliberately the first out-of-line virtual/key
     * function. mwccarm then owns this class's RTTI/vtable and emits the
     * retail D1/D0 pair from this inline destructor, with no retained D2. */
    virtual ~daObjFm_Battan_c() {}
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    /* Leaf operator new until #2570 puts the same allocator on fBase_c.
       Parameter is unsigned long (size_t on this ABI). `return new`
       relocates to `_Znwm` without this. */
    static void *operator new(unsigned long size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

private:
    /* The class ownership and call targets are proven. These readable private
     * spellings are inferred aliases; no original symbol strings survive. */
    int UpdateShadow();
    void UpdateModelTransform();
};

typedef char daObjFm_Battan_c_size_must_be_0x37c[
    sizeof(daObjFm_Battan_c) == 0x37c ? 1 : -1];

#endif /* DAOBJFM_BATTAN_C_H */
