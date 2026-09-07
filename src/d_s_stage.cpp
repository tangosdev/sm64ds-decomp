//cpp
// @symbol dScStage_c_classInit
#include "Stage.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN7fBase_cC2Ev(fBase_c *base);
void _ZN8Particle10SysTrackerC1Ev(Particle::SysTracker *tracker);
void _ZN5ModelC1Ev(Model *model);
void _ZN7dBgW_KcC1Ev(dBgW_Kc *meshCollider);
extern void *_ZTV7dBase_c[];
extern void *_ZTV8dScene_c[];
extern void *_ZTV5Stage[];

/* This is the allocation boundary for Stage, not a compiler-emitted C3
   constructor. Its base and member construction order remains explicit until
   the surrounding original translation unit can own a real Stage constructor. */
/* Reconstructed source-style name: SM64DS proves dScStage_c through RTTI,
 * allocation size, vtable identity, and the STAGE registry profile; later EAD
 * lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: Stage_Spawn. The project's Stage implementation type is
 * retained here because this wave reconstructs registry symbols, not classes. */
Stage *dScStage_c_classInit()
{
    Stage *stage = (Stage *)_ZN7fBase_cnwEj(sizeof(Stage));
    if (stage) {
        _ZN7fBase_cC2Ev(stage);
        *(void ***)stage = _ZTV7dBase_c;
        *(void ***)stage = _ZTV8dScene_c;
        stage->pauseFlags |= 1;
        stage->pauseFlags |= 4;
        *(void ***)stage = _ZTV5Stage;
        _ZN8Particle10SysTrackerC1Ev(&stage->mSysTracker);
        _ZN5ModelC1Ev(&stage->mModel);
        _ZN7dBgW_KcC1Ev(&stage->mMeshCollider);
    }
    return stage;
}
}
