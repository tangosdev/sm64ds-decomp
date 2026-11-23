#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
} data_ov017_02111af4_Layout;
typedef char data_ov017_02111af4_size[sizeof(data_ov017_02111af4_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov017_02111afc_Layout;
typedef char data_ov017_02111afc_size[sizeof(data_ov017_02111afc_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov017_02111b14_Layout;
typedef char data_ov017_02111b14_size[sizeof(data_ov017_02111b14_Layout) == 0x18 ? 1 : -1];

extern data_ov017_02111af4_Layout data_ov017_02111af4;
extern data_ov017_02111afc_Layout data_ov017_02111afc;
extern data_ov017_02111b14_Layout data_ov017_02111b14;
extern u8 data_ov017_02111a04[];

#pragma define_section sName ".data.ksRest"                                                        \
                             ".data.ksRest"                                                        \
                             ".data.ksRest" far_data
#pragma section sName begin

data_ov017_02111af4_Layout data_ov017_02111af4 = {
    0x0000010e,
    &data_ov017_02111a04[0],
};

data_ov017_02111afc_Layout data_ov017_02111afc = {{
    0x0000001f,
    0x0b5405c6,
    0x00000000,
    0x01010000,
    0x00000124,
    0xf862fea9,
}};

data_ov017_02111b14_Layout data_ov017_02111b14 = {{
    0x00000000,
    0xff010000,
    0x000000b9,
    0x00000809,
    0x00000000,
    0xffff0000,
}};
