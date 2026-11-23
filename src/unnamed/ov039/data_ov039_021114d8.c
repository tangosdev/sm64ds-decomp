#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[13];
} data_ov039_021114d8_Layout;
typedef char data_ov039_021114d8_size[sizeof(data_ov039_021114d8_Layout) == 0x34 ? 1 : -1];

extern data_ov039_021114d8_Layout data_ov039_021114d8;

data_ov039_021114d8_Layout data_ov039_021114d8 = {{
    0x53504c43,
    0x00040008,
    0x00040fc4,
    0x000000ff,
    0x01040fc4,
    0x000000ff,
    0x012c0fc0,
    0x000000ff,
    0x01003fc0,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};
