#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov027_02112fe0_Layout;
typedef char data_ov027_02112fe0_size[sizeof(data_ov027_02112fe0_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov027_02112fe4_Layout;
typedef char data_ov027_02112fe4_size[sizeof(data_ov027_02112fe4_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov027_02112fe8_Layout;
typedef char data_ov027_02112fe8_size[sizeof(data_ov027_02112fe8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov027_02112ff0_Layout;
typedef char data_ov027_02112ff0_size[sizeof(data_ov027_02112ff0_Layout) == 0x8 ? 1 : -1];

extern data_ov027_02112fe0_Layout data_ov027_02112fe0;
extern data_ov027_02112fe4_Layout data_ov027_02112fe4;
extern data_ov027_02112fe8_Layout data_ov027_02112fe8;
extern data_ov027_02112ff0_Layout data_ov027_02112ff0;
extern u8 data_ov027_021131cc[];

#pragma define_section sName ".data.pre"                                                           \
                             ".data.pre"                                                           \
                             ".data.pre" far_data
#pragma section sName begin

data_ov027_02112fe0_Layout data_ov027_02112fe0 = {{
    0x000007b9,
}};

data_ov027_02112fe4_Layout data_ov027_02112fe4 = {{
    0x00101014,
}};

data_ov027_02112fe8_Layout data_ov027_02112fe8 = {{
    0xff020000,
    0x00007f01,
}};

data_ov027_02112ff0_Layout data_ov027_02112ff0 = {
    0x00000009,
    &data_ov027_021131cc[0],
};
