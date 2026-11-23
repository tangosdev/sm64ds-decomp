#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} sTrmpStartTouchX_Layout;
typedef char sTrmpStartTouchX_size[sizeof(sTrmpStartTouchX_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmpStartTouchY_Layout;
typedef char sTrmpStartTouchY_size[sizeof(sTrmpStartTouchY_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmpIntroFromX_Layout;
typedef char sTrmpIntroFromX_size[sizeof(sTrmpIntroFromX_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmp2IntroToX_Layout;
typedef char sTrmp2IntroToX_size[sizeof(sTrmp2IntroToX_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e054_Layout;
typedef char data_ov006_0212e054_size[sizeof(data_ov006_0212e054_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} sTrmp2IntroFromX_Layout;
typedef char sTrmp2IntroFromX_size[sizeof(sTrmp2IntroFromX_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212e05c_Layout;
typedef char data_ov006_0212e05c_size[sizeof(data_ov006_0212e05c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} sTrmpLineDiffuse_Layout;
typedef char sTrmpLineDiffuse_size[sizeof(sTrmpLineDiffuse_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} sTrmpLineAmbient_Layout;
typedef char sTrmpLineAmbient_size[sizeof(sTrmpLineAmbient_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[3];
} sTrmpItemHitWidths_Layout;
typedef char sTrmpItemHitWidths_size[sizeof(sTrmpItemHitWidths_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} sTrmpItemPatrolLimits_Layout;
typedef char sTrmpItemPatrolLimits_size[sizeof(sTrmpItemPatrolLimits_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} sTrmpItemPatrolSpeed_Layout;
typedef char sTrmpItemPatrolSpeed_size[sizeof(sTrmpItemPatrolSpeed_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
} data_ov006_0212e094_Layout;
typedef char data_ov006_0212e094_size[sizeof(data_ov006_0212e094_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[16];
} sTrmpLineTexCoords_Layout;
typedef char sTrmpLineTexCoords_size[sizeof(sTrmpLineTexCoords_Layout) == 0x40 ? 1 : -1];

typedef struct {
    u32 w[16];
} sTrmpLineFlexWeights_Layout;
typedef char sTrmpLineFlexWeights_size[sizeof(sTrmpLineFlexWeights_Layout) == 0x40 ? 1 : -1];

typedef struct {
    u32 w[30];
} data_ov006_0212e130_Layout;
typedef char data_ov006_0212e130_size[sizeof(data_ov006_0212e130_Layout) == 0x78 ? 1 : -1];

extern sTrmpStartTouchX_Layout sTrmpStartTouchX;
extern sTrmpStartTouchY_Layout sTrmpStartTouchY;
extern sTrmpIntroFromX_Layout sTrmpIntroFromX;
extern sTrmp2IntroToX_Layout sTrmp2IntroToX;
extern data_ov006_0212e054_Layout data_ov006_0212e054;
extern sTrmp2IntroFromX_Layout sTrmp2IntroFromX;
extern data_ov006_0212e05c_Layout data_ov006_0212e05c;
extern sTrmpLineDiffuse_Layout sTrmpLineDiffuse;
extern sTrmpLineAmbient_Layout sTrmpLineAmbient;
extern sTrmpItemHitWidths_Layout sTrmpItemHitWidths;
extern sTrmpItemPatrolLimits_Layout sTrmpItemPatrolLimits;
extern sTrmpItemPatrolSpeed_Layout sTrmpItemPatrolSpeed;
extern data_ov006_0212e094_Layout data_ov006_0212e094;
extern sTrmpLineTexCoords_Layout sTrmpLineTexCoords;
extern sTrmpLineFlexWeights_Layout sTrmpLineFlexWeights;
extern data_ov006_0212e130_Layout data_ov006_0212e130;
extern u8 data_ov006_0213b314[];

#pragma define_section sName ".rodata.before_coin_tables"                                          \
                             ".rodata.before_coin_tables"                                          \
                             ".rodata.before_coin_tables" far_data
#pragma section sName begin

data_ov006_0212e05c_Layout data_ov006_0212e05c = {{
    0x00001000,
}};

#pragma explicit_zero_data on
data_ov006_0212e054_Layout data_ov006_0212e054 = {0};
#pragma explicit_zero_data off

sTrmp2IntroToX_Layout sTrmp2IntroToX = {{
    0x00000010,
}};

sTrmpStartTouchX_Layout sTrmpStartTouchX = {{
    0x00000018,
}};

sTrmp2IntroFromX_Layout sTrmp2IntroFromX = {{
    0x000000f0,
}};

sTrmpStartTouchY_Layout sTrmpStartTouchY = {{
    0x000000b0,
}};

sTrmpIntroFromX_Layout sTrmpIntroFromX = {{
    0x000000e8,
}};

sTrmpLineDiffuse_Layout sTrmpLineDiffuse = {{
    0x13ff7fff,
    0x0000011f,
}};

sTrmpLineAmbient_Layout sTrmpLineAmbient = {{
    0x018c3def,
    0x0000318c,
}};

sTrmpItemHitWidths_Layout sTrmpItemHitWidths = {{
    0x00016000,
    0x00020000,
    0x00030000,
}};

sTrmpItemPatrolLimits_Layout sTrmpItemPatrolLimits = {{
    0x00048000,
    0x00040000,
    0x00038000,
}};

sTrmpItemPatrolSpeed_Layout sTrmpItemPatrolSpeed = {{
    0x00001200,
    0x00001000,
    0x00000e00,
}};

data_ov006_0212e094_Layout data_ov006_0212e094 = {
    0x0000ffff, &data_ov006_0213b314[0], 0x00000001, 0x00000001, 0x00000001, 0x0000001e, 0x00000001,
};

sTrmpLineTexCoords_Layout sTrmpLineTexCoords = {{
    0x00000000,
    0x00000155,
    0x000002aa,
    0x000003ff,
    0x01550000,
    0x01550155,
    0x015502aa,
    0x015503ff,
    0x02aa0000,
    0x02aa0155,
    0x02aa02aa,
    0x02aa03ff,
    0x03ff0000,
    0x03ff0155,
    0x03ff02aa,
    0x03ff03ff,
}};

sTrmpLineFlexWeights_Layout sTrmpLineFlexWeights = {{
    0x00000000,
    0x00006000,
    0x00006000,
    0x00000000,
    0x00006000,
    0x00009000,
    0x00009000,
    0x00006000,
    0x00006000,
    0x00009000,
    0x00009000,
    0x00006000,
    0x00000000,
    0x00006000,
    0x00006000,
    0x00000000,
}};

data_ov006_0212e130_Layout data_ov006_0212e130 = {{
    0x00000000, 0x00000111, 0x00000222, 0x00000333, 0x00000444, 0x00000555, 0x00000666, 0x00000777,
    0x00000889, 0x0000099a, 0x00000aab, 0x00000bbc, 0x00000ccd, 0x00000dde, 0x00000eef, 0x00001000,
    0x00001111, 0x00001222, 0x00001333, 0x00001444, 0x00001555, 0x00001666, 0x00001777, 0x00001889,
    0x0000199a, 0x00001aab, 0x00001bbc, 0x00001ccd, 0x00001dde, 0x00001eef,
}};
