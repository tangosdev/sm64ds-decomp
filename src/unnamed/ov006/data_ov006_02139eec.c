#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[12];
} data_ov006_02139eec_Layout;
typedef char data_ov006_02139eec_size[sizeof(data_ov006_02139eec_Layout) == 0x30 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov006_02139f1c_Layout;
typedef char data_ov006_02139f1c_size[sizeof(data_ov006_02139f1c_Layout) == 0x30 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov006_02139f4c_Layout;
typedef char data_ov006_02139f4c_size[sizeof(data_ov006_02139f4c_Layout) == 0x30 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov006_02139f7c_Layout;
typedef char data_ov006_02139f7c_size[sizeof(data_ov006_02139f7c_Layout) == 0x30 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov006_02139fac_Layout;
typedef char data_ov006_02139fac_size[sizeof(data_ov006_02139fac_Layout) == 0x30 ? 1 : -1];

extern data_ov006_02139eec_Layout data_ov006_02139eec;
extern data_ov006_02139f1c_Layout data_ov006_02139f1c;
extern data_ov006_02139f4c_Layout data_ov006_02139f4c;
extern data_ov006_02139f7c_Layout data_ov006_02139f7c;
extern data_ov006_02139fac_Layout data_ov006_02139fac;

#pragma define_section sName ".data.gap37"                                                         \
                             ".data.gap37"                                                         \
                             ".data.gap37" far_data
#pragma section sName begin

data_ov006_02139f7c_Layout data_ov006_02139f7c = {{
    0x81df00eb,
    0x0000340c,
    0x81e300d4,
    0x00000510,
    0x800380d4,
    0x00000514,
    0x81e340f4,
    0x00000590,
    0x400300f4,
    0x00000594,
    0x800000ea,
    0xffff251c,
}};

data_ov006_02139f4c_Layout data_ov006_02139f4c = {{
    0x81de00ea,
    0x00003408,
    0x81e300d4,
    0x00000510,
    0x800380d4,
    0x00000514,
    0x81e340f4,
    0x00000590,
    0x400300f4,
    0x00000594,
    0x800000ea,
    0xffff251c,
}};

data_ov006_02139f1c_Layout data_ov006_02139f1c = {{
    0x81e000ed,
    0x0000341c,
    0x81e300d4,
    0x00000510,
    0x800380d4,
    0x00000514,
    0x81e340f4,
    0x00000590,
    0x400300f4,
    0x00000594,
    0x800000ea,
    0xffff251c,
}};

data_ov006_02139eec_Layout data_ov006_02139eec = {{
    0x81e100ee,
    0x00003498,
    0x81e300d4,
    0x00000510,
    0x800380d4,
    0x00000514,
    0x81e340f4,
    0x00000590,
    0x400300f4,
    0x00000594,
    0x800000ea,
    0xffff251c,
}};

data_ov006_02139fac_Layout data_ov006_02139fac = {{
    0x81e100eb,
    0x00003418,
    0x81e300d4,
    0x00000510,
    0x800380d4,
    0x00000514,
    0x81e340f4,
    0x00000590,
    0x400300f4,
    0x00000594,
    0x800000ea,
    0xffff251c,
}};
