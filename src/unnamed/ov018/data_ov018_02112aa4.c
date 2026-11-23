#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov018_02112aa4_Layout;
typedef char data_ov018_02112aa4_size[sizeof(data_ov018_02112aa4_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov018_02112aa8_Layout;
typedef char data_ov018_02112aa8_size[sizeof(data_ov018_02112aa8_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov018_02112ac0_Layout;
typedef char data_ov018_02112ac0_size[sizeof(data_ov018_02112ac0_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[11];
} data_ov018_02112adc_Layout;
typedef char data_ov018_02112adc_size[sizeof(data_ov018_02112adc_Layout) == 0x2c ? 1 : -1];

typedef struct {
    u32 w[31];
} data_ov018_02112b08_Layout;
typedef char data_ov018_02112b08_size[sizeof(data_ov018_02112b08_Layout) == 0x7c ? 1 : -1];

extern const data_ov018_02112aa4_Layout data_ov018_02112aa4;
extern const data_ov018_02112aa8_Layout data_ov018_02112aa8;
extern const data_ov018_02112ac0_Layout data_ov018_02112ac0;
extern const data_ov018_02112adc_Layout data_ov018_02112adc;
extern const data_ov018_02112b08_Layout data_ov018_02112b08;

const data_ov018_02112aa4_Layout data_ov018_02112aa4 = {{
    0x00000091,
}};

const data_ov018_02112aa8_Layout data_ov018_02112aa8 = {{
    0x000000bf,
    0x000000ec,
    0x0000011a,
    0x00000147,
    0x00000174,
    0x000001a2,
}};

const data_ov018_02112ac0_Layout data_ov018_02112ac0 = {{
    0x000001cf,
    0x000001fd,
    0x0000022a,
    0x00000257,
    0x00000285,
    0x000002b2,
    0x000002e0,
}};

const data_ov018_02112adc_Layout data_ov018_02112adc = {{
    0x0000030d,
    0x0000033a,
    0x00000368,
    0x00000395,
    0x000003c3,
    0x000003f0,
    0x0000041e,
    0x0000044b,
    0x00000478,
    0x000004a6,
    0x000004d3,
}};

const data_ov018_02112b08_Layout data_ov018_02112b08 = {{
    0x00000501, 0x0000052e, 0x0000055b, 0x00000589, 0x000005b6, 0x000005e4, 0x00000611, 0x0000063e,
    0x0000066c, 0x00000699, 0x000006c7, 0x000006f4, 0x00000721, 0x0000074f, 0x0000077c, 0x000007aa,
    0x000007d7, 0x00000805, 0x00000832, 0x0000085f, 0x0000088d, 0x000008ba, 0x000008e8, 0x00000915,
    0x00000942, 0x00000970, 0x0000099d, 0x000009cb, 0x000009f8, 0x00000a25, 0x00000a53,
}};
