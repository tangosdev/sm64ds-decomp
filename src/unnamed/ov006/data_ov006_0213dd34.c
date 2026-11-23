#include "types.h"
#include "OamAttr.h"

/* Definition order is what makes the compiler's size sort match address order. */

extern u8 sPanelFlipFrameSprites[12];
extern void *_ZTI12dScMgPanel_c[3];
extern u16 *sPanelPuzzleXPerMode[3];
extern u16 *sPanelPuzzleYPerMode[3];
extern u8 sPanelScrambleFlipEnabled[16];
extern u8 sPanelScrambleFlips[16];
extern u8 _ZTS12dScMgPanel_c[16];
extern u8 sPanelScrambleWidths[16];
extern struct OamAttr data_ov006_0213dda4[2];
extern struct OamAttr data_ov006_0213ddb4[2];
extern u32 data_ov006_0213ddc4[4];
extern u32 data_ov006_0213ddd4[4];
extern u32 data_ov006_0213dde4[4];
extern struct OamAttr data_ov006_0213ddf4[3];
extern u32 data_ov006_0213de0c[7];
extern u8 sPanelScrambleFlipEnabledAlt[28];
extern u8 sPanelScrambleFlipsAlt[28];
extern u8 sPanelPuzzleSizes[28];
extern u32 data_ov006_0213de7c[7];
extern u32 data_ov006_0213de98[7];
extern u32 data_ov006_0213deb4[7];
extern u8 *sPanelPuzzlesAlt[8];
extern struct OamAttr *sPanelParticleSprites[8];
extern u32 data_ov006_0213df10[8];
extern u32 data_ov006_0213df30[8];
extern u32 data_ov006_0213df50[9];
extern u32 data_ov006_0213df74[9];
extern u32 data_ov006_0213df98[9];
extern u32 data_ov006_0213dfbc[9];
extern u32 data_ov006_0213dfe0[9];
extern u32 data_ov006_0213e004[9];
extern u32 data_ov006_0213e028[9];
extern u32 data_ov006_0213e04c[9];
extern u8 *sPanelPuzzles[9];
extern struct OamAttr data_ov006_0213e094[5];
extern struct OamAttr data_ov006_0213e0bc[5];
extern struct OamAttr data_ov006_0213e0e4[5];
extern u32 data_ov006_0213e10c[13];
extern u32 data_ov006_0213e140[13];
extern struct OamAttr data_ov006_0213e174[8];
extern u32 data_ov006_0213e1b4[18];

extern u8 _ZTI11dScMgBase_c[];
extern u8 _ZTVN3abi20__si_class_type_infoE[];
extern unsigned int data_ov006_0213dc94[];
extern u16 data_ov006_0213e1fc[];
extern int data_ov006_021427ac[];
extern int data_ov006_02142804[];

#pragma define_section sGap60 ".data.gap60"                                                        \
                              ".data.gap60"                                                        \
                              ".data.gap60" far_data
#pragma section sGap60 begin
u16 *sPanelPuzzleYPerMode[3] = {(u16 *)&data_ov006_0213df30, (u16 *)&data_ov006_0213e140,
                                (u16 *)&data_ov006_0213e1fc};
void *_ZTI12dScMgPanel_c[3] = {(void *)&_ZTVN3abi20__si_class_type_infoE,
                               (void *)&_ZTS12dScMgPanel_c, (void *)&_ZTI11dScMgBase_c};
u16 *sPanelPuzzleXPerMode[3] = {(u16 *)&data_ov006_0213df10, (u16 *)&data_ov006_0213e10c,
                                (u16 *)&data_ov006_0213e1b4};
u8 sPanelFlipFrameSprites[12] = {0x03, 0x04, 0x05, 0x01, 0x00, 0x00,
                                 0x01, 0x02, 0x04, 0x03, 0x00, 0x00};
u8 sPanelScrambleFlips[16] = {0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x03,
                              0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00};
u8 sPanelScrambleWidths[16] = {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
                               0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00};
u32 data_ov006_0213ddd4[4] = {0x01000001, 0x00010100, 0x00010100, 0x01000001};
u8 sPanelScrambleFlipEnabled[16] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
                                    0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00};
struct OamAttr data_ov006_0213dda4[2] = {{0x00ee, 0x01f0, 0x20b9, 0x0000},
                                         {0x00fc, 0x01fc, 0x2079, 0xffff}};
