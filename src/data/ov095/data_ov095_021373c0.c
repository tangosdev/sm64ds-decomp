/* claimed-storage */
/* ov095 .data 0x021373c0..0x0213757c. 15 storage addresses, one object. */
#include "types.h"

extern void _Z30daObjSeesaw_c_classInit_SEESAWv(void);
extern void _Z34daObjSeesaw_c_classInit_KM1_SEESAWv(void);
extern void _Z38daObjSeesaw_c_classInit_KM2_YOKOSEESAWv(void);
extern void _Z34daObjSeesaw_c_classInit_KM3_SEESAWv(void);
extern void _Z38daObjSeesaw_c_classInit_KM3_YOKOSEESAWv(void);
extern void _Z33daObjSeesaw_c_classInit_RC_SEESAWv(void);
extern void _Z35daObjSeesaw_c_classInit_BOMB_SEESAWv(void);
extern void _ZN13daObjSeesaw_c13InitResourcesEv(void);
extern void _ZN8dActor_c19BeforeInitResourcesEv(void);
extern void _ZN8dActor_c18AfterInitResourcesEj(void);
extern void _ZN13daObjSeesaw_c16CleanupResourcesEv(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN13daObjSeesaw_c8BehaviorEv(void);
extern void _ZN8dActor_c14BeforeBehaviorEv(void);
extern void _ZN8dActor_c13AfterBehaviorEj(void);
extern void _ZN13daObjSeesaw_c6RenderEv(void);
extern void _ZN8dActor_c12BeforeRenderEv(void);
extern void _ZN8dActor_c11AfterRenderEj(void);
extern void _ZN7fBase_c16OnPendingDestroyEv(void);
extern void _ZN7fBase_c9Virtual34Ejj(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);
extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN13daObjSeesaw_cD1Ev(void);
extern void _ZN13daObjSeesaw_cD0Ev(void);
extern void _ZN8dActor_c13OnYoshiTryEatEv(void);
extern void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void);
extern void _ZN8dActor_c9Virtual50Ev(void);
extern void _ZN13daObjSeesaw_c15OnGroundPoundedER8dActor_c(void);
extern void _ZN8dActor_c11OnAttacked1ERS_(void);
extern void _ZN8dActor_c11OnAttacked2ERS_(void);
extern void _ZN8dActor_c8OnKickedERS_(void);
extern void _ZN8dActor_c8OnPushedERS_(void);
extern void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void);
extern void _ZN8dActor_c15OnHitByMegaCharER7daPly_c(void);
extern void _ZN8dActor_c19OnHitFromUnderneathERS_(void);
extern void _ZN8dActor_c16OnAimedAtWithEggEv(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);
extern void _ZN10dBgActor_c4KillEv(void);
extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS13daObjSeesaw_c[];
extern u32 _ZTI10dBgActor_c[];
extern u32 gObjSeesawModelFile[];
extern u32 gObjSeesawCollisionFile[];
extern u32 data_ov002_0210d974[];
extern u32 gBSiSoModelFile[];
extern u32 gBSiSoCollisionFile[];
extern u32 data_ov014_02114598[];
extern u32 gKm1ShisoModelFile[];
extern u32 gKm1ShisoCollisionFile[];
extern u32 data_ov012_02111c20[];
extern u32 gKm2YokoshisoModelFile[];
extern u32 gKm2YokoshisoCollisionFile[];
extern u32 data_ov010_02112570[];
extern u32 gKm3ShisoModelFile[];
extern u32 gKm3ShisoCollisionFile[];
extern u32 data_ov008_02111ad4[];
extern u32 gKm3YokoshisoModelFile[];
extern u32 gKm3YokoshisoCollisionFile[];
extern u32 data_ov008_02111b14[];
extern u32 gRcShisoModelFile[];
extern u32 gRcShisoCollisionFile[];
extern u32 data_ov009_02112ba8[];

