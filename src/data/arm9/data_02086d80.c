/* Camera script-op constants for the dCamera_c camera commands (continued).
 * The 8-byte records data_02086d80..data_02086e38 are {handler, adjust} pairs
 * whose handler word is a dCamera_c camera-command member function; every
 * adjust word is 0.
 * Consumers: src/engine/view/d_camera.cpp, __sinit_d_camera.cpp.
 * ROM 0x02086d80..0x02086e40, 192 bytes, 24 configured symbols.
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

extern int _ZN9dCamera_c13func_02009e70Ev();
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

/* 0x02086e38: 8 bytes. */
CamScriptOp data_02086e38 = {(void *)&_Z13func_02008028P9dCamera_c, 0};
