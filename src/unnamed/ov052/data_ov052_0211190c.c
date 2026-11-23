#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[7];
} data_ov052_0211190c_Layout;
typedef char data_ov052_0211190c_size[sizeof(data_ov052_0211190c_Layout) == 0x1c ? 1 : -1];

extern const data_ov052_0211190c_Layout data_ov052_0211190c;

const data_ov052_0211190c_Layout data_ov052_0211190c = {{
    0x000002c1,
    0x000002d8,
    0x000002ef,
    0x00000306,
    0x0000031c,
    0x00000333,
    0x0000034a,
}};
