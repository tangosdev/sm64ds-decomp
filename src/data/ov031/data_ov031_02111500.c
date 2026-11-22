/* claimed-storage */
/* ov031 .data 0x02111500..0x02111a00. 46 storage addresses, one object. */
#include "types.h"

extern void _Z36daObjHsBillboard_c_classInit_HS_MOONv(void);
extern void _Z36daObjHsBillboard_c_classInit_HS_STARv(void);
extern void _Z38daObjHsBillboard_c_classInit_HS_Y_STARv(void);
extern void _Z38daObjHsBillboard_c_classInit_HS_B_STARv(void);
extern void _ZN18daObjHsBillboard_c13InitResourcesEv(void);
extern void _ZN8dActor_c19BeforeInitResourcesEv(void);
extern void _ZN8dActor_c18AfterInitResourcesEj(void);
extern void _ZN18daObjHsBillboard_c16CleanupResourcesEv(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN7fBase_c8BehaviorEv(void);
extern void _ZN8dActor_c14BeforeBehaviorEv(void);
extern void _ZN8dActor_c13AfterBehaviorEj(void);
extern void _ZN18daObjHsBillboard_c6RenderEv(void);
extern void _ZN8dActor_c12BeforeRenderEv(void);
extern void _ZN8dActor_c11AfterRenderEj(void);
extern void _ZN7fBase_c16OnPendingDestroyEv(void);
extern void _ZN7fBase_c9Virtual34Ejj(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);
extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN18daObjHsBillboard_cD1Ev(void);
extern void _ZN18daObjHsBillboard_cD0Ev(void);
extern void _ZN8dActor_c13OnYoshiTryEatEv(void);
extern void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void);
extern void _ZN8dActor_c9Virtual50Ev(void);
extern void _ZN8dActor_c15OnGroundPoundedERS_(void);
extern void _ZN8dActor_c11OnAttacked1ERS_(void);
extern void _ZN8dActor_c11OnAttacked2ERS_(void);
extern void _ZN8dActor_c8OnKickedERS_(void);
extern void _ZN8dActor_c8OnPushedERS_(void);
extern void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void);
extern void _ZN8dActor_c15OnHitByMegaCharER7daPly_c(void);
extern void _ZN8dActor_c19OnHitFromUnderneathERS_(void);
extern void _ZN8dActor_c16OnAimedAtWithEggEv(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);
extern u32 data_ov031_0211154c[];
extern u32 data_ov031_02111578[];
extern u32 data_ov031_02111508[];
extern u32 data_ov031_02111668[];
extern u32 data_ov031_02111758[];
extern u32 data_ov031_021115e8[];
extern u32 data_ov031_02111510[];
extern u32 data_ov031_02111520[];
extern u32 data_ov031_0211152c[];
extern u32 data_ov031_021115a8[];
extern u32 data_ov031_02111518[];
extern u32 data_ov031_02111504[];
extern u32 data_ov031_0211153c[];
extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS18daObjHsBillboard_c[];
extern u32 _ZTI8dActor_c[];
extern u32 _ZTI18daObjHsBillboard_c[];

