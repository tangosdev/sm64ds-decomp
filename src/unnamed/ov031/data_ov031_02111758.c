#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov031_02111758_Layout;
typedef char data_ov031_02111758_size[sizeof(data_ov031_02111758_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov031_02111760_Layout;
typedef char data_ov031_02111760_size[sizeof(data_ov031_02111760_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[51];
} data_ov031_02111768_Layout;
typedef char data_ov031_02111768_size[sizeof(data_ov031_02111768_Layout) == 0xcc ? 1 : -1];

extern data_ov031_02111758_Layout data_ov031_02111758;
extern data_ov031_02111760_Layout data_ov031_02111760;
extern data_ov031_02111768_Layout data_ov031_02111768;

data_ov031_02111760_Layout data_ov031_02111760 = {{
    0x1f25e225,
    0x23b32e65,
}};

data_ov031_02111758_Layout data_ov031_02111758 = {{
    0x29b1e225,
    0x3001322d,
}};

data_ov031_02111768_Layout data_ov031_02111768 = {{
    0x1e8fe225, 0x231d2e65, 0x1df9e225, 0x22872e33, 0x1d63e225, 0x21f12de4, 0x1ccde225, 0x215b2d46,
    0x12d7e225, 0x17972819, 0x1241e225, 0x170127d8, 0x11abe225, 0x166b273b, 0x1147e225, 0x16072620,
    0x10e3e225, 0x15a32505, 0x06ede225, 0x0b7b1f8d, 0xe4c09a27, 0x0b49180d, 0xd4eee225, 0x08bf12a9,
    0xd232e225, 0x06031119, 0xd070e225, 0x01e90ec1, 0xcfa81a27, 0xfff60e0d, 0xcfda1a27, 0xee6208a5,
    0xdcbee225, 0xd65c02cb, 0xdf7ae225, 0xd65c024c, 0xe236e225, 0xd7560241, 0xe3f8e225, 0xd8e60197,
    0xe556e225, 0xda7600ed, 0xe61ee225, 0xe14cfe87, 0xe5ece225, 0xea7afa72, 0xe524e225, 0xf470f569,
    0xe524e225, 0xf506f52c, 0xe524e225,
}};
