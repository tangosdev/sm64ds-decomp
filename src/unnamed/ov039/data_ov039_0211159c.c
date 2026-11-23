#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[8];
} data_ov039_0211159c_Layout;
typedef char data_ov039_0211159c_size[sizeof(data_ov039_0211159c_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[9];
} data_ov039_021115bc_Layout;
typedef char data_ov039_021115bc_size[sizeof(data_ov039_021115bc_Layout) == 0x24 ? 1 : -1];

extern data_ov039_0211159c_Layout data_ov039_0211159c;
extern data_ov039_021115bc_Layout data_ov039_021115bc;

data_ov039_0211159c_Layout data_ov039_0211159c = {{
    0x05dcf2ea,
    0x00000000,
    0xffff0000,
    0xf0600106,
    0x01f4028a,
    0x00000000,
    0xffff0000,
    0xec780106,
}};

data_ov039_021115bc_Layout data_ov039_021115bc = {{
    0x03e8f6d2,
    0x00000000,
    0xffff0000,
    0x11940106,
    0xfe0cfea2,
    0x00000000,
    0xffff0000,
    0x17700106,
    0x01f4faba,
}};
