#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov047_02111c0c_Layout;
typedef char data_ov047_02111c0c_size[sizeof(data_ov047_02111c0c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov047_02111c14_Layout;
typedef char data_ov047_02111c14_size[sizeof(data_ov047_02111c14_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[11];
} data_ov047_02111c20_Layout;
typedef char data_ov047_02111c20_size[sizeof(data_ov047_02111c20_Layout) == 0x2c ? 1 : -1];

extern data_ov047_02111c0c_Layout data_ov047_02111c0c;
extern data_ov047_02111c14_Layout data_ov047_02111c14;
extern data_ov047_02111c20_Layout data_ov047_02111c20;

data_ov047_02111c0c_Layout data_ov047_02111c0c = {{
    0xffffffff,
    0xffffffff,
}};

data_ov047_02111c14_Layout data_ov047_02111c14 = {{
    0xe53eff02,
    0x03b6ef90,
    0x80000000,
}};

data_ov047_02111c20_Layout data_ov047_02111c20 = {{
    0xff020000,
    0xfce017a2,
    0x00000226,
    0x00008000,
    0xff38ff07,
    0xdfc6e82c,
    0x00000000,
    0xff020000,
    0x1900015e,
    0x0000f222,
    0x00008000,
}};
