#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} sAmidaPatterns_Layout;
typedef char sAmidaPatterns_size[sizeof(sAmidaPatterns_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sAmidaPatternGoalShuffle_Layout;
typedef char sAmidaPatternGoalShuffle_size[sizeof(sAmidaPatternGoalShuffle_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sAmidaPatternSpeeds_Layout;
typedef char sAmidaPatternSpeeds_size[sizeof(sAmidaPatternSpeeds_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sAmidaPatternRungSkip_Layout;
typedef char sAmidaPatternRungSkip_size[sizeof(sAmidaPatternRungSkip_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} sAmidaPatternExtraRungs_Layout;
typedef char sAmidaPatternExtraRungs_size[sizeof(sAmidaPatternExtraRungs_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[64];
} sAmidaPatternSpeedUp_Layout;
typedef char sAmidaPatternSpeedUp_size[sizeof(sAmidaPatternSpeedUp_Layout) == 0x100 ? 1 : -1];

extern sAmidaPatterns_Layout sAmidaPatterns;
extern sAmidaPatternGoalShuffle_Layout sAmidaPatternGoalShuffle;
extern sAmidaPatternSpeeds_Layout sAmidaPatternSpeeds;
extern sAmidaPatternRungSkip_Layout sAmidaPatternRungSkip;
extern sAmidaPatternExtraRungs_Layout sAmidaPatternExtraRungs;
extern sAmidaPatternSpeedUp_Layout sAmidaPatternSpeedUp;

#pragma define_section sName ".rodata.before_coin_tables"                                          \
                             ".rodata.before_coin_tables"                                          \
                             ".rodata.before_coin_tables" far_data
#pragma section sName begin

sAmidaPatterns_Layout sAmidaPatterns = {{
    0x00000001,
}};

sAmidaPatternRungSkip_Layout sAmidaPatternRungSkip = {{
    0x00000001,
}};

sAmidaPatternSpeeds_Layout sAmidaPatternSpeeds = {{
    0x00000001,
}};

sAmidaPatternGoalShuffle_Layout sAmidaPatternGoalShuffle = {{
    0x00000001,
}};

#pragma explicit_zero_data on
sAmidaPatternExtraRungs_Layout sAmidaPatternExtraRungs = {0};
#pragma explicit_zero_data off

sAmidaPatternSpeedUp_Layout sAmidaPatternSpeedUp = {{
    0x00000001, 0x00000001, 0x00000001, 0x00000003, 0x00000002, 0x00000000, 0x00000000, 0x00000001,
    0x00000002, 0x00000002, 0x00000001, 0x00000001, 0x00000000, 0x00000003, 0x00000001, 0x00000001,
    0x00000001, 0x00000004, 0x00000002, 0x00000001, 0x00000000, 0x00000000, 0x00000003, 0x00000003,
    0x00000003, 0x00000001, 0x00000000, 0x00000002, 0x00000001, 0x00000002, 0x00000002, 0x00000003,
    0x00000002, 0x00000000, 0x00000002, 0x00000001, 0x00000001, 0x00000001, 0x00000006, 0x00000002,
    0x00000001, 0x00000000, 0x00000001, 0x00000003, 0x00000003, 0x00000004, 0x00000001, 0x00000000,
    0x00000001, 0x00000001, 0x00000002, 0x00000001, 0x00000001, 0x00000001, 0x00000000, 0x00000003,
    0x00000001, 0x00000003, 0x00000002, 0x00000004, 0x00000002, 0x00000000, 0x00000004, 0x00000000,
}};
