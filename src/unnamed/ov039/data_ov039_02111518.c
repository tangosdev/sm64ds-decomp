#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov039_02111518_Layout;
typedef char data_ov039_02111518_size[sizeof(data_ov039_02111518_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov039_02111520_Layout;
typedef char data_ov039_02111520_size[sizeof(data_ov039_02111520_Layout) == 0x1c ? 1 : -1];

extern data_ov039_02111518_Layout data_ov039_02111518;
extern data_ov039_02111520_Layout data_ov039_02111520;

data_ov039_02111518_Layout data_ov039_02111518 = {{
    0x0000001e,
    0xfd44f92a,
}};

data_ov039_02111520_Layout data_ov039_02111520 = {{
    0x00000000,
    0xff050000,
    0x0000001b,
    0x0000f830,
    0x00000000,
    0xffff0000,
    0xea840106,
}};
