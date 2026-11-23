#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov058_02111760_Layout;
typedef char data_ov058_02111760_size[sizeof(data_ov058_02111760_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
    u32 f014;
    u32 f018;
} data_ov058_02111768_Layout;
typedef char data_ov058_02111768_size[sizeof(data_ov058_02111768_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov058_02111784_Layout;
typedef char data_ov058_02111784_size[sizeof(data_ov058_02111784_Layout) == 0x20 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    void *f00c;
    u32 f010;
    u32 f014;
    void *f018;
    u32 f01c;
    u32 f020;
} data_ov058_021117a4_Layout;
typedef char data_ov058_021117a4_size[sizeof(data_ov058_021117a4_Layout) == 0x24 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov058_021117c8_Layout;
typedef char data_ov058_021117c8_size[sizeof(data_ov058_021117c8_Layout) == 0x30 ? 1 : -1];

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
    void *f024;
    u32 f028;
    void *f02c;
    u32 f030;
    void *f034;
} data_ov058_021117f8_Layout;
typedef char data_ov058_021117f8_size[sizeof(data_ov058_021117f8_Layout) == 0x38 ? 1 : -1];

extern data_ov058_02111760_Layout data_ov058_02111760;
extern data_ov058_02111768_Layout data_ov058_02111768;
extern data_ov058_02111784_Layout data_ov058_02111784;
extern data_ov058_021117a4_Layout data_ov058_021117a4;
extern data_ov058_021117c8_Layout data_ov058_021117c8;
extern data_ov058_021117f8_Layout data_ov058_021117f8;
extern u8 data_ov058_02111920[];
extern u8 data_ov058_0211170c[];
extern u8 data_ov058_02111734[];
extern u8 data_ov058_02111714[];
extern u8 data_ov058_0211173c[];
extern u8 data_ov058_02111754[];
extern u8 data_ov058_02111704[];
extern u8 data_ov058_02111830[];
extern u8 data_ov058_02111700[];
extern u8 data_ov058_021118c0[];
extern u8 data_ov058_021119a0[];
extern u8 data_ov058_02111878[];

data_ov058_02111760_Layout data_ov058_02111760 = {{
    0x025810ca,
    0x0000ee06,
}};

data_ov058_02111768_Layout data_ov058_02111768 = {
    &data_ov058_02111920[0],
    &data_ov058_0211170c[0],
    0x07a407a6,
    0x07a807a7,
    &data_ov058_021117a4,
    0x06400003,
    0x0000000f,
};

data_ov058_02111784_Layout data_ov058_02111784 = {{
    0x1372002a,
    0xf00202bc,
    0xa0000000,
    0x0e220000,
    0x132400ed,
    0xeda40258,
    0xd5560000,
    0x12000000,
}};

data_ov058_021117a4_Layout data_ov058_021117a4 = {
    &data_ov058_02111734[0], 0x00000000, 0x06400100,
    &data_ov058_02111714[0], 0x00000000, 0x06400100,
    &data_ov058_0211173c[0], 0x00000000, 0x06400100,
};

data_ov058_021117c8_Layout data_ov058_021117c8 = {{
    0x0c1e0050,
    0xf0c40258,
    0xe000ffff,
    0x0b060000,
    0x0da8004a,
    0xef3d028a,
    0xe0000000,
    0xffff0000,
    0x09ee002a,
    0xeea402bc,
    0x20000000,
    0x12220000,
}};

data_ov058_021117f8_Layout data_ov058_021117f8 = {
    0x00000302, &data_ov058_02111754[0], 0x00000103, &data_ov058_02111704[0],
    0x00000504, &data_ov058_02111830[0], 0x0000010b, &data_ov058_02111700[0],
    0x00000601, &data_ov058_021118c0[0], 0x00000c09, &data_ov058_021119a0[0],
    0x0000050a, &data_ov058_02111878[0],
};
