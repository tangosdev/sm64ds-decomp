#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov052_0211227c_Layout;
typedef char data_ov052_0211227c_size[sizeof(data_ov052_0211227c_Layout) == 0x18 ? 1 : -1];

extern data_ov052_0211227c_Layout data_ov052_0211227c;

data_ov052_0211227c_Layout data_ov052_0211227c = {{
    0x00000000,
    0xffff0000,
    0xf7cc0017,
    0xf7cc0fa0,
    0x00000000,
    0xffff0000,
}};
