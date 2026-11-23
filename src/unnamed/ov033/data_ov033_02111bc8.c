#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    void *f008;
} data_ov033_02111bc8_Layout;
typedef char data_ov033_02111bc8_size[sizeof(data_ov033_02111bc8_Layout) == 0xc ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
} data_ov033_02111bd4_Layout;
typedef char data_ov033_02111bd4_size[sizeof(data_ov033_02111bd4_Layout) == 0xc ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
    u32 f014;
    u32 f018;
} data_ov033_02111be0_Layout;
typedef char data_ov033_02111be0_size[sizeof(data_ov033_02111be0_Layout) == 0x1c ? 1 : -1];

extern data_ov033_02111bc8_Layout data_ov033_02111bc8;
extern data_ov033_02111bd4_Layout data_ov033_02111bd4;
extern data_ov033_02111be0_Layout data_ov033_02111be0;
extern u8 data_ov033_021116d0[];
extern u8 data_ov033_021116c8[];
extern u8 data_ov033_0211187c[];
extern u8 data_ov033_021116d8[];
extern u8 data_ov033_02111e44[];
extern u8 data_ov033_02111aec[];
extern u8 data_ov033_02111b20[];

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov033_02111bd4_Layout data_ov033_02111bd4 = {
    &data_ov033_0211187c[0],
    0x00000001,
    &data_ov033_021116d8[0],
};

data_ov033_02111bc8_Layout data_ov033_02111bc8 = {
    0x0000005b,
    &data_ov033_021116d0[0],
    &data_ov033_021116c8[0],
};

data_ov033_02111be0_Layout data_ov033_02111be0 = {
    &data_ov033_02111e44[0],
    &data_ov033_02111aec[0],
    0x07e807ea,
    0x07ec07eb,
    &data_ov033_02111b20[0],
    0x03e80001,
    0x0000001f,
};
