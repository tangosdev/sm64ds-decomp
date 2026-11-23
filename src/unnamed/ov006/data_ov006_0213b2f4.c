#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} sTrmpLinePushStrongPitchMid_Layout;
typedef char
    sTrmpLinePushStrongPitchMid_size[sizeof(sTrmpLinePushStrongPitchMid_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmpLineStrongPitchEnd_Layout;
typedef char sTrmpLineStrongPitchEnd_size[sizeof(sTrmpLineStrongPitchEnd_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmpLineWeakPitchEnd_Layout;
typedef char sTrmpLineWeakPitchEnd_size[sizeof(sTrmpLineWeakPitchEnd_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmpLineWeakPitchMid_Layout;
typedef char sTrmpLineWeakPitchMid_size[sizeof(sTrmpLineWeakPitchMid_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmpLineStrongPitchMid_Layout;
typedef char sTrmpLineStrongPitchMid_size[sizeof(sTrmpLineStrongPitchMid_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmpLinePushStrongPitchEnd_Layout;
typedef char
    sTrmpLinePushStrongPitchEnd_size[sizeof(sTrmpLinePushStrongPitchEnd_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmpLinePushWeakPitchMid_Layout;
typedef char
    sTrmpLinePushWeakPitchMid_size[sizeof(sTrmpLinePushWeakPitchMid_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmpLinePushWeakPitchEnd_Layout;
typedef char
    sTrmpLinePushWeakPitchEnd_size[sizeof(sTrmpLinePushWeakPitchEnd_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0213b314_Layout;
typedef char data_ov006_0213b314_size[sizeof(data_ov006_0213b314_Layout) == 0x8 ? 1 : -1];

extern sTrmpLinePushStrongPitchMid_Layout sTrmpLinePushStrongPitchMid;
extern sTrmpLineStrongPitchEnd_Layout sTrmpLineStrongPitchEnd;
extern sTrmpLineWeakPitchEnd_Layout sTrmpLineWeakPitchEnd;
extern sTrmpLineWeakPitchMid_Layout sTrmpLineWeakPitchMid;
extern sTrmpLineStrongPitchMid_Layout sTrmpLineStrongPitchMid;
extern sTrmpLinePushStrongPitchEnd_Layout sTrmpLinePushStrongPitchEnd;
extern sTrmpLinePushWeakPitchMid_Layout sTrmpLinePushWeakPitchMid;
extern sTrmpLinePushWeakPitchEnd_Layout sTrmpLinePushWeakPitchEnd;
extern data_ov006_0213b314_Layout data_ov006_0213b314;

#pragma define_section sName ".data.gap50"                                                         \
                             ".data.gap50"                                                         \
                             ".data.gap50" far_data
#pragma section sName begin

sTrmpLinePushWeakPitchEnd_Layout sTrmpLinePushWeakPitchEnd = {{
    0xfffffeca,
}};

sTrmpLinePushWeakPitchMid_Layout sTrmpLinePushWeakPitchMid = {{
    0x00000118,
}};

sTrmpLinePushStrongPitchEnd_Layout sTrmpLinePushStrongPitchEnd = {{
    0xfffffeca,
}};

sTrmpLineStrongPitchMid_Layout sTrmpLineStrongPitchMid = {{
    0x00000118,
}};

sTrmpLineWeakPitchMid_Layout sTrmpLineWeakPitchMid = {{
    0x00000118,
}};

sTrmpLineWeakPitchEnd_Layout sTrmpLineWeakPitchEnd = {{
    0xfffffeca,
}};

sTrmpLineStrongPitchEnd_Layout sTrmpLineStrongPitchEnd = {{
    0xfffffeca,
}};

sTrmpLinePushStrongPitchMid_Layout sTrmpLinePushStrongPitchMid = {{
    0x00000118,
}};

data_ov006_0213b314_Layout data_ov006_0213b314 = {{
    0x676e6972,
    0x00000000,
}};
