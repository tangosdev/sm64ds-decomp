#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov089_02132b40_Layout;
typedef char data_ov089_02132b40_size[sizeof(data_ov089_02132b40_Layout) == 0xc ? 1 : -1];

extern data_ov089_02132b40_Layout data_ov089_02132b40;

data_ov089_02132b40_Layout data_ov089_02132b40 = {{
    0x00000000,
    0xfff92000,
    0x00000000,
}};
