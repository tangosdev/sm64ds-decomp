#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov018_02113800_Layout;
typedef char data_ov018_02113800_size[sizeof(data_ov018_02113800_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov018_02113804_Layout;
typedef char data_ov018_02113804_size[sizeof(data_ov018_02113804_Layout) == 0x1c ? 1 : -1];

extern data_ov018_02113800_Layout data_ov018_02113800;
extern data_ov018_02113804_Layout data_ov018_02113804;

#pragma explicit_zero_data on
data_ov018_02113800_Layout data_ov018_02113800 = {0};
#pragma explicit_zero_data off

data_ov018_02113804_Layout data_ov018_02113804 = {{
    0xf1f20000,
    0x03520108,
    0xfea20c21,
    0x00000000,
    0xffff0000,
    0xfcc4011d,
    0xfc6c0d2a,
}};