#pragma explicit_zero_data on
u32 _ZTI13daObjSeesaw_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS13daObjSeesaw_c,
    (u32)&_ZTI10dBgActor_c,
    0x61643331, 0x536a624f, 0x61736565, 0x00635f77,
    (u32)&_Z30daObjSeesaw_c_classInit_SEESAWv,
    0x0128001c, 0x00000002, 0x00000000, 0x00500000, 0x01850000, 0x00000000,
    (u32)&_Z34daObjSeesaw_c_classInit_KM1_SEESAWv,
    0x00ae0085, 0x00000002, 0x00000000, 0x00400000, 0x01000000, 0x00000000,
    (u32)&_Z38daObjSeesaw_c_classInit_KM2_YOKOSEESAWv,
    0x00d6008f, 0x00000002, 0x00000000, 0x00400000, 0x01000000, 0x00000000,
    (u32)&_Z34daObjSeesaw_c_classInit_KM3_SEESAWv,
    0x00dc0095, 0x00000002, 0x00000000, 0x00400000, 0x01000000, 0x00000000,
    (u32)&_Z38daObjSeesaw_c_classInit_KM3_YOKOSEESAWv,
    0x00dd0096, 0x00000002, 0x00000000, 0x00800000, 0x02000000, 0x00000000,
    (u32)&_Z33daObjSeesaw_c_classInit_RC_SEESAWv,
    0x01020080, 0x00000002, 0x00000000, 0x00400000, 0x01000000, 0x00000000,
    (u32)&_Z35daObjSeesaw_c_classInit_BOMB_SEESAWv,
    0x00ad0027, 0x00000002, 0x00000000, 0x00400000, 0x01000000, 0x00000000,
    (u32)&gObjSeesawModelFile,
    (u32)&gObjSeesawCollisionFile,
    (u32)&data_ov002_0210d974,
    (u32)&gBSiSoModelFile,
    (u32)&gBSiSoCollisionFile,
    (u32)&data_ov014_02114598,
    (u32)&gKm1ShisoModelFile,
    (u32)&gKm1ShisoCollisionFile,
    (u32)&data_ov012_02111c20,
    (u32)&gKm2YokoshisoModelFile,
    (u32)&gKm2YokoshisoCollisionFile,
    (u32)&data_ov010_02112570,
    (u32)&gKm3ShisoModelFile,
    (u32)&gKm3ShisoCollisionFile,
    (u32)&data_ov008_02111ad4,
    (u32)&gKm3YokoshisoModelFile,
    (u32)&gKm3YokoshisoCollisionFile,
    (u32)&data_ov008_02111b14,
    (u32)&gRcShisoModelFile,
    (u32)&gRcShisoCollisionFile,
    (u32)&data_ov009_02112ba8,
    0x00000000,
    (u32)&_ZTI13daObjSeesaw_c,
    (u32)&_ZN13daObjSeesaw_c13InitResourcesEv,
    (u32)&_ZN8dActor_c19BeforeInitResourcesEv,
    (u32)&_ZN8dActor_c18AfterInitResourcesEj,
    (u32)&_ZN13daObjSeesaw_c16CleanupResourcesEv,
    (u32)&_ZN8dActor_c22BeforeCleanupResourcesEv,
    (u32)&_ZN8dActor_c21AfterCleanupResourcesEj,
    (u32)&_ZN13daObjSeesaw_c8BehaviorEv,
    (u32)&_ZN8dActor_c14BeforeBehaviorEv,
    (u32)&_ZN8dActor_c13AfterBehaviorEj,
    (u32)&_ZN13daObjSeesaw_c6RenderEv,
    (u32)&_ZN8dActor_c12BeforeRenderEv,
    (u32)&_ZN8dActor_c11AfterRenderEj,
    (u32)&_ZN7fBase_c16OnPendingDestroyEv,
    (u32)&_ZN7fBase_c9Virtual34Ejj,
    (u32)&_ZN7fBase_c9Virtual38Ejj,
    (u32)&_ZN7fBase_c13OnHeapCreatedEv,
    (u32)&_ZN13daObjSeesaw_cD1Ev,
    (u32)&_ZN13daObjSeesaw_cD0Ev,
    (u32)&_ZN8dActor_c13OnYoshiTryEatEv,
    (u32)&_ZN8dActor_c13OnTurnIntoEggER7daPly_c,
    (u32)&_ZN8dActor_c9Virtual50Ev,
    (u32)&_ZN13daObjSeesaw_c15OnGroundPoundedER8dActor_c,
    (u32)&_ZN8dActor_c11OnAttacked1ERS_,
    (u32)&_ZN8dActor_c11OnAttacked2ERS_,
    (u32)&_ZN8dActor_c8OnKickedERS_,
    (u32)&_ZN8dActor_c8OnPushedERS_,
    (u32)&_ZN8dActor_c24OnHitByCannonBlastedCharERS_,
    (u32)&_ZN8dActor_c15OnHitByMegaCharER7daPly_c,
    (u32)&_ZN8dActor_c19OnHitFromUnderneathERS_,
    (u32)&_ZN8dActor_c16OnAimedAtWithEggEv,
    (u32)&_ZN8dActor_c25OnAimedAtWithEggReturnVecEv,
    (u32)&_ZN10dBgActor_c4KillEv,
};
#pragma explicit_zero_data off
