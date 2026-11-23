#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} data_ov006_02135018_Layout;
typedef char data_ov006_02135018_size[sizeof(data_ov006_02135018_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02135024_Layout;
typedef char data_ov006_02135024_size[sizeof(data_ov006_02135024_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02135034_Layout;
typedef char data_ov006_02135034_size[sizeof(data_ov006_02135034_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov006_02135044_Layout;
typedef char data_ov006_02135044_size[sizeof(data_ov006_02135044_Layout) == 0x10 ? 1 : -1];

extern data_ov006_02135018_Layout data_ov006_02135018;
extern data_ov006_02135024_Layout data_ov006_02135024;
extern data_ov006_02135034_Layout data_ov006_02135034;
extern data_ov006_02135044_Layout data_ov006_02135044;

#pragma define_section sName ".data.gap18"                                                         \
                             ".data.gap18"                                                         \
                             ".data.gap18" far_data
#pragma section sName begin

data_ov006_02135018_Layout data_ov006_02135018 = {
    &data_ov006_02135034,
    &data_ov006_02135024,
    &data_ov006_02135044,
};

data_ov006_02135024_Layout data_ov006_02135024 = {{
    0x41f800f8,
    0x000010c2,
    0x81f040f8,
    0xffff2122,
}};

data_ov006_02135034_Layout data_ov006_02135034 = {{
    0x41f800f8,
    0x000010c2,
    0x81f040f8,
    0xffff0122,
}};

data_ov006_02135044_Layout data_ov006_02135044 = {{
    0x41f800f8,
    0x000010c2,
    0x81f040f8,
    0xffff1122,
}};
