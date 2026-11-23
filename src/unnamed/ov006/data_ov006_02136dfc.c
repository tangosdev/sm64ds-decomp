#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_02136dfc_Layout;
typedef char data_ov006_02136dfc_size[sizeof(data_ov006_02136dfc_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02136e04_Layout;
typedef char data_ov006_02136e04_size[sizeof(data_ov006_02136e04_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
} sPachinkoTargetSprites_Layout;
typedef char sPachinkoTargetSprites_size[sizeof(sPachinkoTargetSprites_Layout) == 0x8 ? 1 : -1];

extern data_ov006_02136dfc_Layout data_ov006_02136dfc;
extern data_ov006_02136e04_Layout data_ov006_02136e04;
extern sPachinkoTargetSprites_Layout sPachinkoTargetSprites;

#pragma define_section sName ".data.gap22"                                                         \
                             ".data.gap22"                                                         \
                             ".data.gap22" far_data
#pragma section sName begin

data_ov006_02136e04_Layout data_ov006_02136e04 = {{
    0x81f000f0,
    0xffff1054,
}};

data_ov006_02136dfc_Layout data_ov006_02136dfc = {{
    0x41f04008,
    0xffff10d4,
}};

sPachinkoTargetSprites_Layout sPachinkoTargetSprites = {
    &data_ov006_02136e04,
    &data_ov006_02136dfc,
};
