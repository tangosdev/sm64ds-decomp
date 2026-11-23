#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov039_02111640_Layout;
typedef char data_ov039_02111640_size[sizeof(data_ov039_02111640_Layout) == 0x18 ? 1 : -1];

extern data_ov039_02111640_Layout data_ov039_02111640;

data_ov039_02111640_Layout data_ov039_02111640 = {{
    0x00000000,
    0xffff0000,
    0xfb500014,
    0xfe3ef830,
    0x2aaa0003,
    0xffff0000,
}};
