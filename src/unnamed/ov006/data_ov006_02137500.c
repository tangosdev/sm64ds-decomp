#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov006_02137500[];
extern Word data_ov006_02137508[];
extern Word data_ov006_02137510[];
extern Word data_ov006_02137518[];
extern Word data_ov006_02137520[];
extern Word data_ov006_02137528[];
extern Word data_ov006_02137530[];
extern Word data_ov006_02137538[];
extern Word data_ov006_02137540[];
extern Word data_ov006_02137548[];
extern Word data_ov006_02137550[];
extern Word data_ov006_02137558[];

#pragma define_section sRun ".data.gap28"                                                          \
                            ".data.gap28"                                                          \
                            ".data.gap28" far_data
#pragma section sRun begin

Word data_ov006_02137550[2] = {{.w = 0x81f000f0}, {.w = 0xffff400c}};

Word data_ov006_02137548[2] = {{.w = 0x81f000f0}, {.w = 0xffff500c}};

Word data_ov006_02137540[2] = {{.w = 0x81f000f0}, {.w = 0xffff008c}};

Word data_ov006_02137538[2] = {{.w = 0x81f000f0}, {.w = 0xffff408c}};

Word data_ov006_02137530[2] = {{.w = 0x81f000f0}, {.w = 0xffff508c}};

Word data_ov006_02137528[2] = {{.w = 0x81f000f0}, {.w = 0xffff0010}};

Word data_ov006_02137520[2] = {{.w = 0x81f000f0}, {.w = 0xffff4010}};

Word data_ov006_02137518[2] = {{.w = 0x81f000f0}, {.w = 0xffff5010}};

Word data_ov006_02137510[2] = {{.w = 0x81f000f0}, {.w = 0xffff0090}};

Word data_ov006_02137508[2] = {{.w = 0x81f000f0}, {.w = 0xffff4090}};

Word data_ov006_02137500[2] = {{.w = 0x81f000f0}, {.w = 0xffff5090}};

Word data_ov006_02137558[14] = {
    {.w = 0x81f000f0},          {.w = 0xffff000c},          {.p = data_ov006_02137558},
    {.p = data_ov006_02137550}, {.p = data_ov006_02137548}, {.p = data_ov006_02137540},
    {.p = data_ov006_02137538}, {.p = data_ov006_02137530}, {.p = data_ov006_02137528},
    {.p = data_ov006_02137520}, {.p = data_ov006_02137518}, {.p = data_ov006_02137510},
    {.p = data_ov006_02137508}, {.p = data_ov006_02137500}};

#pragma section sRun end
