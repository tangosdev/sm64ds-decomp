#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov012_02111c14[];
extern Word data_ov012_02111ff8[];
extern Word data_ov012_02111c04[];
extern Word data_ov012_02111d2c[];
extern Word data_ov012_02111d74[];
extern Word data_ov012_02111b80[];
extern Word data_ov012_02111b84[];
extern Word data_ov012_02111b8c[];
extern Word data_ov012_02111b94[];
extern Word data_ov012_02111b9c[];
extern Word data_ov012_02111ba4[];
extern Word data_ov012_02111bac[];
extern Word data_ov012_02111bb4[];
extern Word data_ov012_02111bbc[];

#pragma define_section sRun ".data.c0Pre"                                                          \
                            ".data.c0Pre"                                                          \
                            ".data.c0Pre" far_data
#pragma section sRun begin

Word data_ov012_02111b80[1] = {{.w = 0x00000004}};

Word data_ov012_02111b8c[2] = {{.w = 0x00001400}, {.p = data_ov012_02111ff8}};

Word data_ov012_02111ba4[2] = {{.w = 0x00000009}, {.p = data_ov012_02111d2c}};

Word data_ov012_02111b9c[2] = {{.w = 0x00000002}, {.p = data_ov012_02111c04}};

Word data_ov012_02111b94[2] = {{.w = 0x00000001}, {.p = data_ov012_02111b8c}};

Word data_ov012_02111bac[2] = {{.w = 0x00000500}, {.p = data_ov012_02111d74}};

Word data_ov012_02111b84[2] = {{.w = 0x00000100}, {.p = data_ov012_02111c14}};

Word data_ov012_02111bb4[2] = {{.w = 0x00000001}, {.p = data_ov012_02111bac}};

Word data_ov012_02111bbc[2] = {{.w = 0x00000001}, {.p = data_ov012_02111b84}};

#pragma section sRun end
