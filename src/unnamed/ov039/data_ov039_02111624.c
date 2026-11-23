#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[7];
} data_ov039_02111624_Layout;
typedef char data_ov039_02111624_size[sizeof(data_ov039_02111624_Layout) == 0x1c ? 1 : -1];

extern data_ov039_02111624_Layout data_ov039_02111624;

data_ov039_02111624_Layout data_ov039_02111624 = {{
    0xffff0000,
    0x17700106,
    0xec78f2ea,
    0x00000000,
    0xffff0000,
    0xf63c0106,
    0xe69cf6d2,
}};
