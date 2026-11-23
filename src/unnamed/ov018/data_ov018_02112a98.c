#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov018_02112a98_Layout;
typedef char data_ov018_02112a98_size[sizeof(data_ov018_02112a98_Layout) == 0xc ? 1 : -1];

extern const data_ov018_02112a98_Layout data_ov018_02112a98;

const data_ov018_02112a98_Layout data_ov018_02112a98 = {{
    0x00000009,
    0x00000036,
    0x00000064,
}};
