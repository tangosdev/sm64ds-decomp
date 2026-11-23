#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov035_021124c4_Layout;
typedef char data_ov035_021124c4_size[sizeof(data_ov035_021124c4_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov035_021124d4_Layout;
typedef char data_ov035_021124d4_size[sizeof(data_ov035_021124d4_Layout) == 0x10 ? 1 : -1];

extern data_ov035_021124c4_Layout data_ov035_021124c4;
extern data_ov035_021124d4_Layout data_ov035_021124d4;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_021124c4_Layout data_ov035_021124c4 = {{
    0xfae2063d,
    0xfae2f786,
    0x01c22227,
    0xf8f8ffec,
}};

data_ov035_021124d4_Layout data_ov035_021124d4 = {{
    0x01c22227,
    0xf8f8ffec,
    0xfc4ae43c,
    0xffb000fa,
}};