#pragma explicit_zero_data on
u32 data_ov031_02111500[] = {
    0x00000756, 0x11111114, 0x00000002,
    (u32)&data_ov031_0211154c,
    0x00000006,
    (u32)&data_ov031_02111578,
    0x00000001, 0x75306d60,
    (u32)&data_ov031_02111508,
    0x00000000, 0x35480000, 0x2d67ff02, 0x31f532e3, 0xa0000000, 0x00000000, 0xcaf2f9fb, 0xc000d2d8,
    0x04160000, 0x000011ff, 0x00000f00,
    (u32)&data_ov031_02111668,
    0x00003205,
    (u32)&data_ov031_02111758,
    (u32)&data_ov031_021115e8,
    (u32)&data_ov031_02111510,
    0x07550757, 0x07590758,
    (u32)&data_ov031_02111520,
    0x0af00001, 0x0000000f, 0x00000104,
    (u32)&data_ov031_0211152c,
    0x0000010b,
    (u32)&data_ov031_02111500,
    0x00000401,
    (u32)&data_ov031_021115a8,
    0x00000108,
    (u32)&data_ov031_02111518,
    0x0000010e,
    (u32)&data_ov031_02111504,
    0x0000010a,
    (u32)&data_ov031_0211153c,
    0x2c6d0000, 0x30fb3473, 0x9c720000, 0x02000000, 0x2c090000, 0x31c3322d, 0x9c720000, 0x00000000,
    0x2d670000, 0x30c9321b, 0x9c720000, 0x00000000, 0x2d030000, 0x3191321b, 0x9c720000, 0x00000000,
    0x53504c43, 0x000e0008, 0x00003fc4, 0x000000ff, 0x0005bfc4, 0x000000ff, 0x01000fc0, 0x000000ff,
    0x04000fc0, 0x000000ff, 0x00200fc0, 0x000000ff, 0x01040fc4, 0x000000ff, 0x00043fc0, 0x000000ff,
    0x00040fc0, 0x000000ff, 0x00043fc4, 0x000000ff, 0x01008fc4, 0x000000ff, 0x01008fc0, 0x000000ff,
    0x0000dfc0, 0x000000ff, 0x0000dfc5, 0x000000ff, 0x0000dfc4, 0x000000ff, 0xffffffff, 0xffffffff,
    0x2b730031, 0x2e3f322d, 0x00000000, 0x00f30000, 0x19170031, 0x1da52b77, 0xa0000000, 0x00f20000,
    0xcfda0031, 0xf6960ad9, 0x80000000, 0x00f20000, 0x109f00c0, 0x07f723b7, 0x00000000, 0xffff0000,
    0x0b4d00c0, 0x114df19a, 0x00000000, 0xffff0000, 0xc81300c0, 0xcd6708b0, 0x00000000, 0xffff0000,
    0x082500c1, 0x159b2526, 0x00000000, 0xffff0000, 0xbfde00c1, 0x096517fe, 0x00000000, 0xffff0000,
    0x405c00c1, 0xeabae082, 0x00000000, 0xffff0000, 0x0d1700c2, 0x20122973, 0x00000000, 0xffff0000,
    0xe71c00c2, 0x20c9f039, 0x00000000, 0xffff0000, 0x297900c2, 0xc2a1ddbe, 0x00000000, 0xffff0000,
    0x229b00c3, 0x19f82dc0, 0x00000000, 0xffff0000, 0xdabc00c3, 0x13341cc4, 0x00000000, 0xffff0000,
    0x0d0200c3, 0xe25f01b9, 0x00000000, 0xffff0000, 0x29b1e225, 0x3001322d, 0x1f25e225, 0x23b32e65,
    0x1e8fe225, 0x231d2e65, 0x1df9e225, 0x22872e33, 0x1d63e225, 0x21f12de4, 0x1ccde225, 0x215b2d46,
    0x12d7e225, 0x17972819, 0x1241e225, 0x170127d8, 0x11abe225, 0x166b273b, 0x1147e225, 0x16072620,
    0x10e3e225, 0x15a32505, 0x06ede225, 0x0b7b1f8d, 0xe4c09a27, 0x0b49180d, 0xd4eee225, 0x08bf12a9,
    0xd232e225, 0x06031119, 0xd070e225, 0x01e90ec1, 0xcfa81a27, 0xfff60e0d, 0xcfda1a27, 0xee6208a5,
    0xdcbee225, 0xd65c02cb, 0xdf7ae225, 0xd65c024c, 0xe236e225, 0xd7560241, 0xe3f8e225, 0xd8e60197,
    0xe556e225, 0xda7600ed, 0xe61ee225, 0xe14cfe87, 0xe5ece225, 0xea7afa72, 0xe524e225, 0xf470f569,
    0xe524e225, 0xf506f52c, 0xe524e225, 0xf59cf4ef, 0xe524e225, 0xf632f4b2, 0xe524e225, 0xf6c8f0b0,
    0xe6b4e225, 0x0379eec8, 0xe844e225, 0x0667ee62, 0xea6ae225, 0x088dedfe, 0xf87ae225, 0x0a4feba9,
    0xfad2e225, 0x08bfeadf, 0xfbcce225, 0x0667ea33, 0xfc94e225, 0xfd08e953, 0x0f21e225, 0xeef8e2d8,
    0x1a43e225, 0xef2adf08, 0x2371e225, 0xefc0dcf4, 0x2d03e225, 0xedfedb85, 0x2f5be225, 0xeb74dada,
    0x2f5be225, 0xe1b0d93e, 0x2e2fe225, 0xd882d7e0, 0x2a79e225, 0xd4ccd718, 0x1f57e225, 0xd274d566,
    0x1ec1e225, 0xd274d546, 0x1e2be225, 0xd274d525, 0x1d95e225, 0xd274d505, 0x1cffe225, 0xd274d4e4,
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS18daObjHsBillboard_c,
    (u32)&_ZTI8dActor_c,
    0x61643831, 0x486a624f, 0x6c694273, 0x616f626c, 0x635f6472, 0x00000000,
    (u32)&_Z36daObjHsBillboard_c_classInit_HS_MOONv,
    0x00bf012e, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0x00000000,
    (u32)&_Z36daObjHsBillboard_c_classInit_HS_STARv,
    0x00c0012f, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0x00000000,
    (u32)&_Z38daObjHsBillboard_c_classInit_HS_Y_STARv,
    0x00c10130, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0x00000000,
    (u32)&_Z38daObjHsBillboard_c_classInit_HS_B_STARv,
    0x00c20131, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&_ZTI18daObjHsBillboard_c,
    (u32)&_ZN18daObjHsBillboard_c13InitResourcesEv,
    (u32)&_ZN8dActor_c19BeforeInitResourcesEv,
    (u32)&_ZN8dActor_c18AfterInitResourcesEj,
    (u32)&_ZN18daObjHsBillboard_c16CleanupResourcesEv,
    (u32)&_ZN8dActor_c22BeforeCleanupResourcesEv,
    (u32)&_ZN8dActor_c21AfterCleanupResourcesEj,
    (u32)&_ZN7fBase_c8BehaviorEv,
    (u32)&_ZN8dActor_c14BeforeBehaviorEv,
    (u32)&_ZN8dActor_c13AfterBehaviorEj,
    (u32)&_ZN18daObjHsBillboard_c6RenderEv,
    (u32)&_ZN8dActor_c12BeforeRenderEv,
    (u32)&_ZN8dActor_c11AfterRenderEj,
    (u32)&_ZN7fBase_c16OnPendingDestroyEv,
    (u32)&_ZN7fBase_c9Virtual34Ejj,
    (u32)&_ZN7fBase_c9Virtual38Ejj,
    (u32)&_ZN7fBase_c13OnHeapCreatedEv,
    (u32)&_ZN18daObjHsBillboard_cD1Ev,
    (u32)&_ZN18daObjHsBillboard_cD0Ev,
    (u32)&_ZN8dActor_c13OnYoshiTryEatEv,
    (u32)&_ZN8dActor_c13OnTurnIntoEggER7daPly_c,
    (u32)&_ZN8dActor_c9Virtual50Ev,
    (u32)&_ZN8dActor_c15OnGroundPoundedERS_,
    (u32)&_ZN8dActor_c11OnAttacked1ERS_,
    (u32)&_ZN8dActor_c11OnAttacked2ERS_,
    (u32)&_ZN8dActor_c8OnKickedERS_,
    (u32)&_ZN8dActor_c8OnPushedERS_,
    (u32)&_ZN8dActor_c24OnHitByCannonBlastedCharERS_,
    (u32)&_ZN8dActor_c15OnHitByMegaCharER7daPly_c,
    (u32)&_ZN8dActor_c19OnHitFromUnderneathERS_,
    (u32)&_ZN8dActor_c16OnAimedAtWithEggEv,
    (u32)&_ZN8dActor_c25OnAimedAtWithEggReturnVecEv,
};
#pragma explicit_zero_data off
