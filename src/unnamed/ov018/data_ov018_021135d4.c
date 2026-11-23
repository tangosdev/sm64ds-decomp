#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[8];
} data_ov018_021135d4_Layout;
typedef char data_ov018_021135d4_size[sizeof(data_ov018_021135d4_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[56];
} data_ov018_021135f4_Layout;
typedef char data_ov018_021135f4_size[sizeof(data_ov018_021135f4_Layout) == 0xe0 ? 1 : -1];

extern data_ov018_021135d4_Layout data_ov018_021135d4;
extern data_ov018_021135f4_Layout data_ov018_021135f4;

data_ov018_021135d4_Layout data_ov018_021135d4 = {{
    0xffff0000,
    0x0c1c0131,
    0x00c8fce0,
    0x00000000,
    0xff000000,
    0xed40001f,
    0xeffcfa25,
    0x00000000,
}};

data_ov018_021135f4_Layout data_ov018_021135f4 = {{
    0xff020000, 0xe8f40103, 0x07d0f95c, 0x00000000, 0xffff0000, 0xfaba0039, 0x1180f9c1, 0xb1c80000,
    0x00000000, 0x02780039, 0x13c1fa1e, 0xb1c80000, 0x00030000, 0x076c004c, 0x0ce4fa25, 0x0000ffff,
    0x047e0000, 0x0c390039, 0x0786f538, 0x00000000, 0x01ff0000, 0x10040103, 0x0190f448, 0x00000000,
    0xffff0000, 0x09c4009d, 0x0ed8f289, 0x00000000, 0x00010000, 0x0d7a009e, 0x0f0aee6c, 0x00000000,
    0x00020000, 0xfaca0014, 0x16eaf254, 0x20000001, 0x0b220000, 0xfc35004c, 0x15cdf254, 0x0000ffff,
    0x07490000, 0x042e0032, 0x15e0ee6c, 0x80000000, 0x0002c000, 0xecbe0032, 0x1194f95c, 0x00000000,
    0x00024000, 0xfe70004c, 0xf1f0ed56, 0x8000ffff, 0x047f0000, 0xfed4004f, 0xf448edee, 0x00000000,
}};
