#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov033_021120f8_Layout;
typedef char data_ov033_021120f8_size[sizeof(data_ov033_021120f8_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov033_02112104_Layout;
typedef char data_ov033_02112104_size[sizeof(data_ov033_02112104_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov033_02112118_Layout;
typedef char data_ov033_02112118_size[sizeof(data_ov033_02112118_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov033_02112138_Layout;
typedef char data_ov033_02112138_size[sizeof(data_ov033_02112138_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov033_02112158_Layout;
typedef char data_ov033_02112158_size[sizeof(data_ov033_02112158_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov033_02112178_Layout;
typedef char data_ov033_02112178_size[sizeof(data_ov033_02112178_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov033_02112198_Layout;
typedef char data_ov033_02112198_size[sizeof(data_ov033_02112198_Layout) == 0x20 ? 1 : -1];

extern data_ov033_021120f8_Layout data_ov033_021120f8;
extern data_ov033_02112104_Layout data_ov033_02112104;
extern data_ov033_02112118_Layout data_ov033_02112118;
extern data_ov033_02112138_Layout data_ov033_02112138;
extern data_ov033_02112158_Layout data_ov033_02112158;
extern data_ov033_02112178_Layout data_ov033_02112178;
extern data_ov033_02112198_Layout data_ov033_02112198;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_021120f8_Layout data_ov033_021120f8 = {{
    0xff000000,
    0xff9c0019,
    0xf704fc4a,
}};

data_ov033_02112104_Layout data_ov033_02112104 = {{
    0x00000000,
    0xff000000,
    0xff9c0019,
    0xf63cfc4a,
    0x00000000,
}};

data_ov033_02112138_Layout data_ov033_02112138 = {{
    0xff000000,
    0xff9c0019,
    0xf3e4fc4a,
    0x00000000,
    0xff000000,
    0xff9c0019,
    0xf31cfc4a,
    0x00000000,
}};

data_ov033_02112158_Layout data_ov033_02112158 = {{
    0xff000000,
    0xff9c0019,
    0xf254fc4a,
    0x00000000,
    0xff000000,
    0x00640019,
    0xf7ccfc4a,
    0x00000000,
}};

data_ov033_02112118_Layout data_ov033_02112118 = {{
    0xff000000,
    0xff9c0019,
    0xf574fc4a,
    0x00000000,
    0xff000000,
    0xff9c0019,
    0xf4acfc4a,
    0x00000000,
}};

data_ov033_02112178_Layout data_ov033_02112178 = {{
    0xff000000,
    0x00640019,
    0xf704fc4a,
    0x00000000,
    0xff000000,
    0x00640019,
    0xf63cfc4a,
    0x00000000,
}};

data_ov033_02112198_Layout data_ov033_02112198 = {{
    0xff000000,
    0x00640019,
    0xf574fc4a,
    0x00000000,
    0xff000000,
    0x00640019,
    0xf4acfc4a,
    0x00000000,
}};
