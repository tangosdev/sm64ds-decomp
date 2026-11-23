#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} sBomroomMarkerY_Layout;
typedef char sBomroomMarkerY_size[sizeof(sBomroomMarkerY_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} sBomroomPenGateX_Layout;
typedef char sBomroomPenGateX_size[sizeof(sBomroomPenGateX_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} sBomroomHatchPanelY_Layout;
typedef char sBomroomHatchPanelY_size[sizeof(sBomroomHatchPanelY_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} sBomroomHatchPanelX_Layout;
typedef char sBomroomHatchPanelX_size[sizeof(sBomroomHatchPanelX_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} sBomroomPenGateY_Layout;
typedef char sBomroomPenGateY_size[sizeof(sBomroomPenGateY_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[8];
} sBomroomSpawnDelays_Layout;
typedef char sBomroomSpawnDelays_size[sizeof(sBomroomSpawnDelays_Layout) == 0x20 ? 1 : -1];

extern sBomroomMarkerY_Layout sBomroomMarkerY;
extern sBomroomPenGateX_Layout sBomroomPenGateX;
extern sBomroomHatchPanelY_Layout sBomroomHatchPanelY;
extern sBomroomHatchPanelX_Layout sBomroomHatchPanelX;
extern sBomroomPenGateY_Layout sBomroomPenGateY;
extern sBomroomSpawnDelays_Layout sBomroomSpawnDelays;

#pragma define_section sName ".rodata.before_coin_tables"                                          \
                             ".rodata.before_coin_tables"                                          \
                             ".rodata.before_coin_tables" far_data
#pragma section sName begin

sBomroomPenGateY_Layout sBomroomPenGateY = {{
    0x00000050,
    0x00000050,
}};

sBomroomHatchPanelX_Layout sBomroomHatchPanelX = {{
    0x00000080,
    0x00000080,
}};

sBomroomHatchPanelY_Layout sBomroomHatchPanelY = {{
    0x00000008,
    0x000000b8,
}};

sBomroomPenGateX_Layout sBomroomPenGateX = {{
    0x00000000,
    0x00000100,
}};

sBomroomMarkerY_Layout sBomroomMarkerY = {{
    0x00000010,
    0x000000b4,
}};

sBomroomSpawnDelays_Layout sBomroomSpawnDelays = {{
    0x00600060,
    0x00480060,
    0x00600030,
    0x00480048,
    0x00200060,
    0x00480060,
    0x00600060,
    0x00600060,
}};
