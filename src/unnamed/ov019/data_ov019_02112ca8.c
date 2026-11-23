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
} data_ov019_02112ca8_Layout;
typedef char data_ov019_02112ca8_size[sizeof(data_ov019_02112ca8_Layout) == 0x1c ? 1 : -1];

extern data_ov019_02112ca8_Layout data_ov019_02112ca8;
extern u8 data_ov019_02112cc4[];
extern u8 data_ov019_02112bdc[];
extern u8 data_ov019_02112b84[];
extern u8 data_ov019_02112bf8[];

data_ov019_02112ca8_Layout data_ov019_02112ca8 = {
    &data_ov019_02112cc4[0], 0x00000109, &data_ov019_02112bdc[0], 0x0000010e,
    &data_ov019_02112b84[0], 0x0000010a, &data_ov019_02112bf8[0],
};
