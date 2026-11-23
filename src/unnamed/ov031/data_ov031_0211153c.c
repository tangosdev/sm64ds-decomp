#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov031_0211153c_Layout;
typedef char data_ov031_0211153c_size[sizeof(data_ov031_0211153c_Layout) == 0xc ? 1 : -1];

extern data_ov031_0211153c_Layout data_ov031_0211153c;

data_ov031_0211153c_Layout data_ov031_0211153c = {{
    0xcaf2f9fb,
    0xc000d2d8,
    0x04160000,
}};
