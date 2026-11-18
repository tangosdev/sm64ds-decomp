/* Camera script-op constants for the dCamera_c camera commands.
 * data_02086d00..data_02086d68 are 8-byte camera-script records: {handler,
 * adjust} pairs whose handler is a dCamera_c camera-command member function and
 * whose adjust word is 0, plus the two-word script constant data_02086d50 the
 * zoomed-out camera path selects.
 * Consumers: src/engine/view/d_camera.cpp, __sinit_d_camera.cpp.
 * ROM 0x02086d00..0x02086d70, 112 bytes, 14 configured symbols.
 *
 * Order: mwcc emits one .data section per initialized object and the linker
 * concatenates them in declaration order, so the definitions below are in the
 * order that yields the ascending addresses above.
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

/* 0x02086d68: 8 bytes. */
CamScriptOp data_02086d68 = {(void *)&_Z25CutsceneDanceCloseupStartP9dCamera_c, 0};
