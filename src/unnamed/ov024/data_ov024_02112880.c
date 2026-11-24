#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov024_02112a7c[];
extern Word data_ov024_02112ac4[];
extern Word data_ov024_021118b4[];
extern Word data_ov024_021118ac[];
extern Word data_ov024_02111948[];
extern Word data_ov024_021118bc[];
extern Word data_ov024_021118b0[];
extern Word data_ov024_021119e8[];
extern Word data_ov024_02111cbc[];
extern Word data_ov024_021118f4[];
extern Word data_ov024_02113260[];
extern Word data_ov024_02112880[];
extern Word data_ov024_02112884[];
extern Word data_ov024_02112888[];
extern Word data_ov024_02112890[];
extern Word data_ov024_02112898[];
extern Word data_ov024_021128a0[];
extern Word data_ov024_021128a8[];
extern Word data_ov024_021128b4[];
extern Word data_ov024_021128c0[];
extern Word data_ov024_021128cc[];
extern Word data_ov024_021128d8[];
extern Word data_ov024_021128e8[];
extern Word data_ov024_021128f8[];
extern Word data_ov024_02112908[];
extern Word data_ov024_02112918[];
extern Word data_ov024_02112928[];
extern Word data_ov024_02112938[];
extern Word data_ov024_02112948[];
extern Word data_ov024_02112958[];

#pragma define_section sRun ".data.gap0"                                                           \
                            ".data.gap0"                                                           \
                            ".data.gap0" far_data
#pragma section sRun begin

Word data_ov024_02112880[1] = {{.w = 0x00000712}};

Word data_ov024_02112884[1] = {{.w = 0x00000104}};

Word data_ov024_021128a0[2] = {{.w = 0x0000000a}, {.p = data_ov024_02112ac4}};

Word data_ov024_02112898[2] = {{.w = 0xf82fe63d}, {.w = 0xfbf60693}};

Word data_ov024_02112890[2] = {{.w = 0xea41423f}, {.w = 0xfc2100d8}};

Word data_ov024_02112888[2] = {{.w = 0x00000009}, {.p = data_ov024_02112a7c}};

Word data_ov024_021128a8[3] = {
    {.p = data_ov024_021128a0}, {.p = data_ov024_02112958 + 10}, {.w = 0x05140000}};

Word data_ov024_021128b4[3] = {{.w = 0x6e726f74}, {.w = 0x5f6f6461}, {.w = 0x006f3130}};

Word data_ov024_021128c0[3] = {{.w = 0x5f74616d}, {.w = 0x73757972}, {.w = 0x00615f61}};

Word data_ov024_021128cc[3] = {{.w = 0x6e726f74}, {.w = 0x5f6f6461}, {.w = 0x00753030}};

Word data_ov024_02112918[4] = {
    {.w = 0xeb4c00fc}, {.w = 0xeed0020d}, {.w = 0x00000000}, {.w = 0xf0000000}};

Word data_ov024_02112928[4] = {
    {.w = 0xeb4c00fc}, {.w = 0xeed0020d}, {.w = 0x00000000}, {.w = 0xf0000000}};

Word data_ov024_021128d8[4] = {
    {.w = 0x5f74616d}, {.w = 0x775f787a}, {.w = 0x72657461}, {.w = 0x00000000}};

Word data_ov024_021128e8[4] = {
    {.w = 0x5f74616d}, {.w = 0x725f797a}, {.w = 0x61737579}, {.w = 0x0000625f}};

Word data_ov024_02112908[4] = {
    {.w = 0xeb4c00fc}, {.w = 0xeed0020d}, {.w = 0x00000000}, {.w = 0xf0000000}};

Word data_ov024_02112938[4] = {
    {.w = 0xeb4c00fc}, {.w = 0xeed0020d}, {.w = 0x00000000}, {.w = 0xf0000000}};

Word data_ov024_021128f8[4] = {
    {.w = 0x0000e9d4}, {.w = 0x00a0ed35}, {.w = 0x00001a5e}, {.w = 0x0060ed40}};

Word data_ov024_02112948[4] = {
    {.w = 0x06d500fc}, {.w = 0x00660504}, {.w = 0x00000000}, {.w = 0x11000000}};

Word data_ov024_02112958[38] = {
    {.w = 0x00001a5e},          {.w = 0x0002ed40},          {.w = 0x0000e926},
    {.w = 0x1000ed72},          {.w = 0x00000028},          {.p = data_ov024_021118b4},
    {.p = data_ov024_021118ac}, {.p = data_ov024_02111948}, {.w = 0x00000002},
    {.p = data_ov024_021118bc}, {.w = 0x00000169},          {.p = data_ov024_021118b0},
    {.p = data_ov024_021119e8}, {.p = data_ov024_02111cbc}, {.w = 0x00000003},
    {.p = data_ov024_021118f4}, {.w = 0xffdcf830},          {.w = 0xf8e400ad},
    {.w = 0x00110000},          {.w = 0xf83042ff},          {.w = 0xfc7c032a},
    {.w = 0x0000c000},          {.w = 0x11ff0411},          {.p = data_ov024_02113260},
    {.p = data_ov024_02112888}, {.w = 0x07110713},          {.w = 0x07150714},
    {.p = data_ov024_021128a8}, {.w = 0x06400001},          {.w = 0x0000006f},
    {.w = 0xeb4c00fc},          {.w = 0xeed0020d},          {.w = 0x00000000},
    {.w = 0xf0000000},          {.w = 0x16760014},          {.w = 0x0e740258},
    {.w = 0x00000003},          {.w = 0xffff0000}};

#pragma section sRun end