struct OamAttr data_ov006_0213ddb4[2] = {{0x00f8, 0x41f8, 0x20ba, 0x0000},
                                         {0x00f3, 0x0006, 0x2099, 0xffff}};
u32 data_ov006_0213ddc4[4] = {0x01000001, 0x01000101, 0x01010001, 0x01000001};
u32 data_ov006_0213dde4[4] = {0x00000000, 0x00010100, 0x00010100, 0x00000000};
u8 _ZTS12dScMgPanel_c[16] = {0x31, 0x32, 0x64, 0x53, 0x63, 0x4d, 0x67, 0x50,
                             0x61, 0x6e, 0x65, 0x6c, 0x5f, 0x63, 0x00, 0x00};
struct OamAttr data_ov006_0213ddf4[3] = {{0x00f5, 0x0005, 0x2079, 0x0000},
                                         {0x00f8, 0x41f8, 0x207a, 0x0000},
                                         {0x00f5, 0x01f5, 0x2099, 0xffff}};
u8 sPanelPuzzleSizes[28] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05,
                            0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05,
                            0x04, 0x04, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00};
u32 data_ov006_0213de0c[7] = {0x01010101, 0x00000101, 0x01010100, 0x01010101,
                              0x01000000, 0x01010101, 0x00000001};
u8 sPanelScrambleFlipEnabledAlt[28] = {0x01, 0x00, 0x01, 0x00, 0x01, 0x02, 0x00, 0x02, 0x00, 0x02,
                                       0x01, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x02, 0x00,
                                       0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 sPanelScrambleFlipsAlt[28] = {0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x01, 0x01, 0x02, 0x02,
                                 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
                                 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00};
u32 data_ov006_0213deb4[7] = {0x01000100, 0x01000100, 0x01000100, 0x01000100,
                              0x01000100, 0x01000100, 0x00000000};
u32 data_ov006_0213de7c[7] = {0x00000000, 0x01010000, 0x01000001, 0x00000101,
                              0x00010101, 0x00000000, 0x00000000};
u32 data_ov006_0213de98[7] = {0x00000001, 0x00010101, 0x00010100, 0x01010001,
                              0x01010000, 0x00000001, 0x00000001};
u8 *sPanelPuzzlesAlt[8] = {(u8 *)&data_ov006_0213df50, (u8 *)&data_ov006_0213df74,
                           (u8 *)&data_ov006_0213df98, (u8 *)&data_ov006_0213dfbc,
                           (u8 *)&data_ov006_0213dfe0, (u8 *)&data_ov006_0213e004,
                           (u8 *)&data_ov006_0213e028, (u8 *)&data_ov006_0213e04c};
struct OamAttr *sPanelParticleSprites[8] = {
    (struct OamAttr *)&data_ov006_0213e174, (struct OamAttr *)&data_ov006_0213e094,
    (struct OamAttr *)&data_ov006_0213e0bc, (struct OamAttr *)&data_ov006_0213dda4,
    (struct OamAttr *)&data_ov006_0213e0e4, (struct OamAttr *)&data_ov006_0213ddf4,
    (struct OamAttr *)&data_ov006_0213ddb4, (struct OamAttr *)&data_ov006_0213dc94};
u32 data_ov006_0213df10[8] = {0x00700050, 0x00b00090, 0x00700050, 0x00b00090,
                              0x00700050, 0x00b00090, 0x00700050, 0x00b00090};
u32 data_ov006_0213df30[8] = {0x00300030, 0x00300030, 0x00500050, 0x00500050,
                              0x00700070, 0x00700070, 0x00900090, 0x00900090};
u32 data_ov006_0213e04c[9] = {0x01010000, 0x01000000, 0x00010101, 0x01010101, 0x00000101,
                              0x00000101, 0x01010000, 0x00000000, 0x00000101};
u32 data_ov006_0213df50[9] = {0x00000000, 0x01000000, 0x00010101, 0x01010100, 0x01000001,
                              0x00010101, 0x01010100, 0x00000001, 0x00000000};
u32 data_ov006_0213df74[9] = {0x00000001, 0x01010100, 0x01000000, 0x00010001, 0x00010100,
                              0x01000100, 0x00000001, 0x00010101, 0x01000000};
u32 data_ov006_0213df98[9] = {0x01000100, 0x00010100, 0x00010001, 0x01000100, 0x00010100,
                              0x00010001, 0x01000100, 0x00010100, 0x00010001};
u32 data_ov006_0213dfbc[9] = {0x00000000, 0x00000000, 0x00000101, 0x01010100, 0x01000001,
                              0x00010101, 0x01010000, 0x00000000, 0x00000000};
u32 data_ov006_0213dfe0[9] = {0x00000000, 0x00000000, 0x00000101, 0x00000100, 0x01000001,
                              0x00010101, 0x00000100, 0x01000001, 0x00010000};
u32 data_ov006_0213e004[9] = {0x01010101, 0x00000101, 0x01000000, 0x01010101, 0x00010100,
                              0x01000101, 0x00000001, 0x01010100, 0x01010101};
u32 data_ov006_0213e028[9] = {0x00000000, 0x01010000, 0x01010000, 0x01010101, 0x01010101,
                              0x01010101, 0x01010100, 0x00000001, 0x00000101};
u8 *sPanelPuzzles[9] = {
    (u8 *)&data_ov006_0213dde4, (u8 *)&data_ov006_0213ddc4, (u8 *)&data_ov006_0213ddd4,
    (u8 *)&data_ov006_021427ac, (u8 *)&data_ov006_0213de7c, (u8 *)&data_ov006_0213de98,
    (u8 *)&data_ov006_0213deb4, (u8 *)&data_ov006_02142804, (u8 *)&data_ov006_0213de0c};
struct OamAttr data_ov006_0213e094[5] = {{0x00ee, 0x41ed, 0x20ba, 0x0000},
                                         {0x0003, 0x01f0, 0x2079, 0x0000},
                                         {0x0001, 0x0009, 0x2099, 0x0000},
                                         {0x00f8, 0x41f8, 0x207a, 0x0000},
                                         {0x00ee, 0x7005, 0x20ba, 0xffff}};
struct OamAttr data_ov006_0213e0bc[5] = {{0x00ef, 0x0009, 0x20b9, 0x0000},
                                         {0x00fc, 0x01ed, 0x20b9, 0x0000},
                                         {0x00f1, 0x01f4, 0x2079, 0x0000},
                                         {0x00ff, 0x0008, 0x20b9, 0x0000},
                                         {0x00f8, 0x41f8, 0x20ba, 0xffff}};
struct OamAttr data_ov006_0213e0e4[5] = {{0x00f2, 0x41f1, 0x20ba, 0x0000},
                                         {0x00f3, 0x41f5, 0x2019, 0x0000},
                                         {0x80f3, 0x0005, 0x201b, 0x0000},
                                         {0x4003, 0x01f5, 0x2059, 0x0000},
                                         {0x0003, 0x0005, 0x205b, 0xffff}};
u32 data_ov006_0213e140[13] = {0x00200020, 0x00200020, 0x00400020, 0x00400040, 0x00400040,
                               0x00600060, 0x00600060, 0x00800060, 0x00800080, 0x00800080,
                               0x00a000a0, 0x00a000a0, 0x000000a0};
u32 data_ov006_0213e10c[13] = {0x00600040, 0x00a00080, 0x004000c0, 0x00800060, 0x00c000a0,
                               0x00600040, 0x00a00080, 0x004000c0, 0x00800060, 0x00c000a0,
                               0x00600040, 0x00a00080, 0x000000c0};
struct OamAttr data_ov006_0213e174[8] = {
    {0x00f5, 0x0005, 0x2079, 0x0000}, {0x00f4, 0x41f4, 0x2019, 0x0000},
    {0x80f4, 0x0004, 0x201b, 0x0000}, {0x4004, 0x01f4, 0x2059, 0x0000},
    {0x0004, 0x0004, 0x205b, 0x0000}, {0x00ef, 0x41ed, 0x207a, 0x0000},
    {0x0003, 0x41ee, 0x20ba, 0x0000}, {0x0001, 0x4002, 0x20ba, 0xffff}};
u32 data_ov006_0213e1b4[18] = {0x00500030, 0x00900070, 0x00d000b0, 0x00500030, 0x00900070,
                               0x00d000b0, 0x00500030, 0x00900070, 0x00d000b0, 0x00500030,
                               0x00900070, 0x00d000b0, 0x00500030, 0x00900070, 0x00d000b0,
                               0x00500030, 0x00900070, 0x00d000b0};
#pragma section sGap60 end
