#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov058_02111700_Layout;
typedef char data_ov058_02111700_size[sizeof(data_ov058_02111700_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov058_02111704_Layout;
typedef char data_ov058_02111704_size[sizeof(data_ov058_02111704_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov058_0211170c_Layout;
typedef char data_ov058_0211170c_size[sizeof(data_ov058_0211170c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov058_02111714_Layout;
typedef char data_ov058_02111714_size[sizeof(data_ov058_02111714_Layout) == 0x8 ? 1 : -1];

extern data_ov058_02111700_Layout data_ov058_02111700;
extern data_ov058_02111704_Layout data_ov058_02111704;
extern data_ov058_0211170c_Layout data_ov058_0211170c;
extern data_ov058_02111714_Layout data_ov058_02111714;
extern u8 data_ov058_021117f8[];
extern u8 data_ov058_02111724[];

data_ov058_02111700_Layout data_ov058_02111700 = {{
    0x000007a5,
}};

data_ov058_02111704_Layout data_ov058_02111704 = {{
    0xff030000,
    0x0000ffff,
}};

data_ov058_0211170c_Layout data_ov058_0211170c = {
    0x00000007,
    &data_ov058_021117f8[0],
};

data_ov058_02111714_Layout data_ov058_02111714 = {
    0x00000001,
    &data_ov058_02111724[0],
};
