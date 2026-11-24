#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov031_02111500[];
extern Word data_ov031_02111504[];
extern Word data_ov031_02111508[];
extern Word data_ov031_02111510[];
extern Word data_ov031_02111518[];
extern Word data_ov031_02111520[];
extern Word data_ov031_0211152c[];
extern Word data_ov031_0211153c[];
extern Word data_ov031_0211154c[];
extern Word data_ov031_02111578[];
extern Word data_ov031_021115a8[];
extern Word data_ov031_021115e8[];
extern Word data_ov031_02111668[];
extern Word data_ov031_02111758[];

Word data_ov031_02111500[1] = {{.w = 0x00000756}};

Word data_ov031_02111504[1] = {{.w = 0x11111114}};

Word data_ov031_02111508[2] = {{.w = 0x00000002}, {.p = data_ov031_0211154c}};

Word data_ov031_02111518[2] = {{.w = 0x00000001}, {.w = 0x75306d60}};

Word data_ov031_02111510[2] = {{.w = 0x00000006}, {.p = data_ov031_02111578}};

Word data_ov031_02111520[3] = {{.p = data_ov031_02111508}, {.w = 0x00000000}, {.w = 0x35480000}};

Word data_ov031_0211153c[4] = {
    {.w = 0xcaf2f9fb}, {.w = 0xc000d2d8}, {.w = 0x04160000}, {.w = 0x000011ff}};

Word data_ov031_0211152c[4] = {
    {.w = 0x2d67ff02}, {.w = 0x31f532e3}, {.w = 0xa0000000}, {.w = 0x00000000}};

Word data_ov031_0211154c[11] = {
    {.w = 0x00000f00},          {.p = data_ov031_02111668}, {.w = 0x00003205},
    {.p = data_ov031_02111758}, {.p = data_ov031_021115e8}, {.p = data_ov031_02111510},
    {.w = 0x07550757},          {.w = 0x07590758},          {.p = data_ov031_02111520},
    {.w = 0x0af00001},          {.w = 0x0000000f}};

Word data_ov031_02111578[12] = {
    {.w = 0x00000104}, {.p = data_ov031_0211152c}, {.w = 0x0000010b}, {.p = data_ov031_02111500},
    {.w = 0x00000401}, {.p = data_ov031_021115a8}, {.w = 0x00000108}, {.p = data_ov031_02111518},
    {.w = 0x0000010e}, {.p = data_ov031_02111504}, {.w = 0x0000010a}, {.p = data_ov031_0211153c}};

Word data_ov031_021115a8[16] = {
    {.w = 0x2c6d0000}, {.w = 0x30fb3473}, {.w = 0x9c720000}, {.w = 0x02000000},
    {.w = 0x2c090000}, {.w = 0x31c3322d}, {.w = 0x9c720000}, {.w = 0x00000000},
    {.w = 0x2d670000}, {.w = 0x30c9321b}, {.w = 0x9c720000}, {.w = 0x00000000},
    {.w = 0x2d030000}, {.w = 0x3191321b}, {.w = 0x9c720000}, {.w = 0x00000000}};

Word data_ov031_021115e8[32] = {
    {.w = 0x53504c43}, {.w = 0x000e0008}, {.w = 0x00003fc4}, {.w = 0x000000ff}, {.w = 0x0005bfc4},
    {.w = 0x000000ff}, {.w = 0x01000fc0}, {.w = 0x000000ff}, {.w = 0x04000fc0}, {.w = 0x000000ff},
    {.w = 0x00200fc0}, {.w = 0x000000ff}, {.w = 0x01040fc4}, {.w = 0x000000ff}, {.w = 0x00043fc0},
    {.w = 0x000000ff}, {.w = 0x00040fc0}, {.w = 0x000000ff}, {.w = 0x00043fc4}, {.w = 0x000000ff},
    {.w = 0x01008fc4}, {.w = 0x000000ff}, {.w = 0x01008fc0}, {.w = 0x000000ff}, {.w = 0x0000dfc0},
    {.w = 0x000000ff}, {.w = 0x0000dfc5}, {.w = 0x000000ff}, {.w = 0x0000dfc4}, {.w = 0x000000ff},
    {.w = 0xffffffff}, {.w = 0xffffffff}};

