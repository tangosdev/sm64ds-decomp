/* Camera script-op and state member-function constants (dCamera_c).
 * The 8-byte records data_02086bc0..data_02086e40 are the pointer-to-member
 * constants RunCameraCommand and the state table index; every adjust word is 0.
 * The strings are the _ZTS names and the _ZTI records of the camera classes,
 * and 0x02086e84..0x02086f7c holds the look-at/position offset triples.
 * Consumer: src/engine/view/d_camera.cpp, __sinit_d_camera.cpp.
 * ROM 0x02086d00..0x02086e40, 320 bytes, 40 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__class_type_info 0x0209a774); it stays a number because the tree
 * declares no _ZTV array.
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

extern int _Z13func_0200814cP9dCamera_c();
extern int _Z26CutsceneGrandStarMarioJumpP9dCamera_c();
extern int func_020098b8();
extern int _Z13func_020082f8P9dCamera_cPhii();
extern int _Z29CutsceneGrandStarFrontOfMarioP9dCamera_c();
extern int _ZN9dCamera_c13func_0200af0cEv();
extern int _ZN9dCamera_c13func_02009e70Ev();
extern int _Z22CutsceneGrandStarStartP9dCamera_c();
extern int _Z24CutsceneKeyDanceJumpCvarP9dCamera_cPh();
extern int func_02009dd0();
extern int _Z18RotateAndMoveVec3fP9dCamera_cPh();
extern int _Z25CutsceneDanceCloseupStartP9dCamera_c();
extern char _ZTS7fBase_c[];
extern struct dCamera_c *dCamera_c_classInit();
extern int _Z13func_020084b0P9dCamera_cPhii();
extern int _Z13func_020078c4P9dCamera_cPh();
extern int _Z13func_020082a0P9dCamera_cPhi();
extern int _Z16CutscenePanCvar9P9dCamera_cPhi();
extern int _ZN9dCamera_c13func_02009e50Ev();
extern int func_020095e4();
extern int func_02009d90();
extern int _Z19FocusInFrontOfMarioP9dCamera_cPh();
extern void _Z13func_02005000P9dCamera_c();
extern int _Z13func_02005060P9dCamera_c();
extern int _Z13func_02007b98P9dCamera_cPh();
extern void func_020094c0();
extern int func_020094e4();
extern int _Z13func_02007e5cP9dCamera_cPh();
extern int _ZN9dCamera_c13func_020098e0Ev();
extern int _ZN9dCamera_c13func_02009a8cEv();
extern int _Z13func_02007f14P9dCamera_cPh();
extern int func_02009540();
extern int func_020095c4();
extern int _Z13func_02008008P9dCamera_cPh();
extern int _Z13func_02005098P9dCamera_c();
extern int _Z13func_020080b0P9dCamera_c();
extern int _Z13func_02008028P9dCamera_c();

/* 0x02086e30: 8 bytes. */
CamScriptOp data_02086e30 = {(void *)&_Z13func_020080b0P9dCamera_c, 0};

/* 0x02086e28: 8 bytes. */
CamPmf data_02086e28 = {(void *)&_Z13func_02005098P9dCamera_c, 0};

/* 0x02086e20: 8 bytes. */
CamScriptOp data_02086e20 = {(void *)&_Z13func_02008008P9dCamera_cPh, 0};

/* 0x02086e18: 8 bytes. */
CamPmf data_02086e18 = {(void *)&func_020095c4, 0};

/* 0x02086e10: 8 bytes. */
CamPmf data_02086e10 = {(void *)&func_02009540, 0};

/* 0x02086e08: 8 bytes. */
CamScriptOp data_02086e08 = {(void *)&_Z13func_02007f14P9dCamera_cPh, 0};

/* 0x02086e00: 8 bytes. */
CamPmf data_02086e00 = {(void *)&_ZN9dCamera_c13func_02009a8cEv, 0};

/* 0x02086df8: 8 bytes. */
CamPmf data_02086df8 = {(void *)&_ZN9dCamera_c13func_020098e0Ev, 0};

/* 0x02086df0: 8 bytes. */
CamScriptOp data_02086df0 = {(void *)&_Z13func_02007e5cP9dCamera_cPh, 0};

/* 0x02086de8: 8 bytes. */
CamPmf data_02086de8 = {(void *)&func_020094e4, 0};

/* 0x02086de0: 8 bytes. */
CamPmf data_02086de0 = {(void *)&func_020094c0, 0};

/* 0x02086dd8: 8 bytes. */
CamScriptOp data_02086dd8 = {(void *)&_Z13func_02007b98P9dCamera_cPh, 0};

