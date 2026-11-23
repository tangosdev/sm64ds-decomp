#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov033_02111b6c_Layout;
typedef char data_ov033_02111b6c_size[sizeof(data_ov033_02111b6c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov033_02111b74_Layout;
typedef char data_ov033_02111b74_size[sizeof(data_ov033_02111b74_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov033_02111b7c_Layout;
typedef char data_ov033_02111b7c_size[sizeof(data_ov033_02111b7c_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov033_02111b8c_Layout;
typedef char data_ov033_02111b8c_size[sizeof(data_ov033_02111b8c_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov033_02111b9c_Layout;
typedef char data_ov033_02111b9c_size[sizeof(data_ov033_02111b9c_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
    void *f00c;
    u32 f010;
    void *f014;
} data_ov033_02111bb0_Layout;
typedef char data_ov033_02111bb0_size[sizeof(data_ov033_02111bb0_Layout) == 0x18 ? 1 : -1];

extern data_ov033_02111b6c_Layout data_ov033_02111b6c;
extern data_ov033_02111b74_Layout data_ov033_02111b74;
extern data_ov033_02111b7c_Layout data_ov033_02111b7c;
extern data_ov033_02111b8c_Layout data_ov033_02111b8c;
extern data_ov033_02111b9c_Layout data_ov033_02111b9c;
extern data_ov033_02111bb0_Layout data_ov033_02111bb0;
extern u8 data_ov033_021116d4[];
extern u8 data_ov033_021116cc[];
extern u8 data_ov033_02111710[];
extern u8 data_ov033_021116f4[];

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov033_02111b74_Layout data_ov033_02111b74 = {{
    0x00000000,
    0xf0020000,
}};

data_ov033_02111b6c_Layout data_ov033_02111b6c = {{
    0x012c00fc,
    0x076cfe70,
}};

data_ov033_02111b8c_Layout data_ov033_02111b8c = {{
    0x012c00fc,
    0x076cfe70,
    0x00000000,
    0xf0020000,
}};

data_ov033_02111b7c_Layout data_ov033_02111b7c = {{
    0x012c00fc,
    0x076cfe70,
    0x00000000,
    0xf0020000,
}};

data_ov033_02111b9c_Layout data_ov033_02111b9c = {{
    0xff090000,
    0x00097fff,
    0xffffff04,
    0xff04000d,
    0x0000ffff,
}};

data_ov033_02111bb0_Layout data_ov033_02111bb0 = {
    0x0000005b, &data_ov033_021116d4[0], &data_ov033_021116cc[0], &data_ov033_02111710[0],
    0x00000001, &data_ov033_021116f4[0],
};
