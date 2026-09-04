//cpp
// @symbol daWanwan2_c_classInit
#include "UnchainedChomp.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN12dEnemyBase_cC2Ev(void *self);
void _ZN10dCcAcPos_cC1Ev(void *self);
void _ZN10dBgCh_ActrC1Ev(void *self);
void _ZN9ModelAnimC1Ev(void *self);
void _ZN11ShadowModelC1Ev(void *self);
void func_020733a8(void *array, int count, int stride, void *ctor, void *dtor);
extern void *_ZTV14UnchainedChomp;
void _ZN5ModelC1Ev();
void _ZN5ModelD1Ev();
void _ZN11ShadowModelD1Ev();
void _ZN7Vector3D1Ev();
void func_0203d384();
void _ZN8Vector3sD1Ev();
void func_0203d73c();
}

/* Reconstructed source-style name: SM64DS proves daWanwan2_c through RTTI,
 * allocation size, vtable identity, and the WANWAN2 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: UnchainedChomp_Spawn. */
extern "C" UnchainedChomp *daWanwan2_c_classInit()
{
    UnchainedChomp *actor =
        (UnchainedChomp *)_ZN7fBase_cnwEj(sizeof(UnchainedChomp));
    if (actor) {
        _ZN12dEnemyBase_cC2Ev(actor);
        *(void **)actor = &_ZTV14UnchainedChomp;
        _ZN10dCcAcPos_cC1Ev(&actor->mdCcAcPos_c);
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
        func_020733a8(actor->mModels, 6, sizeof(Model),
            (void *)_ZN5ModelC1Ev, (void *)_ZN5ModelD1Ev);
        func_020733a8(actor->mShadowModels, 6, sizeof(ShadowModel),
            (void *)_ZN11ShadowModelC1Ev, (void *)_ZN11ShadowModelD1Ev);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel);
        func_020733a8(actor->mUnk_6d8, 6, sizeof(Vector3),
            (void *)func_0203d384, (void *)_ZN7Vector3D1Ev);
        func_020733a8(actor->mUnk_720, 6, sizeof(Vector3),
            (void *)func_0203d384, (void *)_ZN7Vector3D1Ev);
        func_020733a8(actor->mUnk_768, 6, sizeof(Vector3s),
            (void *)func_0203d73c, (void *)_ZN8Vector3sD1Ev);
    }
    return actor;
}
