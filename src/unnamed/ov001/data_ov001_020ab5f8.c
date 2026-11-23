#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov001_020ab5f8_Layout;
typedef char data_ov001_020ab5f8_size[sizeof(data_ov001_020ab5f8_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov001_020ab5fc_Layout;
typedef char data_ov001_020ab5fc_size[sizeof(data_ov001_020ab5fc_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov001_020ab600_Layout;
typedef char data_ov001_020ab600_size[sizeof(data_ov001_020ab600_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov001_020ab604_Layout;
typedef char data_ov001_020ab604_size[sizeof(data_ov001_020ab604_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov001_020ab608_Layout;
typedef char data_ov001_020ab608_size[sizeof(data_ov001_020ab608_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov001_020ab60c_Layout;
typedef char data_ov001_020ab60c_size[sizeof(data_ov001_020ab60c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov001_020ab610_Layout;
typedef char data_ov001_020ab610_size[sizeof(data_ov001_020ab610_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov001_020ab618_Layout;
typedef char data_ov001_020ab618_size[sizeof(data_ov001_020ab618_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
} data_ov001_020ab620_Layout;
typedef char data_ov001_020ab620_size[sizeof(data_ov001_020ab620_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
} data_ov001_020ab630_Layout;
typedef char data_ov001_020ab630_size[sizeof(data_ov001_020ab630_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[28];
} data_ov001_020ab640_Layout;
typedef char data_ov001_020ab640_size[sizeof(data_ov001_020ab640_Layout) == 0x70 ? 1 : -1];

typedef struct {
    u32 w[77];
} data_ov001_020ab6b0_Layout;
typedef char data_ov001_020ab6b0_size[sizeof(data_ov001_020ab6b0_Layout) == 0x134 ? 1 : -1];

extern const data_ov001_020ab5f8_Layout data_ov001_020ab5f8;
extern const data_ov001_020ab5fc_Layout data_ov001_020ab5fc;
extern const data_ov001_020ab600_Layout data_ov001_020ab600;
extern const data_ov001_020ab604_Layout data_ov001_020ab604;
extern const data_ov001_020ab608_Layout data_ov001_020ab608;
extern const data_ov001_020ab60c_Layout data_ov001_020ab60c;
extern const data_ov001_020ab610_Layout data_ov001_020ab610;
extern const data_ov001_020ab618_Layout data_ov001_020ab618;
extern const data_ov001_020ab620_Layout data_ov001_020ab620;
extern const data_ov001_020ab630_Layout data_ov001_020ab630;
extern const data_ov001_020ab640_Layout data_ov001_020ab640;
extern const data_ov001_020ab6b0_Layout data_ov001_020ab6b0;

const data_ov001_020ab600_Layout data_ov001_020ab600 = {{
    0x00000000,
}};

const data_ov001_020ab604_Layout data_ov001_020ab604 = {{
    0x00000000,
}};

const data_ov001_020ab608_Layout data_ov001_020ab608 = {{
    0x00000000,
}};

const data_ov001_020ab5f8_Layout data_ov001_020ab5f8 = {{
    0x00000000,
}};

const data_ov001_020ab5fc_Layout data_ov001_020ab5fc = {{
    0x00000000,
}};

const data_ov001_020ab60c_Layout data_ov001_020ab60c = {{
    0x00000000,
}};

const data_ov001_020ab610_Layout data_ov001_020ab610 = {
    0x00000001,
    &data_ov001_020ab630,
};

const data_ov001_020ab618_Layout data_ov001_020ab618 = {
    0x00000001,
    &data_ov001_020ab620,
};

const data_ov001_020ab630_Layout data_ov001_020ab630 = {
    0x00000001,
    &data_ov001_020ab600,
    0x00000134,
    &data_ov001_020ab6b0,
};

const data_ov001_020ab620_Layout data_ov001_020ab620 = {
    0x00000001,
    &data_ov001_020ab604,
    0x00000070,
    &data_ov001_020ab640,
};

const data_ov001_020ab640_Layout data_ov001_020ab640 = {{
    0x24201440, 0x00000003, 0x00000000, 0x00000000, 0x3fcfe3fc, 0x26262726, 0x0100ff00,
    0xff000000, 0x0100ff00, 0xff000100, 0x26262726, 0x01000100, 0xff00fe00, 0x01000100,
    0xff00ff00, 0x27404126, 0x0100ff00, 0x00000001, 0xff000000, 0x26252525, 0x00000100,
    0xfe000100, 0xfe00ff00, 0x0100ff00, 0x41252525, 0xfe000100, 0x00000100, 0x0000ff00,
}};

const data_ov001_020ab6b0_Layout data_ov001_020ab6b0 = {{
    0x23201440, 0x00000003, 0x00000000, 0x00000000, 0xfe0000b5, 0x0000ff4b, 0x28252425, 0x000000b5,
    0x000fe004, 0x00000100, 0x0b5803b5, 0x28252425, 0x000000b5, 0x004fe000, 0x00000000, 0x3b58034b,
    0x28252425, 0x0000ff4b, 0x000fe3fc, 0x0000ff00, 0x34b8004b, 0x28252425, 0x0000ff4b, 0x3fcfe000,
    0x00000000, 0x04b800b5, 0x24404125, 0x000000b5, 0x00000002, 0x3fcfe000, 0x26252626, 0xff4b00b5,
    0x00000000, 0xfe000100, 0x00b500b5, 0x26274041, 0x00000002, 0xff4b0000, 0xff000000, 0x41262626,
    0x00000000, 0xff4bff4b, 0x0000ff00, 0x26262540, 0x00000002, 0x0000ff00, 0x00b5ff4b, 0x00000000,
    0x40412626, 0x01000000, 0x00b500b5, 0x00000002, 0x26262624, 0x3fcfe000, 0x00000000, 0xff4bff4b,
    0x0000ff00, 0x25254041, 0x00000002, 0xfe00ff00, 0xfe000000, 0x40412626, 0x00b5ff4b, 0x01000000,
    0x00000002, 0x26252623, 0x000000b5, 0x000000b5, 0x00000100, 0x00000000, 0xff4b00b5, 0x26274041,
    0x00000000, 0x00b5fe00, 0x01000000, 0x00004126, 0x00000000,
}};
