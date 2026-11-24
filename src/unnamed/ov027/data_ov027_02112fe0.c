#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov027_021131cc[];
extern Word data_ov027_02113184[];
extern Word data_ov027_02112b18[];
extern Word data_ov027_02112b14[];
extern Word data_ov027_02112b38[];
extern Word data_ov027_02112b1c[];
extern Word data_ov027_02113264[];
extern Word data_ov027_02112fe0[];
extern Word data_ov027_02112fe4[];
extern Word data_ov027_02112fe8[];
extern Word data_ov027_02112ff0[];
extern Word data_ov027_02112ff8[];
extern Word data_ov027_02113000[];
extern Word data_ov027_0211300c[];
extern Word data_ov027_02113018[];
extern Word data_ov027_02113024[];
extern Word data_ov027_02113034[];
extern Word data_ov027_02113044[];
extern Word data_ov027_02113054[];
extern Word data_ov027_02113064[];
extern Word data_ov027_02113074[];
extern Word data_ov027_02113084[];
extern Word data_ov027_02113094[];
extern Word data_ov027_021130a4[];
extern Word data_ov027_021130b4[];

#pragma define_section sRun ".data.pre"                                                            \
                            ".data.pre"                                                            \
                            ".data.pre" far_data
#pragma section sRun begin

Word data_ov027_02112fe4[1] = {{.w = 0x00101014}};

Word data_ov027_02112fe0[1] = {{.w = 0x000007b9}};

Word data_ov027_02112ff0[2] = {{.w = 0x00000009}, {.p = data_ov027_021131cc}};

Word data_ov027_02112ff8[2] = {{.w = 0x00000009}, {.p = data_ov027_02113184}};

Word data_ov027_02112fe8[2] = {{.w = 0xff020000}, {.w = 0x00007f01}};

Word data_ov027_02113018[3] = {
    {.p = data_ov027_02112ff0}, {.p = data_ov027_021130b4}, {.w = 0x16cc0000}};

Word data_ov027_02113000[3] = {{.w = 0x5f74616d}, {.w = 0x65746177}, {.w = 0x00000072}};

Word data_ov027_0211300c[3] = {{.w = 0x03e80802}, {.w = 0x1518ef66}, {.w = 0xf89403b6}};

Word data_ov027_02113054[4] = {
    {.w = 0x065200ef}, {.w = 0xf23f0834}, {.w = 0x00000000}, {.w = 0xffff0000}};

Word data_ov027_02113074[4] = {
    {.w = 0x04051494}, {.w = 0x0001eb3c}, {.w = 0x03e8f19f}, {.w = 0x1001fb6f}};

Word data_ov027_02113084[4] = {
    {.w = 0x065200ef}, {.w = 0xf23f0834}, {.w = 0x00000000}, {.w = 0xffff0000}};

Word data_ov027_02113024[4] = {
    {.w = 0x083401eb}, {.w = 0x0000052b}, {.w = 0x0014caab}, {.w = 0x000000ff}};

Word data_ov027_02113034[4] = {
    {.w = 0x065200ef}, {.w = 0xf23f0834}, {.w = 0x00000000}, {.w = 0xffff0000}};

Word data_ov027_02113064[4] = {
    {.w = 0x065200ef}, {.w = 0xf23f0834}, {.w = 0x00000000}, {.w = 0xffff0000}};

Word data_ov027_02113094[4] = {
    {.w = 0x065200ef}, {.w = 0xf23f0834}, {.w = 0x00000000}, {.w = 0xffff0000}};

Word data_ov027_02113044[4] = {
    {.w = 0x065200ef}, {.w = 0xf23f0834}, {.w = 0x00000000}, {.w = 0xffff0000}};

Word data_ov027_021130a4[4] = {
    {.w = 0x03e8f19f}, {.w = 0x0080fb6f}, {.w = 0x04051494}, {.w = 0x0080eb3c}};

Word data_ov027_021130b4[13] = {
    {.w = 0x0000005b},          {.p = data_ov027_02112b18}, {.p = data_ov027_02112b14},
    {.p = data_ov027_02112b38}, {.w = 0x00000001},          {.p = data_ov027_02112b1c},
    {.p = data_ov027_02113264}, {.p = data_ov027_02112ff8}, {.w = 0x07b807ba},
    {.w = 0x07bc07bb},          {.p = data_ov027_02113018}, {.w = 0x06400001},
    {.w = 0x0000004f}};

#pragma section sRun end
