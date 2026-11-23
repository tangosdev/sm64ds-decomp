#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} sCurling2StepXPicks_Layout;
typedef char sCurling2StepXPicks_size[sizeof(sCurling2StepXPicks_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sCurling2StepXFastPicks_Layout;
typedef char sCurling2StepXFastPicks_size[sizeof(sCurling2StepXFastPicks_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} sCurling2StepModes_Layout;
typedef char sCurling2StepModes_size[sizeof(sCurling2StepModes_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_0212e504_Layout;
typedef char data_ov006_0212e504_size[sizeof(data_ov006_0212e504_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov006_0212e51c_Layout;
typedef char data_ov006_0212e51c_size[sizeof(data_ov006_0212e51c_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[8];
} sCurling2ScrollX_Layout;
typedef char sCurling2ScrollX_size[sizeof(sCurling2ScrollX_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} sCurling2ScrollY_Layout;
typedef char sCurling2ScrollY_size[sizeof(sCurling2ScrollY_Layout) == 0x20 ? 1 : -1];

extern sCurling2StepXPicks_Layout sCurling2StepXPicks;
extern sCurling2StepXFastPicks_Layout sCurling2StepXFastPicks;
extern sCurling2StepModes_Layout sCurling2StepModes;
extern data_ov006_0212e504_Layout data_ov006_0212e504;
extern data_ov006_0212e51c_Layout data_ov006_0212e51c;
extern sCurling2ScrollX_Layout sCurling2ScrollX;
extern sCurling2ScrollY_Layout sCurling2ScrollY;

#pragma define_section sName ".rodata.gap0"                                                        \
                             ".rodata.gap0"                                                        \
                             ".rodata.gap0" far_data
#pragma section sName begin

sCurling2StepXPicks_Layout sCurling2StepXPicks = {{
    0x00000201,
}};

sCurling2StepXFastPicks_Layout sCurling2StepXFastPicks = {{
    0x00000201,
}};

sCurling2StepModes_Layout sCurling2StepModes = {{
    0x00000001,
    0x00000002,
}};

data_ov006_0212e51c_Layout data_ov006_0212e51c = {{
    0x0000007c,
    0x0000007c,
    0x0000007c,
    0x00000060,
    0x00000060,
    0x00000044,
}};

data_ov006_0212e504_Layout data_ov006_0212e504 = {{
    0x00000062,
    0x00000080,
    0x0000009e,
    0x00000071,
    0x0000008f,
    0x00000080,
}};

sCurling2ScrollX_Layout sCurling2ScrollX = {{
    0x00000080,
    0x00000080,
    0x00000080,
    0x00000000,
    0x00000000,
    0x00000100,
    0x00000080,
    0x00000100,
}};

sCurling2ScrollY_Layout sCurling2ScrollY = {{
    0x00000060,
    0x00000060,
    0x00000000,
    0x00000060,
    0x00000038,
    0x00000038,
    0x00000000,
    0x00000060,
}};

#pragma section sName end
