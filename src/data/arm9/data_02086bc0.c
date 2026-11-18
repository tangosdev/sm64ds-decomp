/* Camera script-op and state member-function constants (dCamera_c).
 * The 8-byte records data_02086bc0..data_02086e40 are the pointer-to-member
 * constants RunCameraCommand and the state table index; every adjust word is 0.
 * The strings are the _ZTS names and the _ZTI records of the camera classes,
 * and 0x02086e84..0x02086f7c holds the look-at/position offset triples.
 * Consumer: src/engine/view/d_camera.cpp, __sinit_d_camera.cpp.
 * ROM 0x02086bc0..0x02086d00, 320 bytes, 40 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

typedef struct {
    void *fn;
    int adjust;
} CamScriptOp;

/* The 8-byte member-function constants: int (CamPmfHost::*)(), the same 8-byte layout as CamScriptOp. */
typedef struct {
    void *fn;
    int adjust;
} CamPmf;

extern int _Z13func_020080f0P9dCamera_c();
extern int _ZN9dCamera_c13func_02009e70Ev();
extern int _Z13func_02005324Pv();
extern int _Z13func_0200840cP9dCamera_cPh();
extern int func_02009aa8();
extern int _ZN9dCamera_c13func_02009d30Ev();
extern void func_0200987c();
extern int func_0200985c();
extern int func_02009d70();
extern int func_020097ec();
extern int _Z13func_02007a8cP9dCamera_cPh();
extern int _Z30CutsceneGrandStarApproachMarioP9dCamera_c();
extern int func_02009e30();
extern int _Z27CutsceneGrandStarFocusMarioP9dCamera_c();
extern int _Z13func_02008474P9dCamera_cPhii();
extern int func_02009df0();
extern int func_02009db0();
extern int _Z28CutsceneDanceCloseupFlyAboveP9dCamera_c();
extern int _Z13func_02008500P9dCamera_cPh();
extern int _Z13func_02008200P9dCamera_cPhi();
extern int _Z13func_02008450P9dCamera_cPh();
extern int func_02009e10();
extern int _Z28CutsceneGrandStarFlyAppCvarsP9dCamera_c();
extern int _Z34CutsceneGrandStarFlyMarioOffscreenP9dCamera_c();
extern int _Z31CutsceneGrandStarFlyMoveToMarioP9dCamera_c();
extern int _Z25CutsceneDanceFlyAwayStartP9dCamera_c();
extern int _Z25CutsceneGrandStarFlyStartP9dCamera_c();
extern int _Z13func_02007fccP9dCamera_cPh();
extern int _Z26CutsceneGrandStarMoveCvar2P9dCamera_c();
extern int _Z13func_020050dcP9dCamera_c();
extern int _Z13func_02008080P9dCamera_c();
extern int _Z27CutsceneGrandStarAccelCvar2P9dCamera_c();
extern int _Z21CutsceneDanceShakeFovP9dCamera_cPh();

/* 0x02086cf0: 8 bytes. */
CamScriptOp data_02086cf0 = {(void *)&_Z27CutsceneGrandStarAccelCvar2P9dCamera_c, 0};

/* 0x02086ce8: 8 bytes. */
CamScriptOp data_02086ce8 = {(void *)&_Z13func_02008080P9dCamera_c, 0};

/* 0x02086ce0: 8 bytes. */
CamPmf data_02086ce0 = {(void *)&_Z13func_020050dcP9dCamera_c, 0};

/* 0x02086cd8: 8 bytes. */
CamScriptOp data_02086cd8 = {(void *)&_Z26CutsceneGrandStarMoveCvar2P9dCamera_c, 0};

/* 0x02086cd0: 8 bytes. */
CamScriptOp data_02086cd0 = {(void *)&_Z13func_02007fccP9dCamera_cPh, 0};

/* 0x02086cc8: 8 bytes. */
int data_02086cc8[2] = {
    0x00434000,
    0x00000640,
};

/* 0x02086cc0: 8 bytes. */
CamScriptOp data_02086cc0 = {(void *)&_Z25CutsceneGrandStarFlyStartP9dCamera_c, 0};

/* 0x02086cb8: 8 bytes. */
CamScriptOp data_02086cb8 = {(void *)&_Z25CutsceneDanceFlyAwayStartP9dCamera_c, 0};

/* 0x02086cb0: 8 bytes. */
CamScriptOp data_02086cb0 = {(void *)&_Z31CutsceneGrandStarFlyMoveToMarioP9dCamera_c, 0};

/* 0x02086ca8: 8 bytes. */
CamScriptOp data_02086ca8 = {(void *)&_Z34CutsceneGrandStarFlyMarioOffscreenP9dCamera_c, 0};

