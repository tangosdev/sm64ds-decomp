#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov052_02111b34_Layout;
typedef char data_ov052_02111b34_size[sizeof(data_ov052_02111b34_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov052_02111b44_Layout;
typedef char data_ov052_02111b44_size[sizeof(data_ov052_02111b44_Layout) == 0x20 ? 1 : -1];

extern const data_ov052_02111b34_Layout data_ov052_02111b34;
extern const data_ov052_02111b44_Layout data_ov052_02111b44;

const data_ov052_02111b34_Layout data_ov052_02111b34 = {{
    0x00000f06,
    0x00000f1c,
    0x00000f33,
    0x00000f4a,
}};

const data_ov052_02111b44_Layout data_ov052_02111b44 = {{
    0x00000f61,
    0x00000f77,
    0x00000f8e,
    0x00000fa5,
    0x00000fbc,
    0x00000fd2,
    0x00000fe9,
    0x00001000,
}};
