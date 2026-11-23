#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov052_02112358_Layout;
typedef char data_ov052_02112358_size[sizeof(data_ov052_02112358_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[31];
} data_ov052_02112368_Layout;
typedef char data_ov052_02112368_size[sizeof(data_ov052_02112368_Layout) == 0x7c ? 1 : -1];

extern data_ov052_02112358_Layout data_ov052_02112358;
extern data_ov052_02112368_Layout data_ov052_02112368;

data_ov052_02112358_Layout data_ov052_02112358 = {{
    0xe9bc186a,
    0x00000000,
    0x00170000,
    0xfb500006,
}};

data_ov052_02112368_Layout data_ov052_02112368 = {{
    0xfda80fa0, 0x40000000, 0x00030000, 0xf8300006, 0xfb500fa0, 0x00000000, 0x00020000, 0xfb500006,
    0x02580fa0, 0x40000000, 0x00010000, 0xf8300006, 0x00000fa0, 0x00000000, 0x00000000, 0xf2b80094,
    0xe9bc1130, 0x00000000, 0x00000000, 0xf8300006, 0xf6a00fa0, 0x00000000, 0x00040000, 0xfb500006,
    0xf8f80fa0, 0x40000000, 0x00050000, 0xe9620006, 0xf1f00fa0, 0x40000000, 0x00060000,
}};
