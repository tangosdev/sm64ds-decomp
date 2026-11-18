/* dCamera_c camera-script record (continued from data_02086d80).
 * The 8-byte record data_02086e40 is the {handler, adjust} pair for
 * func_02005110(dCamera_c*); its adjust word is 0.
 * Consumers: src/engine/view/d_camera.cpp, __sinit_d_camera.cpp.
 * ROM 0x02086e40..0x02086e48, 8 bytes, 1 configured symbol.
 *
 * Order: mwcc emits one .data section per initialized object and the linker
 * concatenates them in declaration order, so the definitions below are in the
 * order that yields the ascending addresses above.
 */
#include "types.h"

/* The 8-byte member-function constants: int (CamPmfHost::*)(), the same 8-byte layout as CamScriptOp. */
typedef struct {
    void *fn;
    int adjust;
} CamPmf;

extern int _Z13func_02005110P9dCamera_c();

/* 0x02086e40: 8 bytes. */
CamPmf data_02086e40 = {(void *)&_Z13func_02005110P9dCamera_c, 0};
