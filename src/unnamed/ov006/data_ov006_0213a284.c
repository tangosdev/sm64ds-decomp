#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
    u32 f00c;
    void *f010;
    u32 f014;
    void *f018;
    u32 f01c;
    void *f020;
    u32 f024;
    void *f028;
    u32 f02c;
    void *f030;
    u32 f034;
    void *f038;
    u32 f03c;
    void *f040;
    u32 f044;
} data_ov006_0213a284_Layout;
typedef char data_ov006_0213a284_size[sizeof(data_ov006_0213a284_Layout) == 0x48 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
    u32 f00c;
    void *f010;
    u32 f014;
    void *f018;
    u32 f01c;
    void *f020;
    u32 f024;
    void *f028;
    u32 f02c;
    void *f030;
    u32 f034;
    void *f038;
    u32 f03c;
    void *f040;
    u32 f044;
    void *f048;
    u32 f04c;
    void *f050;
    u32 f054;
    void *f058;
    u32 f05c;
} data_ov006_0213a2cc_Layout;
typedef char data_ov006_0213a2cc_size[sizeof(data_ov006_0213a2cc_Layout) == 0x60 ? 1 : -1];

extern data_ov006_0213a284_Layout data_ov006_0213a284;
extern data_ov006_0213a2cc_Layout data_ov006_0213a2cc;
extern u8 data_ov006_02139e2c[];
extern u8 data_ov006_0213a18c[];
extern u8 data_ov006_02139f4c[];
extern u8 data_ov006_02139f7c[];
extern u8 data_ov006_02139fac[];
extern u8 data_ov006_02139eec[];
extern u8 data_ov006_02139f1c[];

#pragma define_section sName ".data.gap38"                                                         \
                             ".data.gap38"                                                         \
                             ".data.gap38" far_data
#pragma section sName begin

data_ov006_0213a284_Layout data_ov006_0213a284 = {
    &data_ov006_02139e2c[0], 0x00000003, &data_ov006_0213a18c[0], 0x00000003,
    &data_ov006_02139f4c[0], 0x00000003, &data_ov006_02139f7c[0], 0x00000005,
    &data_ov006_02139fac[0], 0x00000014, &data_ov006_02139eec[0], 0x00000005,
    &data_ov006_02139f1c[0], 0x00000005, &data_ov006_02139eec[0], 0x00000005,
    &data_ov006_02139fac[0], 0x00000005,
};

data_ov006_0213a2cc_Layout data_ov006_0213a2cc = {
    &data_ov006_02139eec[0], 0x00000005, &data_ov006_02139f1c[0], 0x00000005,
    &data_ov006_02139eec[0], 0x00000005, &data_ov006_02139fac[0], 0x00000014,
    &data_ov006_02139eec[0], 0x00000005, &data_ov006_02139f1c[0], 0x00000005,
    &data_ov006_02139eec[0], 0x00000005, &data_ov006_02139fac[0], 0x00000005,
    &data_ov006_02139eec[0], 0x00000005, &data_ov006_02139f1c[0], 0x00000005,
    &data_ov006_02139eec[0], 0x00000005, &data_ov006_02139fac[0], 0x00000000,
};
