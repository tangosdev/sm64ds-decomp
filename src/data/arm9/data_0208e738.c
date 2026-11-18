/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e738..0x0208e7c4, 140 bytes, 12 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u32 _ZTI11dExtModel_c[2];
extern u32 _ZTI15dExtFrameCtrl_c[2];
extern u32 _ZTI15dExtAnmTexSRT_c[3];
extern u32 _ZTS11dExtModel_c[6];
extern u8 _ZTVN3abi17__class_type_infoE[];
extern u32 _ZTS15dExtFrameCtrl_c[];
extern u8 _ZTVN3abi20__si_class_type_infoE[];
extern char _ZTS15dExtAnmTexSRT_c[];
extern char _ZTS15dExtAnmTexPat_c[];
extern char _ZTS17dExtAnmMaterial_c[];
extern u32 _ZTS17dExtShadowModel_c[];
extern u32 _ZTS19dExtBlendAnmModel_c[];
extern u32 _ZTI14dExtAnmModel_c[];
extern char _ZTS15dExtAnm2Model_c[];
extern char _ZTS17dExtSimpleModel_c[];
extern char _ZTS17dExtCommonModel_c[];

/* 0x0208e758: 12 bytes. */
u32 _ZTI15dExtAnmTexPat_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS15dExtAnmTexPat_c,
    (u32)_ZTI15dExtFrameCtrl_c,
};

/* 0x0208e794: 12 bytes. */
u32 _ZTI17dExtSimpleModel_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS17dExtSimpleModel_c,
    (u32)_ZTI11dExtModel_c,
};

/* 0x0208e788: 12 bytes. */
u32 _ZTI15dExtAnm2Model_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS15dExtAnm2Model_c,
    (u32)_ZTI14dExtAnmModel_c,
};

/* 0x0208e77c: 12 bytes. */
u32 _ZTI19dExtBlendAnmModel_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS19dExtBlendAnmModel_c,
    (u32)_ZTI14dExtAnmModel_c,
};

/* 0x0208e7a0: 12 bytes. */
u32 _ZTI17dExtCommonModel_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS17dExtCommonModel_c,
    (u32)_ZTI11dExtModel_c,
};

/* 0x0208e764: 12 bytes. */
u32 _ZTI17dExtAnmMaterial_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS17dExtAnmMaterial_c,
    (u32)_ZTI15dExtFrameCtrl_c,
};

/* 0x0208e74c: 12 bytes. */
u32 _ZTI15dExtAnmTexSRT_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS15dExtAnmTexSRT_c,
    (u32)_ZTI15dExtFrameCtrl_c,
};

/* 0x0208e770: 12 bytes. */
u32 _ZTI17dExtShadowModel_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS17dExtShadowModel_c,
    (u32)_ZTI11dExtModel_c,
};

/* 0x0208e73c: 8 bytes. */
u32 _ZTI11dExtModel_c[2] = {
    (u32)_ZTVN3abi17__class_type_infoE,
    (u32)_ZTS11dExtModel_c,
};

/* 0x0208e744: 8 bytes. */
u32 _ZTI15dExtFrameCtrl_c[2] = {
    (u32)_ZTVN3abi17__class_type_infoE,
    (u32)_ZTS15dExtFrameCtrl_c,
};

/* 0x0208e738: 4 bytes. */
int data_0208e738 = 0x00000001;

/* 0x0208e7ac: 24 bytes. */
u32 _ZTS11dExtModel_c[6] = {
    0x45643131, 0x6f4d7478, 0x5f6c6564, 0x00000063, 0x00000000, (u32)_ZTI15dExtAnmTexSRT_c,
};
