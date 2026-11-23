#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} sCurlingBitAStates_Layout;
typedef char sCurlingBitAStates_size[sizeof(sCurlingBitAStates_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sCurlingBitBStates_Layout;
typedef char sCurlingBitBStates_size[sizeof(sCurlingBitBStates_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} sCurlingScoreDelays_Layout;
typedef char sCurlingScoreDelays_size[sizeof(sCurlingScoreDelays_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} sCurlingBitKinds_Layout;
typedef char sCurlingBitKinds_size[sizeof(sCurlingBitKinds_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} sCurlingStonesLeftX_Layout;
typedef char sCurlingStonesLeftX_size[sizeof(sCurlingStonesLeftX_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[5];
} sCurlingSlideZoneEdges_Layout;
typedef char sCurlingSlideZoneEdges_size[sizeof(sCurlingSlideZoneEdges_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[5];
} sCurlingSlideShiftLeft_Layout;
typedef char sCurlingSlideShiftLeft_size[sizeof(sCurlingSlideShiftLeft_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[5];
} sCurlingSlideShiftRight_Layout;
typedef char sCurlingSlideShiftRight_size[sizeof(sCurlingSlideShiftRight_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[5];
} sCurlingSlideMinLeft_Layout;
typedef char sCurlingSlideMinLeft_size[sizeof(sCurlingSlideMinLeft_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[5];
} sCurlingSlideMinRight_Layout;
typedef char sCurlingSlideMinRight_size[sizeof(sCurlingSlideMinRight_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[6];
} sCurlingHouseSpots_Layout;
typedef char sCurlingHouseSpots_size[sizeof(sCurlingHouseSpots_Layout) == 0x18 ? 1 : -1];

extern sCurlingBitAStates_Layout sCurlingBitAStates;
extern sCurlingBitBStates_Layout sCurlingBitBStates;
extern sCurlingScoreDelays_Layout sCurlingScoreDelays;
extern sCurlingBitKinds_Layout sCurlingBitKinds;
extern sCurlingStonesLeftX_Layout sCurlingStonesLeftX;
extern sCurlingSlideZoneEdges_Layout sCurlingSlideZoneEdges;
extern sCurlingSlideShiftLeft_Layout sCurlingSlideShiftLeft;
extern sCurlingSlideShiftRight_Layout sCurlingSlideShiftRight;
extern sCurlingSlideMinLeft_Layout sCurlingSlideMinLeft;
extern sCurlingSlideMinRight_Layout sCurlingSlideMinRight;
extern sCurlingHouseSpots_Layout sCurlingHouseSpots;

#pragma define_section sName ".rodata.gap0"                                                        \
                             ".rodata.gap0"                                                        \
                             ".rodata.gap0" far_data
#pragma section sName begin

sCurlingBitBStates_Layout sCurlingBitBStates = {{
    0x00000201,
}};

sCurlingBitAStates_Layout sCurlingBitAStates = {{
    0x00000201,
}};

sCurlingBitKinds_Layout sCurlingBitKinds = {{
    0x00000001,
    0x00000002,
}};

sCurlingScoreDelays_Layout sCurlingScoreDelays = {{
    0x30486078,
    0x00000018,
}};

sCurlingStonesLeftX_Layout sCurlingStonesLeftX = {{
    0x00000010,
    0x00000030,
    0x000000d0,
    0x000000f0,
}};

sCurlingSlideZoneEdges_Layout sCurlingSlideZoneEdges = {{
    0x000000a0,
    0x00000080,
    0x00000060,
    0x00000040,
    0x00000020,
}};

sCurlingSlideShiftRight_Layout sCurlingSlideShiftRight = {{
    0x00000008,
    0x00000007,
    0x00000006,
    0x00000006,
    0x00000004,
}};

sCurlingSlideMinLeft_Layout sCurlingSlideMinLeft = {{
    0x00000180,
    0x00000140,
    0x00000100,
    0x000000c0,
    0x00000080,
}};

sCurlingSlideShiftLeft_Layout sCurlingSlideShiftLeft = {{
    0x00000004,
    0x00000005,
    0x00000006,
    0x00000007,
    0x00000008,
}};

sCurlingSlideMinRight_Layout sCurlingSlideMinRight = {{
    0x00000080,
    0x000000c0,
    0x00000100,
    0x00000140,
    0x00000180,
}};

sCurlingHouseSpots_Layout sCurlingHouseSpots = {{
    0x00000080,
    0x00000060,
    0x00000038,
    0x00000038,
    0x000000c8,
    0x00000038,
}};

#pragma section sName end
