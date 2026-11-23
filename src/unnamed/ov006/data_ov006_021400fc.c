#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov006_021400fc_Layout;
typedef char data_ov006_021400fc_size[sizeof(data_ov006_021400fc_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov006_02140108_Layout;
typedef char data_ov006_02140108_size[sizeof(data_ov006_02140108_Layout) == 0xc ? 1 : -1];

extern data_ov006_021400fc_Layout data_ov006_021400fc;
extern data_ov006_02140108_Layout data_ov006_02140108;

data_ov006_021400fc_Layout data_ov006_021400fc = {{
    0x2f474d2f,
    0x776f6e73,
    0x5f6e616d,
}};

data_ov006_02140108_Layout data_ov006_02140108 = {{
    0x64616568,
    0x646d622e,
    0x00000000,
}};
