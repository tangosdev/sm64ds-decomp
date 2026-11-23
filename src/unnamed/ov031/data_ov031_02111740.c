#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov031_02111740_Layout;
typedef char data_ov031_02111740_size[sizeof(data_ov031_02111740_Layout) == 0x18 ? 1 : -1];

extern data_ov031_02111740_Layout data_ov031_02111740;

data_ov031_02111740_Layout data_ov031_02111740 = {{
    0x00000000,
    0xffff0000,
    0x0d0200c3,
    0xe25f01b9,
    0x00000000,
    0xffff0000,
}};
