#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov045_02112460_Layout;
typedef char data_ov045_02112460_size[sizeof(data_ov045_02112460_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov045_02112464_Layout;
typedef char data_ov045_02112464_size[sizeof(data_ov045_02112464_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov045_02112468_Layout;
typedef char data_ov045_02112468_size[sizeof(data_ov045_02112468_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov045_02112470_Layout;
typedef char data_ov045_02112470_size[sizeof(data_ov045_02112470_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov045_02112478_Layout;
typedef char data_ov045_02112478_size[sizeof(data_ov045_02112478_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov045_02112480_Layout;
typedef char data_ov045_02112480_size[sizeof(data_ov045_02112480_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov045_02112488_Layout;
typedef char data_ov045_02112488_size[sizeof(data_ov045_02112488_Layout) == 0x8 ? 1 : -1];

extern data_ov045_02112460_Layout data_ov045_02112460;
extern data_ov045_02112464_Layout data_ov045_02112464;
extern data_ov045_02112468_Layout data_ov045_02112468;
extern data_ov045_02112470_Layout data_ov045_02112470;
extern data_ov045_02112478_Layout data_ov045_02112478;
extern data_ov045_02112480_Layout data_ov045_02112480;
extern data_ov045_02112488_Layout data_ov045_02112488;
extern u8 data_ov045_021124ac[];
extern u8 data_ov045_0211265c[];

#pragma define_section sName ".data.w12460"                                                        \
                             ".data.w12460"                                                        \
                             ".data.w12460" far_data
#pragma section sName begin

data_ov045_02112464_Layout data_ov045_02112464 = {{
    0x00007fff,
}};

data_ov045_02112460_Layout data_ov045_02112460 = {{
    0xff0a0000,
}};

data_ov045_02112470_Layout data_ov045_02112470 = {
    0x00000002,
    &data_ov045_021124ac[0],
};

data_ov045_02112478_Layout data_ov045_02112478 = {{
    0x077c077b,
    0x077e077d,
}};

data_ov045_02112468_Layout data_ov045_02112468 = {{
    0x07d007d0,
    0x07d007d0,
}};

data_ov045_02112480_Layout data_ov045_02112480 = {
    0x00000007,
    &data_ov045_0211265c[0],
};

data_ov045_02112488_Layout data_ov045_02112488 = {{
    0x61676f79,
    0x0030306e,
}};
