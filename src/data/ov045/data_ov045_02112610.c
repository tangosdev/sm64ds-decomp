/* claimed-storage */
/* ov045 .data 0x02112610..0x02112d80. 35 storage addresses, one object. */
#include "types.h"

extern void _Z26daObjKm2_Agaru_c_classInitv(void);
extern void _ZN16daObjKm2_Agaru_c13InitResourcesEv(void);
extern void _ZN8dActor_c19BeforeInitResourcesEv(void);
extern void _ZN8dActor_c18AfterInitResourcesEj(void);
extern void _ZN16daObjKm2_Agaru_c16CleanupResourcesEv(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN16daObjKm2_Agaru_c8BehaviorEv(void);
extern void _ZN8dActor_c14BeforeBehaviorEv(void);
extern void _ZN8dActor_c13AfterBehaviorEj(void);
extern void _ZN16daObjKm2_Agaru_c6RenderEv(void);
extern void _ZN8dActor_c12BeforeRenderEv(void);
extern void _ZN8dActor_c11AfterRenderEj(void);
extern void _ZN7fBase_c16OnPendingDestroyEv(void);
extern void _ZN7fBase_c9Virtual34Ejj(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);
extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN16daObjKm2_Agaru_cD1Ev(void);
extern void _ZN16daObjKm2_Agaru_cD0Ev(void);
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
extern void _ZN10dBgActor_c4KillEv(void);
extern u32 data_ov045_02112694[];
extern u32 data_ov045_02112460[];
extern u32 data_ov045_02112630[];
extern u32 data_ov045_02112478[];
extern u32 data_ov045_021126d0[];
extern u32 data_ov045_02112468[];
extern u32 data_ov045_0211249c[];
extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS16daObjKm2_Agaru_c[];
extern u32 _ZTI10dBgActor_c[];
extern u32 _ZTI16daObjKm2_Agaru_c[];
extern u32 _ZTS18daObjKm2_Ami_Bou_c[];

#pragma explicit_zero_data on
u32 data_ov045_02112610[] = {
    0x53504c43, 0x00020008, 0x00000fc0, 0x000000ff, 0x01880fcc, 0x000000ff, 0xffffffff, 0xffffffff,
    0xe1ecff02, 0x02bcf830, 0x80000000, 0xff070000, 0xf0740000, 0x0000e318, 0x00000000, 0x1518ff02,
    0x038412c0, 0x60000000, 0x00000000, 0x00000a02,
    (u32)&data_ov045_02112694,
    0x00000103,
    (u32)&data_ov045_02112460,
    0x00000304,
    (u32)&data_ov045_02112630,
    0x0000040b,
    (u32)&data_ov045_02112478,
    0x00000501,
    (u32)&data_ov045_021126d0,
    0x0000040c,
    (u32)&data_ov045_02112468,
    0x0000010a,
    (u32)&data_ov045_0211249c,
    0xf448ea20, 0xed400000, 0x0000f448, 0xf3e4edd6, 0xf15a0000, 0x0000f3e4, 0xf448f1f0, 0xf3e40000,
    0x0000f448, 0xf31cf5d8, 0xf7cc0000, 0x0000f31c, 0xf448f9c0, 0xfda80000, 0x0000f448, 0xe2500000,
    0x0000f894, 0x40000000, 0x02000000, 0xe2500000, 0xfe70f574, 0x40000000, 0x00000000, 0xe2500000,
    0x00c8f574, 0x40000000, 0x00000000, 0xe2500000, 0x0190f574, 0x40000000, 0x00000000, 0x16440000,
    0x006413ec, 0x40000000, 0x02100000, 0x53504c43, 0x000a0008, 0x00048fc0, 0x000000ff, 0x000c8fc0,
    0x000000ff, 0x0004cfc0, 0x000000ff, 0x0004afc0, 0x000000ff, 0x011c8fc0, 0x000000ff, 0x01048fc0,
    0x000000ff, 0x01000fcc, 0x000000ff, 0x0104afcc, 0x000000ff, 0x018c8fcc, 0x000000ff, 0x0104cfc0,
    0x000000ff, 0xffffffff, 0xffffffff, 0xf510e225, 0xfd5df529, 0xf894e225, 0xfd5df529, 0x044ce225,
    0x0064f4c5, 0x044ce225, 0xffcef4c5, 0x044ce225, 0xff38f4c5, 0x044ce225, 0x00faf4c5, 0x044ce225,
    0x0190f4c5, 0x04b0e225, 0xf830f7cc, 0x03e8e225, 0xf894f7cc, 0x0384e225, 0xf95cf7cc, 0x03e8e225,
    0xfa24f7cc, 0x04b0e225, 0xfa88f7cc, 0x0578e225, 0xfa24f7cc, 0x05dce225, 0xf95cf7cc, 0x0578e225,
    0xf894f7cc, 0x04b02226, 0xf95cf8f8, 0x16122226, 0x0000f830, 0x109a2226, 0xff6afcae, 0xfda8e225,
    0xff6a0320, 0xfda8e225, 0x00000320, 0xfda8e225, 0x00960320, 0xfda8e225, 0x012c0320, 0xfda8e225,
    0x01c20320, 0xf8c6e225, 0x00640258, 0xf830e225, 0x00960258, 0xf7fee225, 0x012c0258, 0xf830e225,
    0x01c20258, 0xf8c6e225, 0x01f40258, 0xf95ce225, 0x01c20258, 0xf98ee225, 0x012c0258, 0xf95ce225,
    0x00960258, 0xf8f82226, 0xfe3e0640, 0xea52e225, 0xff9c02ee, 0xea52e225, 0xff9c0384, 0xea52e225,
    0xff9c041a, 0xea52e225, 0xff9c04b0, 0xea52e225, 0xff9c0546, 0xeffc2226, 0x00c80a28, 0xe69ce225,
    0xfce00c3b, 0xe732e225, 0xfce00c83, 0xe7c8e225, 0xfce00ccb, 0xe85ee225, 0xfce00d12, 0xe8f4e225,
    0xfce00d57, 0xf60ae225, 0xfed40dc5, 0xf60ae225, 0xff6a0dc5, 0xf60ae225, 0x00000dc5, 0xf60ae225,
    0x00960dc5, 0xf60ae225, 0x012c0dc5, 0xfaba2226, 0xfe700dc5, 0x0bb82226, 0x006411c6, 0x0af02226,
    0x012cfaba, 0x04b0423f, 0x0064157c, 0x1388e3ff, 0x00000064, 0xe82ce5ff, 0x000002bc, 0xe890e7ff,
    0x012c09c4, 0xebb0c43f, 0x02580ed8, 0x0af000d4, 0x012cfb1e, 0x00000000, 0xffff0000, 0x193100d6,
    0x0104f448, 0x00000000, 0xffff0000, 0x070800d6, 0x005d0dac, 0x00000000, 0xff010000, 0x025800d6,
    0x005d0dac, 0x00000000, 0xff010000, 0x0bb800d6, 0x005d0dac, 0x00000000, 0xff010000, 0xf89400d5,
    0xfd5df380, 0x00000000, 0xffff0000, 0xf51000d5, 0xfd5df380, 0x00000000, 0xffff0000, 0x110a00d3,
    0x005d1036, 0x00000000, 0xffff0000, 0x118700d3, 0x005d1036, 0x00000000, 0xffff0000, 0x120400d3,
    0x005d1036, 0x00000000, 0xffff0000, 0x128100d3, 0x005d1036, 0x00000000, 0xffff0000, 0x12fe00d3,
    0x005d1036, 0x00000000, 0xffff0000, 0x137b00d3, 0x005d1036, 0x00000000, 0xffff0000, 0x13f800d3,
    0x005d1036, 0x00000000, 0xffff0000, 0x147500d3, 0x005d1036, 0x00000000, 0xffff0000, 0x14f200d3,
    0x005d1036, 0x00000000, 0xffff0000, 0xe9bd00dc, 0x00c10190, 0x00000000, 0xffff0000, 0xf12800db,
    0x025a0064, 0x00008000, 0xffff0000, 0xf38100db, 0x00020064, 0x00000000, 0xffff0000, 0x080500da,
    0x013100c1, 0x00000000, 0xffff0000, 0xea2000d8, 0x0000f448, 0x00000000, 0x01000000, 0x044c00d9,
    0x012cf448, 0x00000000, 0xffff0000, 0xf3b200d9, 0x005d0d48, 0x00000000, 0xffff0000, 0xfabb00d9,
    0x005d0d48, 0x00000000, 0xffff0000, 0x10fd00d7, 0x0104f781, 0x00000000, 0xffff0000, 0x161100d7,
    0x0104f6eb, 0x00000000, 0xffff0000, 0xef980031, 0xfe3ef5d8, 0x00000000, 0x00f30000, 0xff6a0031,
    0xff9cf510, 0x00000000, 0x00f30000, 0x1b8a001f, 0x00faf8c6, 0x00000000, 0x03000000, 0xebb0001f,
    0x00640f0a, 0x00000000, 0x0a000000, 0x04b00031, 0x003214e6, 0x00000000, 0x00f30000, 0x0ed800f3,
    0x00faf830, 0x00000000, 0x014f0000, 0x0ed80031, 0x00fafb82, 0x00000000, 0x00f80000, 0x0ed80031,
    0x00fafb82, 0x00000000, 0x00170000, 0x099200f0, 0x00faf574, 0x00000000, 0xffff0000, 0x0d48000f,
    0xffcef574, 0x00000000, 0xffff0000, 0x0d48000f, 0x0258f574, 0x00000000, 0xffff0000, 0x0f6e000f,
    0x00faf574, 0x00000000, 0xffff0000, 0x19310099, 0x0104f574, 0x00000000, 0x00030000, 0xf9c000f0,
    0xfe3e0514, 0x00000000, 0xffff0000, 0xecdc00f0, 0x00000190, 0x00000000, 0xffff0000, 0xecdc00f0,
    0x02580190, 0x00000000, 0xffff0000, 0xebb00013, 0x012c0e0f, 0x00000000, 0x00050000, 0xf7cc0099,
    0x00000dd8, 0x00000000, 0x00010000, 0xe444001f, 0x00000546, 0x00000000, 0xff020000, 0x0000004f,
    0x00640e74, 0x00000000, 0xffff0000, 0x0e10004f, 0x0104f574, 0x00000000, 0xffff0000, 0xe47600a1,
    0xfce00c1c, 0x00000000, 0xffff0000, 0x04b0010a, 0xff0000fa, 0x00000000, 0xffff0000, 0xf3b2010a,
    0xfd6b0dac, 0x00000000, 0xffff0000, 0xfaba010a, 0xfd6f0dac, 0x00000000, 0xffff0000, 0x1c520014,
    0x0032f7cc, 0xeaab0003, 0xffff0000, 0x0960002c, 0x00c81405, 0x00000000, 0x3c020000, 0x0000008f,
    0xf830f448, 0x00000000, 0xffff0000, 0x0960001f, 0xff9c14ff, 0x00000000, 0xff020000,
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS16daObjKm2_Agaru_c,
    (u32)&_ZTI10dBgActor_c,
    0x61643631, 0x4b6a624f, 0x415f326d, 0x75726167, 0x0000635f,
    (u32)&_Z26daObjKm2_Agaru_c_classInitv,
    0x00d3008c, 0x00000002, 0x000fa000, 0x000fa000, 0x00fa0000, 0x00fa0000, 0x00000000,
    (u32)&_ZTI16daObjKm2_Agaru_c,
    (u32)&_ZN16daObjKm2_Agaru_c13InitResourcesEv,
    (u32)&_ZN8dActor_c19BeforeInitResourcesEv,
    (u32)&_ZN8dActor_c18AfterInitResourcesEj,
    (u32)&_ZN16daObjKm2_Agaru_c16CleanupResourcesEv,
    (u32)&_ZN8dActor_c22BeforeCleanupResourcesEv,
    (u32)&_ZN8dActor_c21AfterCleanupResourcesEj,
    (u32)&_ZN16daObjKm2_Agaru_c8BehaviorEv,
    (u32)&_ZN8dActor_c14BeforeBehaviorEv,
    (u32)&_ZN8dActor_c13AfterBehaviorEj,
    (u32)&_ZN16daObjKm2_Agaru_c6RenderEv,
    (u32)&_ZN8dActor_c12BeforeRenderEv,
    (u32)&_ZN8dActor_c11AfterRenderEj,
    (u32)&_ZN7fBase_c16OnPendingDestroyEv,
    (u32)&_ZN7fBase_c9Virtual34Ejj,
    (u32)&_ZN7fBase_c9Virtual38Ejj,
    (u32)&_ZN7fBase_c13OnHeapCreatedEv,
    (u32)&_ZN16daObjKm2_Agaru_cD1Ev,
    (u32)&_ZN16daObjKm2_Agaru_cD0Ev,
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
    (u32)&_ZN10dBgActor_c4KillEv,
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS18daObjKm2_Ami_Bou_c,
    (u32)&_ZTI10dBgActor_c,
};
#pragma explicit_zero_data off
