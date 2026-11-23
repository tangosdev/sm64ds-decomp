#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} data_ov006_02138c18_Layout;
typedef char data_ov006_02138c18_size[sizeof(data_ov006_02138c18_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02138c1c_Layout;
typedef char data_ov006_02138c1c_size[sizeof(data_ov006_02138c1c_Layout) == 0x20 ? 1 : -1];

extern data_ov006_02138c18_Layout data_ov006_02138c18;
extern data_ov006_02138c1c_Layout data_ov006_02138c1c;

#pragma define_section sName ".data.gap32"                                                         \
                             ".data.gap32"                                                         \
                             ".data.gap32" far_data
#pragma section sName begin

data_ov006_02138c18_Layout data_ov006_02138c18 = {
    &data_ov006_02138c1c,
};

data_ov006_02138c1c_Layout data_ov006_02138c1c = {{
    0x81e800ec,
    0x00000000,
    0x900880ec,
    0x00000000,
    0x41e8400c,
    0x00000004,
    0x0008400c,
    0xffff0024,
}};
