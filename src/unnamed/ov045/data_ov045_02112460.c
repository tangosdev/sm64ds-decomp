#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov045_0211265c[];
extern Word data_ov045_02112940[];
extern Word data_ov045_02112780[];
extern Word data_ov045_02111e64[];
extern Word data_ov045_02111e60[];
extern Word data_ov045_02111e84[];
extern Word data_ov045_02111e68[];
extern Word data_ov045_02112720[];
extern Word data_ov045_02112460[];
extern Word data_ov045_02112468[];
extern Word data_ov045_02112470[];
extern Word data_ov045_02112478[];
extern Word data_ov045_02112480[];
extern Word data_ov045_02112488[];
extern Word data_ov045_02112490[];
extern Word data_ov045_0211249c[];
extern Word data_ov045_021124ac[];
extern Word data_ov045_021124bc[];

#pragma define_section sRun ".data.w12460"                                                         \
                            ".data.w12460"                                                         \
                            ".data.w12460" far_data
#pragma section sRun begin

Word data_ov045_02112470[2] = {{.w = 0x00000002}, {.p = data_ov045_021124ac}};

Word data_ov045_02112468[2] = {{.w = 0x07d007d0}, {.w = 0x07d007d0}};

Word data_ov045_02112460[2] = {{.w = 0xff0a0000}, {.w = 0x00007fff}};

Word data_ov045_02112488[2] = {{.w = 0x61676f79}, {.w = 0x0030306e}};

Word data_ov045_02112480[2] = {{.w = 0x00000007}, {.p = data_ov045_0211265c}};

Word data_ov045_02112478[2] = {{.w = 0x077c077b}, {.w = 0x077e077d}};

Word data_ov045_02112490[3] = {
    {.p = data_ov045_02112470}, {.p = data_ov045_021124bc}, {.w = 0x14e60000}};

Word data_ov045_0211249c[4] = {
    {.w = 0x0f0a19af}, {.w = 0xc00000f5}, {.w = 0x00260000}, {.w = 0x00002204}};

Word data_ov045_021124ac[4] = {
    {.w = 0x00003700}, {.p = data_ov045_02112940}, {.w = 0x00003805}, {.p = data_ov045_02112780}};

Word data_ov045_021124bc[37] = {
    {.w = 0x000000c8},          {.p = data_ov045_02111e64}, {.p = data_ov045_02111e60},
    {.p = data_ov045_02111e84}, {.w = 0x00000001},          {.p = data_ov045_02111e68},
    {.p = data_ov045_02112720}, {.p = data_ov045_02112480}, {.w = 0x077a0782},
    {.w = 0x07840783},          {.p = data_ov045_02112490}, {.w = 0x06400001},
    {.w = 0x0000007f},          {.w = 0x53504c43},          {.w = 0x00020008},
    {.w = 0x00002fc0},          {.w = 0x000000ff},          {.w = 0x00000fc0},
    {.w = 0x000000ff},          {.w = 0xffffffff},          {.w = 0xffffffff},
    {.w = 0x53504c43},          {.w = 0x00010008},          {.w = 0x01880fcc},
    {.w = 0x000000ff},          {.w = 0xffffffff},          {.w = 0xffffffff},
    {.w = 0xffffffff},          {.w = 0xffffffff},          {.w = 0x53504c43},
    {.w = 0x00010008},          {.w = 0x00000fc0},          {.w = 0x000000ff},
    {.w = 0xffffffff},          {.w = 0xffffffff},          {.w = 0xffffffff},
    {.w = 0xffffffff}};

#pragma section sRun end
