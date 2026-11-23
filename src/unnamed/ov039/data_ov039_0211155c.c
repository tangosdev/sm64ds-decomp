#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov039_0211155c_Layout;
typedef char data_ov039_0211155c_size[sizeof(data_ov039_0211155c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[15];
} data_ov039_02111560_Layout;
typedef char data_ov039_02111560_size[sizeof(data_ov039_02111560_Layout) == 0x3c ? 1 : -1];

extern data_ov039_0211155c_Layout data_ov039_0211155c;
extern data_ov039_02111560_Layout data_ov039_02111560;

data_ov039_0211155c_Layout data_ov039_0211155c = {{
    0x0fa0047e,
}};

data_ov039_02111560_Layout data_ov039_02111560 = {{
    0x00000000,
    0xffff0000,
    0xf8300106,
    0x07d0fcae,
    0x00000000,
    0xffff0000,
    0x06400106,
    0xfa88f4ac,
    0x00000000,
    0xffff0000,
    0x03e80106,
    0x05dc047e,
    0x00000000,
    0xffff0000,
    0x11940106,
}};
