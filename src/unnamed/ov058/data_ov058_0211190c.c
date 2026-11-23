#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[5];
} data_ov058_0211190c_Layout;
typedef char data_ov058_0211190c_size[sizeof(data_ov058_0211190c_Layout) == 0x14 ? 1 : -1];

extern data_ov058_0211190c_Layout data_ov058_0211190c;

data_ov058_0211190c_Layout data_ov058_0211190c = {{
    0x00190000,
    0x05d20000,
    0xf6fa0514,
    0xe0000000,
    0x02200000,
}};
