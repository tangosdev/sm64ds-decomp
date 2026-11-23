#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[9];
} data_ov019_02112f58_Layout;
typedef char data_ov019_02112f58_size[sizeof(data_ov019_02112f58_Layout) == 0x24 ? 1 : -1];

typedef struct {
    u32 w[21];
} data_ov019_02112f7c_Layout;
typedef char data_ov019_02112f7c_size[sizeof(data_ov019_02112f7c_Layout) == 0x54 ? 1 : -1];

extern data_ov019_02112f58_Layout data_ov019_02112f58;
extern data_ov019_02112f7c_Layout data_ov019_02112f7c;

data_ov019_02112f58_Layout data_ov019_02112f58 = {{
    0xf081fe3a,
    0xfd761964,
    0x1964f448,
    0xf8a4fcaa,
    0xfbad1964,
    0x1964fe0c,
    0x01f2f962,
    0xf7511964,
    0x19640578,
}};

data_ov019_02112f7c_Layout data_ov019_02112f7c = {{
    0x0a3df6b7, 0xf6171964, 0x19640f2b, 0x13ecf57e, 0xf53b1900, 0x17d41644, 0x17d4f4d8,
    0xf48d16a8, 0x1388189c, 0x1964f406, 0xf3871004, 0x0b221900, 0x17dff306, 0xf27d05f9,
    0x00c816ae, 0x157cf1f4, 0xf19bfe0c, 0xfc7c1388, 0x10ccf133, 0xf0bcfce0, 0xfda80dac,
}};
