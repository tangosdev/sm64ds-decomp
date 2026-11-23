#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov019_02112c84_Layout;
typedef char data_ov019_02112c84_size[sizeof(data_ov019_02112c84_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
    u32 f00c;
    void *f010;
    u32 f014;
    void *f018;
    u32 f01c;
} data_ov019_02112c88_Layout;
typedef char data_ov019_02112c88_size[sizeof(data_ov019_02112c88_Layout) == 0x20 ? 1 : -1];

extern data_ov019_02112c84_Layout data_ov019_02112c84;
extern data_ov019_02112c88_Layout data_ov019_02112c88;
extern u8 data_ov019_02112e24[];
extern u8 data_ov019_02112b88[];
extern u8 data_ov019_02112be8[];
extern u8 data_ov019_02112b80[];

data_ov019_02112c84_Layout data_ov019_02112c84 = {{
    0x00005602,
}};

data_ov019_02112c88_Layout data_ov019_02112c88 = {
    &data_ov019_02112e24[0], 0x00000103, &data_ov019_02112b88[0], 0x00000104,
    &data_ov019_02112be8[0], 0x0000010b, &data_ov019_02112b80[0], 0x00000401,
};
