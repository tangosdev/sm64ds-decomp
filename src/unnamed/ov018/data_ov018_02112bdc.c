#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov018_02112bdc_Layout;
typedef char data_ov018_02112bdc_size[sizeof(data_ov018_02112bdc_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov018_02112bec_Layout;
typedef char data_ov018_02112bec_size[sizeof(data_ov018_02112bec_Layout) == 0x18 ? 1 : -1];

extern const data_ov018_02112bdc_Layout data_ov018_02112bdc;
extern const data_ov018_02112bec_Layout data_ov018_02112bec;

const data_ov018_02112bdc_Layout data_ov018_02112bdc = {{
    0x00000e67,
    0x00000e95,
    0x00000ec2,
    0x00000ef0,
}};

const data_ov018_02112bec_Layout data_ov018_02112bec = {{
    0x00000f1d,
    0x00000f4a,
    0x00000f78,
    0x00000fa5,
    0x00000fd3,
    0x00001000,
}};
