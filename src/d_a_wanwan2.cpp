//cpp
// @symbol daWanwan2_c_classInit
#include "UnchainedChomp.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
dEnemyBase_c *_ZN12dEnemyBase_cC2Ev(dEnemyBase_c *object);
dCcAcPos_c *_ZN10dCcAcPos_cC1Ev(dCcAcPos_c *object);
dBgCh_Actr *_ZN10dBgCh_ActrC1Ev(dBgCh_Actr *object);
ModelAnim *_ZN9ModelAnimC1Ev(ModelAnim *object);
ShadowModel *_ZN11ShadowModelC1Ev(ShadowModel *object);
void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
    void (*ctor)(void *), void (*dtor)(void *));
extern void *_ZTV14UnchainedChomp;
Model *_ZN5ModelC1Ev(Model *object);
Model *_ZN5ModelD1Ev(Model *object);
ShadowModel *_ZN11ShadowModelD1Ev(ShadowModel *object);
Vector3 *_ZN7Vector3D1Ev(Vector3 *object);
void func_0203d384(void);
Vector3s *_ZN8Vector3sD1Ev(Vector3s *object);
void func_0203d73c(void);
}

/* Reconstructed source-style name: SM64DS proves daWanwan2_c through RTTI,
 * allocation size, vtable identity, and the WANWAN2 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: UnchainedChomp_Spawn. */
/* Array callbacks receive the element address and discard lifecycle results.
 * The empty func_0203d384/func_0203d73c callbacks ignore that address. */
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
        __cxa_vec_ctor(actor->mModels, 6, sizeof(Model),
            (void (*)(void *))_ZN5ModelC1Ev, (void (*)(void *))_ZN5ModelD1Ev);
        __cxa_vec_ctor(actor->mShadowModels, 6, sizeof(ShadowModel),
            (void (*)(void *))_ZN11ShadowModelC1Ev, (void (*)(void *))_ZN11ShadowModelD1Ev);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel);
        __cxa_vec_ctor(actor->mUnk_6d8, 6, sizeof(Vector3),
            (void (*)(void *))func_0203d384, (void (*)(void *))_ZN7Vector3D1Ev);
        __cxa_vec_ctor(actor->mUnk_720, 6, sizeof(Vector3),
            (void (*)(void *))func_0203d384, (void (*)(void *))_ZN7Vector3D1Ev);
        __cxa_vec_ctor(actor->mUnk_768, 6, sizeof(Vector3s),
            (void (*)(void *))func_0203d73c, (void (*)(void *))_ZN8Vector3sD1Ev);
    }
    return actor;
}
