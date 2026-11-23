#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} sPachinkoPipeRiseStartFrames_Layout;
typedef char
    sPachinkoPipeRiseStartFrames_size[sizeof(sPachinkoPipeRiseStartFrames_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sPachinkoPipeSinkStartFrames_Layout;
typedef char
    sPachinkoPipeSinkStartFrames_size[sizeof(sPachinkoPipeSinkStartFrames_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} sPachinkoPipeRiseFrames_Layout;
typedef char sPachinkoPipeRiseFrames_size[sizeof(sPachinkoPipeRiseFrames_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} sPachinkoPipeSinkFrames_Layout;
typedef char sPachinkoPipeSinkFrames_size[sizeof(sPachinkoPipeSinkFrames_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0212eb24_Layout;
typedef char data_ov006_0212eb24_size[sizeof(data_ov006_0212eb24_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} sPachinkoPipeKinds_Layout;
typedef char sPachinkoPipeKinds_size[sizeof(sPachinkoPipeKinds_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} sPachinkoPipeIdleFrames_Layout;
typedef char sPachinkoPipeIdleFrames_size[sizeof(sPachinkoPipeIdleFrames_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} sPachinkoPipeIdleTimes_Layout;
typedef char sPachinkoPipeIdleTimes_size[sizeof(sPachinkoPipeIdleTimes_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov006_0212eb44_Layout;
typedef char data_ov006_0212eb44_size[sizeof(data_ov006_0212eb44_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} sPachinkoPipeRiseStops_Layout;
typedef char sPachinkoPipeRiseStops_size[sizeof(sPachinkoPipeRiseStops_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} sPachinkoPipeSinkStops_Layout;
typedef char sPachinkoPipeSinkStops_size[sizeof(sPachinkoPipeSinkStops_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} sPachinkoTargetX_Layout;
typedef char sPachinkoTargetX_size[sizeof(sPachinkoTargetX_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[5];
} sPachinkoWaveColumnX_Layout;
typedef char sPachinkoWaveColumnX_size[sizeof(sPachinkoWaveColumnX_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[6];
} sPachinkoPopSfx_Layout;
typedef char sPachinkoPopSfx_size[sizeof(sPachinkoPopSfx_Layout) == 0x18 ? 1 : -1];

extern sPachinkoPipeRiseStartFrames_Layout sPachinkoPipeRiseStartFrames;
extern sPachinkoPipeSinkStartFrames_Layout sPachinkoPipeSinkStartFrames;
extern sPachinkoPipeRiseFrames_Layout sPachinkoPipeRiseFrames;
extern sPachinkoPipeSinkFrames_Layout sPachinkoPipeSinkFrames;
extern data_ov006_0212eb24_Layout data_ov006_0212eb24;
extern sPachinkoPipeKinds_Layout sPachinkoPipeKinds;
extern sPachinkoPipeIdleFrames_Layout sPachinkoPipeIdleFrames;
extern sPachinkoPipeIdleTimes_Layout sPachinkoPipeIdleTimes;
extern data_ov006_0212eb44_Layout data_ov006_0212eb44;
extern sPachinkoPipeRiseStops_Layout sPachinkoPipeRiseStops;
extern sPachinkoPipeSinkStops_Layout sPachinkoPipeSinkStops;
extern sPachinkoTargetX_Layout sPachinkoTargetX;
extern sPachinkoWaveColumnX_Layout sPachinkoWaveColumnX;
extern sPachinkoPopSfx_Layout sPachinkoPopSfx;

#pragma define_section sName ".rodata.gap3"                                                        \
                             ".rodata.gap3"                                                        \
                             ".rodata.gap3" far_data
#pragma section sName begin

sPachinkoPipeSinkStartFrames_Layout sPachinkoPipeSinkStartFrames = {{
    0x00040506,
}};

sPachinkoPipeRiseStartFrames_Layout sPachinkoPipeRiseStartFrames = {{
    0x00060504,
}};

sPachinkoPipeIdleFrames_Layout sPachinkoPipeIdleFrames = {{
    0x0e0d0c0b,
    0x0c0d0e0f,
}};

sPachinkoPipeRiseFrames_Layout sPachinkoPipeRiseFrames = {{
    0x03020100,
    0x00000102,
}};

sPachinkoPipeSinkFrames_Layout sPachinkoPipeSinkFrames = {{
    0x0a090807,
    0x00000809,
}};

sPachinkoPipeIdleTimes_Layout sPachinkoPipeIdleTimes = {{
    0x02030204,
    0x02030204,
}};

data_ov006_0212eb24_Layout data_ov006_0212eb24 = {{
    0x00000038,
    0x000000c8,
}};

sPachinkoPipeKinds_Layout sPachinkoPipeKinds = {{
    0x00000001,
    0x00000002,
}};

data_ov006_0212eb44_Layout data_ov006_0212eb44 = {{
    0x00000030,
    0x00000070,
    0x000000b0,
}};

sPachinkoPipeSinkStops_Layout sPachinkoPipeSinkStops = {{
    0x0000006c,
    0x000000f0,
    0x0000005c,
    0x000000e0,
}};

sPachinkoTargetX_Layout sPachinkoTargetX = {{
    0x00000038,
    0x00000068,
    0x00000098,
    0x000000c8,
}};

sPachinkoPipeRiseStops_Layout sPachinkoPipeRiseStops = {{
    0x00000010,
    0x00000094,
    0x00000020,
    0x000000a4,
}};

sPachinkoWaveColumnX_Layout sPachinkoWaveColumnX = {{
    0x00000018,
    0x00000050,
    0x00000080,
    0x000000b0,
    0x000000e0,
}};

sPachinkoPopSfx_Layout sPachinkoPopSfx = {{
    0x0000017f,
    0x00000180,
    0x00000181,
    0x00000182,
    0x00000183,
    0x00000183,
}};

#pragma section sName end