/* 0x02086ca0: 8 bytes. */
CamScriptOp data_02086ca0 = {(void *)&_Z28CutsceneGrandStarFlyAppCvarsP9dCamera_c, 0};

/* 0x02086c98: 8 bytes. */
CamPmf data_02086c98 = {(void *)&func_02009e10, 0};

/* 0x02086c90: 8 bytes. */
CamScriptOp data_02086c90 = {(void *)&_Z13func_02008450P9dCamera_cPh, 0};

/* 0x02086c88: 8 bytes. */
CamScriptOp data_02086c88 = {(void *)&_Z13func_02008200P9dCamera_cPhi, 0};

/* 0x02086c80: 8 bytes. */
CamScriptOp data_02086c80 = {(void *)&_Z13func_02008500P9dCamera_cPh, 0};

/* 0x02086c78: 8 bytes. */
CamScriptOp data_02086c78 = {(void *)&_Z28CutsceneDanceCloseupFlyAboveP9dCamera_c, 0};

/* 0x02086c70: 8 bytes. */
CamPmf data_02086c70 = {(void *)&func_02009db0, 0};

/* 0x02086c68: 8 bytes. */
CamPmf data_02086c68 = {(void *)&func_02009df0, 0};

/* 0x02086c60: 8 bytes. */
CamScriptOp data_02086c60 = {(void *)&_Z13func_02008474P9dCamera_cPhii, 0};

/* 0x02086c58: 8 bytes. */
CamPmf data_02086c58 = {(void *)&_ZN9dCamera_c13func_02009e70Ev, 0};

/* 0x02086c50: 8 bytes. */
CamScriptOp data_02086c50 = {(void *)&_Z27CutsceneGrandStarFocusMarioP9dCamera_c, 0};

/* 0x02086c48: 8 bytes. */
CamPmf data_02086c48 = {(void *)&func_02009e30, 0};

/* 0x02086c40: 8 bytes. */
CamPmf data_02086c40 = {(void *)&_ZN9dCamera_c13func_02009e70Ev, 0};

/* 0x02086c38: 8 bytes. */
CamScriptOp data_02086c38 = {(void *)&_Z30CutsceneGrandStarApproachMarioP9dCamera_c, 0};

/* 0x02086c30: 8 bytes. */
CamPmf data_02086c30 = {(void *)&_ZN9dCamera_c13func_02009e70Ev, 0};

/* 0x02086c28: 8 bytes. */
CamPmf data_02086c28 = {(void *)&_ZN9dCamera_c13func_02009e70Ev, 0};

/* 0x02086c20: 8 bytes. */
CamScriptOp data_02086c20 = {(void *)&_Z13func_02007a8cP9dCamera_cPh, 0};

/* 0x02086c18: 8 bytes. */
CamPmf data_02086c18 = {(void *)&func_020097ec, 0};

/* 0x02086c10: 8 bytes. */
CamPmf data_02086c10 = {(void *)&func_02009d70, 0};

/* 0x02086c08: 8 bytes. */
CamPmf data_02086c08 = {(void *)&func_0200985c, 0};

/* 0x02086c00: 8 bytes. */
CamPmf data_02086c00 = {(void *)&func_0200987c, 0};

/* 0x02086bf8: 8 bytes. */
CamPmf data_02086bf8 = {(void *)&_ZN9dCamera_c13func_02009e70Ev, 0};

/* 0x02086bf0: 8 bytes. */
CamPmf data_02086bf0 = {(void *)&_ZN9dCamera_c13func_02009e70Ev, 0};

/* 0x02086be8: 8 bytes. */
CamPmf data_02086be8 = {(void *)&_ZN9dCamera_c13func_02009d30Ev, 0};

/* 0x02086be0: 8 bytes. */
CamPmf data_02086be0 = {(void *)&func_02009aa8, 0};

/* 0x02086bd8: 8 bytes. */
CamScriptOp data_02086bd8 = {(void *)&_Z13func_0200840cP9dCamera_cPh, 0};

/* 0x02086bd0: 8 bytes. */
CamPmf data_02086bd0 = {(void *)&_Z13func_02005324Pv, 0};

/* 0x02086bc8: 8 bytes. */
CamPmf data_02086bc8 = {(void *)&_ZN9dCamera_c13func_02009e70Ev, 0};

/* 0x02086bc0: 8 bytes. */
CamScriptOp data_02086bc0 = {(void *)&_Z13func_020080f0P9dCamera_c, 0};

/* 0x02086cf8: 8 bytes. */
CamScriptOp data_02086cf8 = {(void *)&_Z21CutsceneDanceShakeFovP9dCamera_cPh, 0};
