/* Camera script-op and state member-function constants (dCamera_c).
 * The 8-byte records data_02086bc0..data_02086e40 are the pointer-to-member
 * constants RunCameraCommand and the state table index; every adjust word is 0.
 * The strings are the _ZTS names and the _ZTI records of the camera classes,
 * and 0x02086e84..0x02086f7c holds the look-at/position offset triples.
 * Consumer: src/engine/view/d_camera.cpp, __sinit_d_camera.cpp.
 * ROM 0x02086e40..0x02086ef0, 176 bytes, 15 configured symbols.
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

extern char _ZTS7dView_c[12];
extern char _ZTS7dBase_c[12];
extern char _ZTS9dCamera_c[12];
extern u32 _ZTI7dBase_c[3];
extern u32 _ZTI7dView_c[3];
extern int _Z13func_02005110P9dCamera_c();
extern u8 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTI7fBase_c[];

/* 0x02086ed8: 12 bytes. */
Vector3 data_02086ed8 = {0x00258000, 0x00000000, (int)0xffe70000u};

/* 0x02086ecc: 12 bytes. */
u32 _ZTI7dView_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS7dView_c,
    (u32)_ZTI7dBase_c,
};

/* 0x02086ec0: 12 bytes. */
Vector3 data_02086ec0 = {0x00000000, 0x00096000, (int)0xffda8000u};

/* 0x02086eb4: 12 bytes. */
Vector3 data_02086eb4 = {0x00000000, 0x00032000, 0x00300000};

/* 0x02086ea8: 12 bytes. */
Vector3 data_02086ea8 = {0x00000000, 0x00000000, (int)0xfff00000u};

/* 0x02086e9c: 12 bytes. */
Vector3 data_02086e9c = {0x00000000, 0x00080000, 0x00000000};

/* 0x02086e90: 12 bytes. */
Vector3 data_02086e90 = {0x00000000, 0x00080000, 0x00000000};

/* 0x02086e54: 12 bytes. */
char _ZTS7dBase_c[12] = "7dBase_c";

/* 0x02086e78: 12 bytes. */
u32 _ZTI7dBase_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS7dBase_c,
    (u32)_ZTI7fBase_c,
};

/* 0x02086e6c: 12 bytes. */
char _ZTS9dCamera_c[12] = "9dCamera_c";

/* 0x02086e60: 12 bytes. */
char _ZTS7fBase_c[12] = "7fBase_c";

/* 0x02086e84: 12 bytes. */
Vector3 data_02086e84 = {0x00000000, 0x00000000, (int)0xffed4000u};

/* 0x02086e48: 12 bytes. */
char _ZTS7dView_c[12] = "7dView_c";

/* 0x02086e40: 8 bytes. */
CamPmf data_02086e40 = {(void *)&_Z13func_02005110P9dCamera_c, 0};

/* 0x02086ee4: 12 bytes. */
u32 _ZTI9dCamera_c[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTS9dCamera_c,
    (u32)_ZTI7dView_c,
};
