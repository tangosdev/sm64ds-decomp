//cpp
// @symbol daPiano_c_classInit
#include "MadPiano.h"

/* The pinned compiler has no usable source spelling for the class-specific
 * operator new or placement new (notes/ctor-migration.md, factory wall), so
 * this factory keeps only that ABI boundary explicit. The subobjects and
 * construction order are expressed through the real class layout. */
extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN10dBgActor_cC2Ev(void *self);
void _ZN9ModelAnimC1Ev(void *self);
void _ZN11ShadowModelC1Ev(void *self);
void func_020733a8(void *array, int count, int stride, void *ctor, void *dtor);
void _ZN10dCcAcPos_cC1Ev(void *self);
void _ZN10dCcAcPos_cD1Ev(void *self);
void _ZN10dBgCh_ActrC1Ev(void *self);
extern void *_ZTV8MadPiano;
}

/* Reconstructed source-style name: SM64DS proves daPiano_c through RTTI,
 * allocation size, vtable identity, and the PIANO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MadPiano_Spawn. */
extern "C" MadPiano *daPiano_c_classInit()
{
    MadPiano *actor = (MadPiano *)_ZN7fBase_cnwEj(sizeof(MadPiano));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *(void **)actor = &_ZTV8MadPiano;
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel1);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel2);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel3);
        func_020733a8(actor->mCylinderClsn, 2, sizeof(dCcAcPos_c),
            (void *)_ZN10dCcAcPos_cC1Ev, (void *)_ZN10dCcAcPos_cD1Ev);
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
    }
    return actor;
}