/* 0x02086dd0: 8 bytes. */
CamPmf data_02086dd0 = {(void *)&_Z13func_02005060P9dCamera_c, 0};

/* 0x02086dc8: 8 bytes. */
CamPmf data_02086dc8 = {(void *)&_Z13func_02005000P9dCamera_c, 0};

/* 0x02086dc0: 8 bytes. */
CamScriptOp data_02086dc0 = {(void *)&_Z19FocusInFrontOfMarioP9dCamera_cPh, 0};

/* 0x02086db8: 8 bytes. */
CamPmf data_02086db8 = {(void *)&func_02009d90, 0};

/* 0x02086db0: 8 bytes. */
CamPmf data_02086db0 = {(void *)&func_020095e4, 0};

/* 0x02086da8: 8 bytes. */
CamPmf data_02086da8 = {(void *)&_ZN9dCamera_c13func_02009e50Ev, 0};

/* 0x02086da0: 8 bytes. */
CamScriptOp data_02086da0 = {(void *)&_Z16CutscenePanCvar9P9dCamera_cPhi, 0};

/* 0x02086d98: 8 bytes. */
CamScriptOp data_02086d98 = {(void *)&_Z13func_020082a0P9dCamera_cPhi, 0};

/* 0x02086d90: 8 bytes. */
CamPmf data_02086d90 = {(void *)&_ZN9dCamera_c13func_02009e70Ev, 0};

/* 0x02086d88: 8 bytes. */
CamScriptOp data_02086d88 = {(void *)&_Z13func_020078c4P9dCamera_cPh, 0};

/* 0x02086d80: 8 bytes. */
CamScriptOp data_02086d80 = {(void *)&_Z13func_020084b0P9dCamera_cPhii, 0};

/* 0x02086d78: 8 bytes. */
u32 g_profile_CAMERA[2] = {
    (u32)&dCamera_c_classInit,
    0x0000014c,
};

/* 0x02086d70: 8 bytes. */
u32 _ZTI7fBase_c[2] = {
    0x0209a774,
    (u32)_ZTS7fBase_c,
};

/* 0x02086d68: 8 bytes. */
CamScriptOp data_02086d68 = {(void *)&_Z25CutsceneDanceCloseupStartP9dCamera_c, 0};

/* 0x02086d60: 8 bytes. */
CamScriptOp data_02086d60 = {(void *)&_Z18RotateAndMoveVec3fP9dCamera_cPh, 0};

/* 0x02086d58: 8 bytes. */
CamPmf data_02086d58 = {(void *)&func_02009dd0, 0};

/* 0x02086d50: 8 bytes. */
int data_02086d50[2] = {
    0x005cb000,
    0x000007c2,
};

/* 0x02086d48: 8 bytes. */
CamPmf data_02086d48 = {(void *)&_ZN9dCamera_c13func_02009e70Ev, 0};

/* 0x02086d40: 8 bytes. */
CamScriptOp data_02086d40 = {(void *)&_Z24CutsceneKeyDanceJumpCvarP9dCamera_cPh, 0};

/* 0x02086d38: 8 bytes. */
CamScriptOp data_02086d38 = {(void *)&_Z22CutsceneGrandStarStartP9dCamera_c, 0};

/* 0x02086d30: 8 bytes. */
CamPmf data_02086d30 = {(void *)&_ZN9dCamera_c13func_02009e70Ev, 0};

/* 0x02086d28: 8 bytes. */
CamPmf data_02086d28 = {(void *)&_ZN9dCamera_c13func_0200af0cEv, 0};

/* 0x02086d20: 8 bytes. */
CamScriptOp data_02086d20 = {(void *)&_Z29CutsceneGrandStarFrontOfMarioP9dCamera_c, 0};

/* 0x02086d18: 8 bytes. */
CamScriptOp data_02086d18 = {(void *)&_Z13func_020082f8P9dCamera_cPhii, 0};

/* 0x02086d10: 8 bytes. */
CamPmf data_02086d10 = {(void *)&func_020098b8, 0};

/* 0x02086d08: 8 bytes. */
CamScriptOp data_02086d08 = {(void *)&_Z26CutsceneGrandStarMarioJumpP9dCamera_c, 0};

/* 0x02086d00: 8 bytes. */
CamScriptOp data_02086d00 = {(void *)&_Z13func_0200814cP9dCamera_c, 0};

/* 0x02086e38: 8 bytes. */
CamScriptOp data_02086e38 = {(void *)&_Z13func_02008028P9dCamera_c, 0};
