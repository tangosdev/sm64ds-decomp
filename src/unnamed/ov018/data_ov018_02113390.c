#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov018_02113390_Layout;
typedef char data_ov018_02113390_size[sizeof(data_ov018_02113390_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[35];
} data_ov018_021133a8_Layout;
typedef char data_ov018_021133a8_size[sizeof(data_ov018_021133a8_Layout) == 0x8c ? 1 : -1];

typedef struct {
    u32 w[57];
} data_ov018_02113434_Layout;
typedef char data_ov018_02113434_size[sizeof(data_ov018_02113434_Layout) == 0xe4 ? 1 : -1];

extern data_ov018_02113390_Layout data_ov018_02113390;
extern data_ov018_021133a8_Layout data_ov018_021133a8;
extern data_ov018_02113434_Layout data_ov018_02113434;

data_ov018_02113390_Layout data_ov018_02113390 = {{
    0xf9415e29,
    0xf92909c4,
    0xfe235e29,
    0xf73509c4,
    0xff4ce225,
    0xfa600bb8,
}};

data_ov018_021133a8_Layout data_ov018_021133a8 = {{
    0xff4ce225, 0xfa600c4e, 0xff4ce225, 0xfa600ce4, 0xff4ce225, 0xfa600d7a, 0xff4ce225,
    0xfa600e10, 0x08885e29, 0xf9c809c3, 0x0b005e29, 0xf9c009c3, 0x0dace225, 0xfe70079c,
    0x0dace225, 0xff380780, 0x0dace225, 0x00000764, 0x0dace225, 0x00c80744, 0x0dace225,
    0x01900724, 0xfb50e225, 0x0758047c, 0xfa88e225, 0x07080474, 0xf9c0e225, 0x06cc046c,
    0xf8f8e225, 0x06900463, 0xf830e225, 0x0640045a, 0xf2dd5e29, 0xf5a60313, 0xfbb4e225,
}};

data_ov018_02113434_Layout data_ov018_02113434 = {{
    0xf57401bc, 0xfc7ce225, 0xf574019e, 0xfd44e225, 0xf5740181, 0xfe0ce225, 0xf5740161, 0xfed4e225,
    0xf5740141, 0xfb50e225, 0xf18cfb9a, 0xfa88e225, 0xf18cfb93, 0xf9c0e225, 0xf18cfb8c, 0xf8f8e225,
    0xf18cfb86, 0xf830e225, 0xf18cfb7f, 0xe9855e29, 0x0318f95c, 0xec295e29, 0x0b6cf95c, 0xeafd5e29,
    0x0fd3f95c, 0xee8a5e29, 0x125df95c, 0xfb305e29, 0x162cf254, 0x04c25e29, 0x14e1ee6c, 0x07965e29,
    0x12e1ee6c, 0xf8300c3d, 0xf510f791, 0xf66ee225, 0xf060ed63, 0xf63ce225, 0xeffced63, 0xf60ae225,
    0xf09ced65, 0xf60ae225, 0xef98ed62, 0xf5d8e225, 0xf038ed65, 0xf5a6e225, 0xf0d8ed67, 0xf542e225,
    0xf010ed66, 0xf4ace225, 0xefdeed69, 0xf1b35e29, 0x1107ee6c, 0x0dac0227, 0xfd12fd77, 0x0dac0227,
    0xf95cfe98,
}};
