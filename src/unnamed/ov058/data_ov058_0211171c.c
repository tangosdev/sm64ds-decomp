#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov058_0211171c_Layout;
typedef char data_ov058_0211171c_size[sizeof(data_ov058_0211171c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
} data_ov058_02111720_Layout;
typedef char data_ov058_02111720_size[sizeof(data_ov058_02111720_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov058_02111724_Layout;
typedef char data_ov058_02111724_size[sizeof(data_ov058_02111724_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov058_0211172c_Layout;
typedef char data_ov058_0211172c_size[sizeof(data_ov058_0211172c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov058_02111734_Layout;
typedef char data_ov058_02111734_size[sizeof(data_ov058_02111734_Layout) == 0x8 ? 1 : -1];

extern data_ov058_0211171c_Layout data_ov058_0211171c;
extern data_ov058_02111720_Layout data_ov058_02111720;
extern data_ov058_02111724_Layout data_ov058_02111724;
extern data_ov058_0211172c_Layout data_ov058_0211172c;
extern data_ov058_02111734_Layout data_ov058_02111734;
extern u8 data_ov058_02111784[];
extern u8 data_ov058_021117c8[];
extern u8 data_ov058_02111744[];

data_ov058_0211171c_Layout data_ov058_0211171c = {{
    0x00000200,
}};

data_ov058_02111720_Layout data_ov058_02111720 = {
    &data_ov058_02111784[0],
};

data_ov058_02111724_Layout data_ov058_02111724 = {
    0x00000300,
    &data_ov058_021117c8[0],
};

data_ov058_0211172c_Layout data_ov058_0211172c = {
    0x00000100,
    &data_ov058_02111744[0],
};

data_ov058_02111734_Layout data_ov058_02111734 = {
    0x00000001,
    &data_ov058_0211172c,
};
