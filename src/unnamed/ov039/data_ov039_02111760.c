#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov039_02111760_Layout;
typedef char data_ov039_02111760_size[sizeof(data_ov039_02111760_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[44];
} data_ov039_02111768_Layout;
typedef char data_ov039_02111768_size[sizeof(data_ov039_02111768_Layout) == 0xb0 ? 1 : -1];

extern data_ov039_02111760_Layout data_ov039_02111760;
extern data_ov039_02111768_Layout data_ov039_02111768;

data_ov039_02111760_Layout data_ov039_02111760 = {{
    0x0000e225,
    0x0fa00258,
}};

data_ov039_02111768_Layout data_ov039_02111768 = {{
    0x0000e225, 0x0f3c0190, 0x0000e225, 0x0e74012c, 0x0000e225, 0x0dac0190, 0x0000e225, 0x0d480258,
    0x0000e225, 0x0dac0320, 0xff9c2226, 0x0e740258, 0x00642226, 0x0e740258, 0xf574e225, 0x09c40a28,
    0xfa24e225, 0x0d480960, 0x05dce225, 0x0d480898, 0x0a8ce225, 0x09c407d0, 0x0d48e225, 0x04b00708,
    0x0d48e225, 0xfb5006a4, 0x0a8ce225, 0xf63c05dc, 0x05dce225, 0xf2b80578, 0xfa24e225, 0xf2b80514,
    0xf574e225, 0xf63c04b0, 0xf2b8e225, 0xfb50044c, 0x0000e225, 0x0e7408fc, 0xf2b8e225, 0x05140320,
    0xf574e225, 0x09c402bc, 0xfa88e225, 0x0d480258,
}};
