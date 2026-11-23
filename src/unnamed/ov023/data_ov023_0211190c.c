#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[7];
} data_ov023_0211190c_Layout;
typedef char data_ov023_0211190c_size[sizeof(data_ov023_0211190c_Layout) == 0x1c ? 1 : -1];

extern const data_ov023_0211190c_Layout data_ov023_0211190c;

const data_ov023_0211190c_Layout data_ov023_0211190c = {{
    0xfffff833,
    0xfffff81f,
    0xfffff80a,
    0xfffff7f6,
    0xfffff7e1,
    0xfffff7cd,
    0xfffff7b8,
}};
