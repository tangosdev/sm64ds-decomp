#ifndef DAOBJFM_BATTAN_C_H
#define DAOBJFM_BATTAN_C_H

#include "dBgActor_c.h"
#include "ShadowModel.h"

/* ROM-proven class identity: _ZTI16daObjFm_Battan_c and
 * _ZTS16daObjFm_Battan_c at ov023:0x02111fa0/0x02111fac. */
struct daObjFm_Battan_c : dBgActor_c {
    s16 mAngVelX;                 /* 0x31e */
    u16 mStateTimer;              /* 0x320 */
    u8 mState;                    /* 0x322 */
    u8 pad_323;
    ShadowModel mShadowModel;     /* 0x324 */
    Matrix4x3 mShadowMat;         /* 0x34c */

    /* InitResources is deliberately the first out-of-line virtual/key
     * function. mwccarm then owns this class's RTTI/vtable and emits the
     * retail D1/D0 pair from this inline destructor, with no retained D2. */
    virtual ~daObjFm_Battan_c() {}
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

private:
    /* The class ownership and call targets are proven. These readable private
     * spellings are inferred aliases; no original symbol strings survive. */
    int UpdateShadow();
    void UpdateModelTransform();
};

typedef char daObjFm_Battan_c_size_must_be_0x37c[
    sizeof(daObjFm_Battan_c) == 0x37c ? 1 : -1];

#endif /* DAOBJFM_BATTAN_C_H */
