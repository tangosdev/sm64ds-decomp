#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} data_ov031_0211155c_Layout;
typedef char data_ov031_0211155c_size[sizeof(data_ov031_0211155c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    void *f00c;
    u32 f010;
    u32 f014;
} data_ov031_02111560_Layout;
typedef char data_ov031_02111560_size[sizeof(data_ov031_02111560_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
    void *f014;
    u32 f018;
    void *f01c;
    u32 f020;
} data_ov031_02111578_Layout;
typedef char data_ov031_02111578_size[sizeof(data_ov031_02111578_Layout) == 0x24 ? 1 : -1];

extern data_ov031_0211155c_Layout data_ov031_0211155c;
extern data_ov031_02111560_Layout data_ov031_02111560;
extern data_ov031_02111578_Layout data_ov031_02111578;
extern u8 data_ov031_021115e8[];
extern u8 data_ov031_02111510[];
extern u8 data_ov031_02111520[];
extern u8 data_ov031_0211152c[];
extern u8 data_ov031_02111500[];
extern u8 data_ov031_021115a8[];
extern u8 data_ov031_02111518[];

data_ov031_0211155c_Layout data_ov031_0211155c = {
    &data_ov031_021115e8[0],
};

data_ov031_02111560_Layout data_ov031_02111560 = {
    &data_ov031_02111510[0], 0x07550757, 0x07590758,
    &data_ov031_02111520[0], 0x0af00001, 0x0000000f,
};

data_ov031_02111578_Layout data_ov031_02111578 = {
    0x00000104, &data_ov031_0211152c[0], 0x0000010b, &data_ov031_02111500[0],
    0x00000401, &data_ov031_021115a8[0], 0x00000108, &data_ov031_02111518[0],
    0x0000010e,
};
