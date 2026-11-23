#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
} data_ov045_021124bc_Layout;
typedef char data_ov045_021124bc_size[sizeof(data_ov045_021124bc_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    void *f00c;
} data_ov045_021124c4_Layout;
typedef char data_ov045_021124c4_size[sizeof(data_ov045_021124c4_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
    u32 f014;
    u32 f018;
} data_ov045_021124d4_Layout;
typedef char data_ov045_021124d4_size[sizeof(data_ov045_021124d4_Layout) == 0x1c ? 1 : -1];

extern data_ov045_021124bc_Layout data_ov045_021124bc;
extern data_ov045_021124c4_Layout data_ov045_021124c4;
extern data_ov045_021124d4_Layout data_ov045_021124d4;
extern u8 data_ov045_02111e64[];
extern u8 data_ov045_02111e60[];
extern u8 data_ov045_02111e84[];
extern u8 data_ov045_02111e68[];
extern u8 data_ov045_02112720[];
extern u8 data_ov045_02112480[];
extern u8 data_ov045_02112490[];

#pragma define_section sName ".data.w12460"                                                        \
                             ".data.w12460"                                                        \
                             ".data.w12460" far_data
#pragma section sName begin

data_ov045_021124bc_Layout data_ov045_021124bc = {
    0x000000c8,
    &data_ov045_02111e64[0],
};

data_ov045_021124c4_Layout data_ov045_021124c4 = {
    &data_ov045_02111e60[0],
    &data_ov045_02111e84[0],
    0x00000001,
    &data_ov045_02111e68[0],
};

data_ov045_021124d4_Layout data_ov045_021124d4 = {
    &data_ov045_02112720[0],
    &data_ov045_02112480[0],
    0x077a0782,
    0x07840783,
    &data_ov045_02112490[0],
    0x06400001,
    0x0000007f,
};