Word data_ov031_02111668[60] = {
    {.w = 0x2b730031}, {.w = 0x2e3f322d}, {.w = 0x00000000}, {.w = 0x00f30000}, {.w = 0x19170031},
    {.w = 0x1da52b77}, {.w = 0xa0000000}, {.w = 0x00f20000}, {.w = 0xcfda0031}, {.w = 0xf6960ad9},
    {.w = 0x80000000}, {.w = 0x00f20000}, {.w = 0x109f00c0}, {.w = 0x07f723b7}, {.w = 0x00000000},
    {.w = 0xffff0000}, {.w = 0x0b4d00c0}, {.w = 0x114df19a}, {.w = 0x00000000}, {.w = 0xffff0000},
    {.w = 0xc81300c0}, {.w = 0xcd6708b0}, {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0x082500c1},
    {.w = 0x159b2526}, {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0xbfde00c1}, {.w = 0x096517fe},
    {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0x405c00c1}, {.w = 0xeabae082}, {.w = 0x00000000},
    {.w = 0xffff0000}, {.w = 0x0d1700c2}, {.w = 0x20122973}, {.w = 0x00000000}, {.w = 0xffff0000},
    {.w = 0xe71c00c2}, {.w = 0x20c9f039}, {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0x297900c2},
    {.w = 0xc2a1ddbe}, {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0x229b00c3}, {.w = 0x19f82dc0},
    {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0xdabc00c3}, {.w = 0x13341cc4}, {.w = 0x00000000},
    {.w = 0xffff0000}, {.w = 0x0d0200c3}, {.w = 0xe25f01b9}, {.w = 0x00000000}, {.w = 0xffff0000}};

Word data_ov031_02111758[100] = {
    {.w = 0x29b1e225}, {.w = 0x3001322d}, {.w = 0x1f25e225}, {.w = 0x23b32e65}, {.w = 0x1e8fe225},
    {.w = 0x231d2e65}, {.w = 0x1df9e225}, {.w = 0x22872e33}, {.w = 0x1d63e225}, {.w = 0x21f12de4},
    {.w = 0x1ccde225}, {.w = 0x215b2d46}, {.w = 0x12d7e225}, {.w = 0x17972819}, {.w = 0x1241e225},
    {.w = 0x170127d8}, {.w = 0x11abe225}, {.w = 0x166b273b}, {.w = 0x1147e225}, {.w = 0x16072620},
    {.w = 0x10e3e225}, {.w = 0x15a32505}, {.w = 0x06ede225}, {.w = 0x0b7b1f8d}, {.w = 0xe4c09a27},
    {.w = 0x0b49180d}, {.w = 0xd4eee225}, {.w = 0x08bf12a9}, {.w = 0xd232e225}, {.w = 0x06031119},
    {.w = 0xd070e225}, {.w = 0x01e90ec1}, {.w = 0xcfa81a27}, {.w = 0xfff60e0d}, {.w = 0xcfda1a27},
    {.w = 0xee6208a5}, {.w = 0xdcbee225}, {.w = 0xd65c02cb}, {.w = 0xdf7ae225}, {.w = 0xd65c024c},
    {.w = 0xe236e225}, {.w = 0xd7560241}, {.w = 0xe3f8e225}, {.w = 0xd8e60197}, {.w = 0xe556e225},
    {.w = 0xda7600ed}, {.w = 0xe61ee225}, {.w = 0xe14cfe87}, {.w = 0xe5ece225}, {.w = 0xea7afa72},
    {.w = 0xe524e225}, {.w = 0xf470f569}, {.w = 0xe524e225}, {.w = 0xf506f52c}, {.w = 0xe524e225},
    {.w = 0xf59cf4ef}, {.w = 0xe524e225}, {.w = 0xf632f4b2}, {.w = 0xe524e225}, {.w = 0xf6c8f0b0},
    {.w = 0xe6b4e225}, {.w = 0x0379eec8}, {.w = 0xe844e225}, {.w = 0x0667ee62}, {.w = 0xea6ae225},
    {.w = 0x088dedfe}, {.w = 0xf87ae225}, {.w = 0x0a4feba9}, {.w = 0xfad2e225}, {.w = 0x08bfeadf},
    {.w = 0xfbcce225}, {.w = 0x0667ea33}, {.w = 0xfc94e225}, {.w = 0xfd08e953}, {.w = 0x0f21e225},
    {.w = 0xeef8e2d8}, {.w = 0x1a43e225}, {.w = 0xef2adf08}, {.w = 0x2371e225}, {.w = 0xefc0dcf4},
    {.w = 0x2d03e225}, {.w = 0xedfedb85}, {.w = 0x2f5be225}, {.w = 0xeb74dada}, {.w = 0x2f5be225},
    {.w = 0xe1b0d93e}, {.w = 0x2e2fe225}, {.w = 0xd882d7e0}, {.w = 0x2a79e225}, {.w = 0xd4ccd718},
    {.w = 0x1f57e225}, {.w = 0xd274d566}, {.w = 0x1ec1e225}, {.w = 0xd274d546}, {.w = 0x1e2be225},
    {.w = 0xd274d525}, {.w = 0x1d95e225}, {.w = 0xd274d505}, {.w = 0x1cffe225}, {.w = 0xd274d4e4}};
