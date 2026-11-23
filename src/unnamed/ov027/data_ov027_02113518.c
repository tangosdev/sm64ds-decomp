#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[7];
} data_ov027_02113518_Layout;
typedef char data_ov027_02113518_size[sizeof(data_ov027_02113518_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov027_02113534_Layout;
typedef char data_ov027_02113534_size[sizeof(data_ov027_02113534_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[32];
} data_ov027_02113554_Layout;
typedef char data_ov027_02113554_size[sizeof(data_ov027_02113554_Layout) == 0x80 ? 1 : -1];

extern data_ov027_02113518_Layout data_ov027_02113518;
extern data_ov027_02113534_Layout data_ov027_02113534;
extern data_ov027_02113554_Layout data_ov027_02113554;

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov027_02113518_Layout data_ov027_02113518 = {{
    0xec140400,
    0x00000000,
    0xffff0000,
    0xfda8004c,
    0xf1f00432,
    0xc000ffff,
    0x05aa0000,
}};

data_ov027_02113534_Layout data_ov027_02113534 = {{
    0xf1f0001f,
    0xef34053e,
    0x00000000,
    0xff020000,
    0xf1f00103,
    0xebb0044e,
    0x00000000,
    0xffff0000,
}};

data_ov027_02113554_Layout data_ov027_02113554 = {{
    0xee6c0103, 0xf060040e, 0x00000000, 0xffff0000, 0xf18c0039, 0xf31c0434, 0x00000000, 0x01ff0000,
    0xf31c0103, 0xff3803e8, 0x00000000, 0xffff0000, 0xf1280103, 0x04b003e8, 0x00000000, 0xffff0000,
    0xf18c004c, 0xfd4403e8, 0x5c71ffff, 0x05ab0000, 0xee08001f, 0x16a804e2, 0x00000000, 0x04010000,
    0xeae8001f, 0x16a804e2, 0x00000000, 0xff030000, 0xec78004c, 0x170c03e8, 0x8000ffff, 0x073b0000,
}};
