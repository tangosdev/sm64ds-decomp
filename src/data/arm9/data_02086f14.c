/* Camera script-op and state member-function constants (dCamera_c).
 * The 8-byte records data_02086bc0..data_02086e40 are the pointer-to-member
 * constants RunCameraCommand and the state table index; every adjust word is 0.
 * The strings are the _ZTS names and the _ZTI records of the camera classes,
 * and 0x02086e84..0x02086f7c holds the look-at/position offset triples.
 * Consumer: src/engine/view/d_camera.cpp, __sinit_d_camera.cpp.
 * ROM 0x02086f14..0x02086f58, 68 bytes, 5 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* The 8-byte member-function constants: int (CamPmfHost::*)(), the same 8-byte layout as CamScriptOp. */
typedef struct {
    void *fn;
    int adjust;
} CamPmf;

/* 0x02086f38: 16 bytes. */
int data_02086f38[4] = {
    0x01f40000,
    (int)0xfe0c0000u,
    0x01b8a000,
    (int)0xfe0c0000u,
};

/* 0x02086f2c: 12 bytes. */
Vector3 data_02086f2c = {0x00000000, 0x00000000, 0x0001b4e0};

/* 0x02086f20: 12 bytes. */
Vector3 data_02086f20 = {0x00000000, 0x00000000, (int)0xffece300u};

/* 0x02086f14: 12 bytes. */
Vector3 data_02086f14 = {0x00000000, 0x00000000, (int)0xffece300u};

/* 0x02086f48: 16 bytes. */
int data_02086f48[4] = {
    0x0119c000,
    (int)0xff165000u,
    0x0119c000,
    (int)0xff165000u,
};
