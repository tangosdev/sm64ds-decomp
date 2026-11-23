#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov035_02112434_Layout;
typedef char data_ov035_02112434_size[sizeof(data_ov035_02112434_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov035_02112438_Layout;
typedef char data_ov035_02112438_size[sizeof(data_ov035_02112438_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov035_0211244c_Layout;
typedef char data_ov035_0211244c_size[sizeof(data_ov035_0211244c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov035_02112464_Layout;
typedef char data_ov035_02112464_size[sizeof(data_ov035_02112464_Layout) == 0x30 ? 1 : -1];

extern data_ov035_02112434_Layout data_ov035_02112434;
extern data_ov035_02112438_Layout data_ov035_02112438;
extern data_ov035_0211244c_Layout data_ov035_0211244c;
extern data_ov035_02112464_Layout data_ov035_02112464;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_02112434_Layout data_ov035_02112434 = {{
    0xfc18e225,
}};

data_ov035_02112438_Layout data_ov035_02112438 = {{
    0x05dcfccc,
    0xfc7ce225,
    0x0640fccc,
    0x00c8c226,
    0x0780ef98,
}};

data_ov035_0211244c_Layout data_ov035_0211244c = {{
    0x031dc226,
    0x0780efca,
    0x00c8c226,
    0x0780f218,
    0x031dc226,
    0x0780f254,
}};

data_ov035_02112464_Layout data_ov035_02112464 = {{
    0x00c8c226,
    0x0780f4a2,
    0x031dc226,
    0x0780f4de,
    0x0513c226,
    0x0588f35d,
    0x06b9c226,
    0x03e2f35d,
    0x06a40c3f,
    0x02bcedcc,
    0x00002227,
    0xfaecffec,
}};
