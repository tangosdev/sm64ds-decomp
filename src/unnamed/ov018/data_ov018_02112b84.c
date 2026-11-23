#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[9];
} data_ov018_02112b84_Layout;
typedef char data_ov018_02112b84_size[sizeof(data_ov018_02112b84_Layout) == 0x24 ? 1 : -1];

typedef struct {
    u32 w[13];
} data_ov018_02112ba8_Layout;
typedef char data_ov018_02112ba8_size[sizeof(data_ov018_02112ba8_Layout) == 0x34 ? 1 : -1];

extern const data_ov018_02112b84_Layout data_ov018_02112b84;
extern const data_ov018_02112ba8_Layout data_ov018_02112ba8;

const data_ov018_02112b84_Layout data_ov018_02112b84 = {{
    0x00000a80,
    0x00000aae,
    0x00000adb,
    0x00000b09,
    0x00000b36,
    0x00000b63,
    0x00000b91,
    0x00000bbe,
    0x00000bec,
}};

const data_ov018_02112ba8_Layout data_ov018_02112ba8 = {{
    0x00000c19,
    0x00000c46,
    0x00000c74,
    0x00000ca1,
    0x00000ccf,
    0x00000cfc,
    0x00000d29,
    0x00000d57,
    0x00000d84,
    0x00000db2,
    0x00000ddf,
    0x00000e0c,
    0x00000e3a,
}};
