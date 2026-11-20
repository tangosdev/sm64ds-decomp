//cpp
/**
 * Minigame-select scene.
 *
 * Four character tabs of nine icons on the top screen; the sub screen
 * scrolls by mSubBgScrollX onto the second page. InitResources loads
 * the menu art and seeds unlock state from the minigame table.
 * Behavior runs page and exit timers plus the tap handlers; Render
 * draws tabs, new-game marks, page arrows and scores.
 *
 * dScMiniGm_c_classInit is reconstructed (RTTI dScMiniGm_c, MINIGAME
 * registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: helpers stay func_ov005_* offset soup. This TU's Behavior
 *   and Render call them. Naming belongs with recovered members, not
 *   coined here.
 * Leftover: func_02013568 / func_02013580 / func_02013598 /
 *   func_020135d0 / IsMinigameUnlocked are arm9 save-table helpers. This
 *   TU's InitResources, IsUnlocked, IsNew,
 *   ClearNew, GetRecord, SetRecord
 *   call them; GetSaveTable reads the table's save-slot byte
 *   and hands it over. Naming belongs in arm9.
 * Leftover: func_02012790 is still the linker name of Sound::Play2D(2,
 *   id). This TU's Behavior and tap handlers (UpdateGameSelect /
 *   UpdatePageArrows / UpdateExitButton / UpdateGroupTabs)
 *   call it. Naming belongs in arm9.
 * Leftover: Disable3dEngines / func_020233f4 are arm9 graphics-setup
 *   tails. This TU's InitResources calls them.
 * Leftover: _ZTV11dScMiniGm_c is emitted by the key function. The
 *   public symbol is eight bytes into that storage, between the
 *   screen offsets and the menu rows, so a second array under the
 *   same name would be a duplicate definition. sMiniGmSaveTable is
 *   the first row's sceneParam at sMiniGmRows + 4.
 * Leftover: g_profile_MINIGAME lives outside this TU (S14).
 * Leftover: inline destructor (out-of-line emits D0 before D1 plus a
 *   homeless D2).
 */

#include "dScMiniGm_c.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_light.h"
#include "MgMenuRow.h"
#include "dWipe_c.h"
#include "dFdBrightness_c.h"
#include "types.h"
#include "Sound.h"
#include "OAM.h"
#include "OamAttr.h"
#include "decl_Scene.h"
#include "decl_common.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "nitro/os/cache.h"
#include "nitro/gx/g3x.h"
#include "dScene_c.h"
#include "LevelRng.h"
#include "SaveData.h"
#include "nitro/mi/memory.h"
#include "math/CalcLib.h"
#include "PlayerInput.h"

struct OamAttr;

typedef struct Pair { int x, y; } Pair;
struct Off9 { int v[9]; };

/* ---- arm9 globals ---- */
extern u8 gMgSaveDirty;
extern u8 gMgMenuPage;
extern MgEntry gMgMenuRow;
extern MinigameSaveData data_0209cae4;
extern u8 gSubScreenLayers;
extern u8 gMainScreenLayers;
extern short gSubBg3HOffset;
extern short gSubBg0HOffset;
extern dFdBrightness_c *gSceneFader; /* the installed screen fader */
extern u8 data_0209f5f8;
extern int gFrameStep;
extern unsigned short data_020a0e5a[];

/* ---- ov005 tables ---- */
extern "C" {
extern const int sMiniGmGroupBases[] = {3, 0, 1, 2};

extern const int sMiniGmBgFiles[] = {80, 79, 81, 82, 85, 84, 86, 87};

extern const Pair sMiniGmRecordPos[] = {{0x58, 0x10}, {0x88, 0x10}, {0xb8, 0x10},
                                        {0x58, 0xb0}, {0x88, 0xb0}, {0xb8, 0xb0},
                                        {0x78, 0x30}, {0xa8, 0x30}, {0xd8, 0x30}};

extern const Pair sMiniGmBadgePos[] = {{0x58, 0x28}, {0x88, 0x28}, {0xb8, 0x28},
                                       {0x58, 0x70}, {0x88, 0x70}, {0xb8, 0x70},
                                       {0x78, 0x48}, {0xa8, 0x48}, {0xd8, 0x48}};

extern const u32 sMiniGmIconPalettes[] = {
    415, 471, 465, 461, 459, 463, 483, 481, 421, 429, 417, 477, 479, 469, 423, 467, 427, 413,
    431, 475, 433, 419, 473, 425, 437, 447, 449, 443, 441, 453, 439, 445, 455, 435, 457, 451};

extern const int sMiniGmIconChars[] = {414, 470, 464, 460, 458, 462, 482, 480, 420, 428, 416, 476,
                                       478, 468, 422, 466, 426, 412, 430, 474, 432, 418, 472, 424,
                                       436, 446, 448, 442, 440, 452, 438, 444, 454, 434, 456, 450};

struct Off9 sMiniGmIconScreenOffsets = {
    {0x68, 0x6e, 0x74, 0x188, 0x18e, 0x194, 0x4e2, 0x4e8, 0x4ee}};

/* 36 menu rows. sMiniGmSaveTable is row 0's sceneParam, at this symbol + 4. */
MgEntry sMiniGmRows[] = {{371, 0, 0x0, 1, 9999, 0, {0, 0, 0, 0, 0}, 13, 1, 0},
                         {362, 0, 0x10100, 2, 9999, 1, {12, 0, 0, 0, 0}, 9, 2, 0},
                         {368, 0, 0xa0600, 3, 999999, 0, {0, 0, 0, 0, 0}, 11, 1, 0},
                         {366, 0, 0x30300, 1, 9999, 0, {0, 0, 0, 0, 0}, 13, 1, 0},
                         {372, 0, 0x40400, 1, 9999, 0, {0, 0, 0, 0, 0}, 15, 1, 0},
                         {381, 0, 0xd0500, 2, 9999, 1, {10, 0, 0, 0, 0}, 9, 2, 0},
                         {377, 0, 0x60a00, 5, 0, 0, {1200, 1200, 1200, 1200, 1200}, 12, 1, 0},
                         {390, 0, 0x171300, 0, 0, 1, {0, 0, 0, 0, 0}, 19, 2, 0},
                         {370, 0, 0x140800, 3, 9999, 0, {0, 0, 0, 0, 0}, 11, 1, 0},
                         {379, 0, 0x50900, 2, 9999, 1, {10, 0, 0, 0, 0}, 9, 2, 0},
                         {376, 0, 0x20200, 3, 999999, 0, {0, 0, 0, 0, 0}, 10, 2, 0},
                         {387, 0, 0x130b00, 1, 9999, 0, {0, 0, 0, 0, 0}, 14, 1, 0},
                         {384, 0, 0xc0c00, 3, 9999, 0, {0, 0, 0, 0, 0}, 16, 1, 0},
                         {383, 0, 0x90d00, 2, 9999, 1, {10, 0, 0, 0, 0}, 9, 2, 0},
                         {378, 0, 0xe0e00, 3, 9999, 0, {0, 0, 0, 0, 0}, 14, 1, 0},
                         {380, 0, 0x160f00, 1, 9999, 0, {0, 0, 0, 0, 0}, 8, 1, 0},
                         {374, 0, 0x101000, 3, 9999, 0, {0, 0, 0, 0, 0}, 12, 2, 0},
                         {364, 0, 0x111100, 2, 9999, 1, {10, 0, 0, 0, 0}, 9, 2, 0},
                         {389, 0, 0x121200, 1, 9999, 0, {0, 0, 0, 0, 0}, 20, 2, 0},
                         {367, 0, 0x70700, 1, 9999, 0, {0, 0, 0, 0, 0}, 6, 0, 0},
                         {373, 0, 0x1c1c01, 3, 9999, 0, {0, 0, 0, 0, 0}, 15, 1, 0},
                         {388, 0, 0x151500, 2, 9999, 1, {10, 0, 0, 0, 0}, 9, 2, 0},
                         {376, 0, 0x221601, 3, 999999, 0, {0, 0, 0, 0, 0}, 10, 2, 0},
                         {361, 0, 0xb1700, 1, 9999, 0, {0, 0, 0, 0, 0}, 17, 2, 0},
                         {371, 0, 0x181802, 1, 9999, 0, {0, 0, 0, 0, 0}, 13, 1, 0},
                         {382, 0, 0x191901, 3, 9999, 0, {0, 0, 0, 0, 0}, 9, 2, 0},
                         {369, 0, 0x1a1a01, 3, 9999, 0, {0, 0, 0, 0, 0}, 10, 1, 0},
                         {386, 0, 0xf1b00, 1, 9999, 0, {0, 0, 0, 0, 0}, 18, 2, 0},
                         {375, 0, 0x1f1f01, 3, 999999, 0, {0, 0, 0, 0, 0}, 12, 2, 0},
                         {363, 0, 0x1d1d01, 2, 9999, 1, {15, 0, 0, 0, 0}, 9, 2, 0},
                         {378, 0, 0x1e1e01, 3, 9999, 0, {0, 0, 0, 0, 0}, 14, 1, 0},
                         {365, 0, 0x1b1400, 1, 9999, 0, {0, 0, 0, 0, 0}, 8, 2, 0},
                         {385, 0, 0x202001, 3, 999999, 0, {0, 0, 0, 0, 0}, 16, 1, 0},
                         {364, 0, 0x212101, 2, 9999, 1, {10, 0, 0, 0, 0}, 9, 2, 0},
                         {377, 0, 0x82201, 5, 0, 0, {2400, 2400, 2400, 2400, 2400}, 12, 1, 0},
                         {380, 0, 0x232301, 1, 9999, 0, {0, 0, 0, 0, 0}, 7, 2, 0}};

/* OAM lists. The entry with attr3 0xffff is the last one drawn. */
OamAttr data_ov005_020c2c48[9] = {
    {0x00f1, 0x81f1, 0x200e, 0x0000}, {0x00e8, 0x81e8, 0x401a, 0x0000},
    {0x80e8, 0x8008, 0x401e, 0x0000}, {0x4008, 0x81e8, 0x409a, 0x0000},
    {0x0008, 0x4008, 0x409e, 0x0000}, {0x00e8, 0x81e8, 0x7000, 0x0000},
    {0x80e8, 0x8008, 0x7004, 0x0000}, {0x4008, 0x81e8, 0x7080, 0x0000},
    {0x0008, 0x4008, 0x7084, 0xffff}};
OamAttr data_ov005_020c2c90[9] = {
    {0x00f1, 0x81f1, 0x100a, 0x0000}, {0x00e8, 0x81e8, 0x601a, 0x0000},
    {0x80e8, 0x8008, 0x601e, 0x0000}, {0x4008, 0x81e8, 0x609a, 0x0000},
    {0x0008, 0x4008, 0x609e, 0x0000}, {0x00e8, 0x81e8, 0x4000, 0x0000},
    {0x80e8, 0x8008, 0x4004, 0x0000}, {0x4008, 0x81e8, 0x4080, 0x0000},
    {0x0008, 0x4008, 0x4084, 0xffff}};
OamAttr data_ov005_020c2cd8[9] = {
    {0x00f1, 0x81f1, 0x5016, 0x0000}, {0x00e8, 0x81e8, 0x401a, 0x0000},
    {0x80e8, 0x8008, 0x401e, 0x0000}, {0x4008, 0x81e8, 0x409a, 0x0000},
    {0x0008, 0x4008, 0x409e, 0x0000}, {0x00e8, 0x81e8, 0x7000, 0x0000},
    {0x80e8, 0x8008, 0x7004, 0x0000}, {0x4008, 0x81e8, 0x7080, 0x0000},
    {0x0008, 0x4008, 0x7084, 0xffff}};
OamAttr data_ov005_020c2d20[9] = {
    {0x00f1, 0x81f1, 0x3012, 0x0000}, {0x00e8, 0x81e8, 0x601a, 0x0000},
    {0x80e8, 0x8008, 0x601e, 0x0000}, {0x4008, 0x81e8, 0x609a, 0x0000},
    {0x0008, 0x4008, 0x609e, 0x0000}, {0x00e8, 0x81e8, 0x4000, 0x0000},
    {0x80e8, 0x8008, 0x4004, 0x0000}, {0x4008, 0x81e8, 0x4080, 0x0000},
    {0x0008, 0x4008, 0x4084, 0xffff}};
OamAttr data_ov005_020c2d68[9] = {
    {0x00f1, 0x81f1, 0x200e, 0x0000}, {0x00e8, 0x81e8, 0x601a, 0x0000},
    {0x80e8, 0x8008, 0x601e, 0x0000}, {0x4008, 0x81e8, 0x609a, 0x0000},
    {0x0008, 0x4008, 0x609e, 0x0000}, {0x00e8, 0x81e8, 0x4000, 0x0000},
    {0x80e8, 0x8008, 0x4004, 0x0000}, {0x4008, 0x81e8, 0x4080, 0x0000},
    {0x0008, 0x4008, 0x4084, 0xffff}};
OamAttr data_ov005_020c2db0[9] = {
    {0x00f1, 0x81f1, 0x100a, 0x0000}, {0x00e8, 0x81e8, 0x401a, 0x0000},
    {0x80e8, 0x8008, 0x401e, 0x0000}, {0x4008, 0x81e8, 0x409a, 0x0000},
    {0x0008, 0x4008, 0x409e, 0x0000}, {0x00e8, 0x81e8, 0x7000, 0x0000},
    {0x80e8, 0x8008, 0x7004, 0x0000}, {0x4008, 0x81e8, 0x7080, 0x0000},
    {0x0008, 0x4008, 0x7084, 0xffff}};
OamAttr data_ov005_020c2df8[9] = {
    {0x00f1, 0x81f1, 0x5016, 0x0000}, {0x00e8, 0x81e8, 0x601a, 0x0000},
    {0x80e8, 0x8008, 0x601e, 0x0000}, {0x4008, 0x81e8, 0x609a, 0x0000},
    {0x0008, 0x4008, 0x609e, 0x0000}, {0x00e8, 0x81e8, 0x4000, 0x0000},
    {0x80e8, 0x8008, 0x4004, 0x0000}, {0x4008, 0x81e8, 0x4080, 0x0000},
    {0x0008, 0x4008, 0x4084, 0xffff}};
OamAttr data_ov005_020c2e40[9] = {
    {0x00f1, 0x81f1, 0x3012, 0x0000}, {0x00e8, 0x81e8, 0x401a, 0x0000},
    {0x80e8, 0x8008, 0x401e, 0x0000}, {0x4008, 0x81e8, 0x409a, 0x0000},
    {0x0008, 0x4008, 0x409e, 0x0000}, {0x00e8, 0x81e8, 0x7000, 0x0000},
    {0x80e8, 0x8008, 0x7004, 0x0000}, {0x4008, 0x81e8, 0x7080, 0x0000},
    {0x0008, 0x4008, 0x7084, 0xffff}};
OamAttr data_ov005_020c2e8c[1] = {{0x00e0, 0xc1e0, 0x0198, 0xffff}};
OamAttr data_ov005_020c2e94[1] = {{0x80e0, 0xc1f0, 0x0194, 0xffff}};
OamAttr data_ov005_020c2e9c[1] = {{0x80e0, 0xc1f0, 0x0190, 0xffff}};
OamAttr data_ov005_020c2eac[1] = {{0x80f8, 0x01fc, 0x408f, 0xffff}};
OamAttr data_ov005_020c2eb4[1] = {{0x80f8, 0x01fc, 0x408e, 0xffff}};
OamAttr data_ov005_020c2ebc[1] = {{0x80f8, 0x01fc, 0x408d, 0xffff}};
OamAttr data_ov005_020c2ec4[1] = {{0x80f8, 0x01fc, 0x408c, 0xffff}};
OamAttr data_ov005_020c2ecc[1] = {{0x80f8, 0x01fc, 0x408b, 0xffff}};
OamAttr data_ov005_020c2ed4[1] = {{0x80f8, 0x01fc, 0x408a, 0xffff}};
OamAttr data_ov005_020c2edc[1] = {{0x80f8, 0x01fc, 0x4089, 0xffff}};
OamAttr data_ov005_020c2ee4[1] = {{0x80f8, 0x01fc, 0x4088, 0xffff}};
OamAttr data_ov005_020c2eec[1] = {{0x80f8, 0x01fc, 0x4087, 0xffff}};
OamAttr data_ov005_020c2ef4[1] = {{0x80f8, 0x01fc, 0x4086, 0xffff}};
OamAttr data_ov005_020c2f24[1] = {{0x00fc, 0x01fc, 0x70b4, 0xffff}};
OamAttr data_ov005_020c2f2c[1] = {{0x00fc, 0x01fc, 0x7094, 0xffff}};
OamAttr data_ov005_020c2f34[1] = {{0x00f8, 0x41f8, 0x6095, 0xffff}};
OamAttr data_ov005_020c2f3c[1] = {{0x00f8, 0x41f8, 0x6092, 0xffff}};
OamAttr data_ov005_020c2f44[1] = {{0x00f8, 0x41f8, 0x6090, 0xffff}};
OamAttr data_ov005_020c2f68[2] = {{0x40f8, 0x81e0, 0x60c6, 0x0000},
                                  {0x40f8, 0x8000, 0x60ca, 0xffff}};
OamAttr data_ov005_020c2f78[2] = {{0x40f8, 0x81e0, 0x00c6, 0x0000},
                                  {0x40f8, 0x8000, 0x00ca, 0xffff}};
OamAttr data_ov005_020c2f8c[8] = {
    {0x40f8, 0x8180, 0x4140, 0x0000}, {0x40f8, 0x81a0, 0x4144, 0x0000},
    {0x40f8, 0x81c0, 0x4148, 0x0000}, {0x40f8, 0x81e0, 0x414c, 0x0000},
    {0x40f8, 0x8000, 0x4150, 0x0000}, {0x40f8, 0x8020, 0x4154, 0x0000},
    {0x40f8, 0x8040, 0x4158, 0x0000}, {0x40f8, 0x8060, 0x415c, 0xffff}};
/* attr3 0xffff ends the caption. The two zero sprites are this symbol's tail through the end of .data. */
OamAttr data_ov005_020c2fd0[10] = {
    {0x40f8, 0x8180, 0x4100, 0x0000}, {0x40f8, 0x81a0, 0x4104, 0x0000},
    {0x40f8, 0x81c0, 0x4108, 0x0000}, {0x40f8, 0x81e0, 0x410c, 0x0000},
    {0x40f8, 0x8000, 0x4110, 0x0000}, {0x40f8, 0x8020, 0x4114, 0x0000},
    {0x40f8, 0x8040, 0x4118, 0x0000}, {0x40f8, 0x8060, 0x411c, 0xffff},
    {0x0000, 0x0000, 0x0000, 0x0000}, {0x0000, 0x0000, 0x0000, 0x0000}};

OamAttr *sMiniGmGroupTabs[] = {
    data_ov005_020c2df8, data_ov005_020c2c90, data_ov005_020c2d68, data_ov005_020c2d20,
    data_ov005_020c2cd8, data_ov005_020c2db0, data_ov005_020c2c48, data_ov005_020c2e40,
};
OamAttr *sMiniGmExitButton = data_ov005_020c2e8c;
OamAttr *sMiniGmPageArrows[] = {data_ov005_020c2e9c, data_ov005_020c2e94};
OamAttr *sMiniGmDigits[] = {
    data_ov005_020c2ef4, data_ov005_020c2eec, data_ov005_020c2ee4, data_ov005_020c2edc,
    data_ov005_020c2ed4, data_ov005_020c2ecc, data_ov005_020c2ec4, data_ov005_020c2ebc,
    data_ov005_020c2eb4, data_ov005_020c2eac,
};
OamAttr *sMiniGmRecordLabels[] = {
    data_ov005_020c2f44, data_ov005_020c2f3c, data_ov005_020c2f34,
    data_ov005_020c2f2c, data_ov005_020c2f24,
};
OamAttr *sMiniGmNewBadge[] = {data_ov005_020c2f68, data_ov005_020c2f78};
OamAttr *sMiniGmSavingCaption = data_ov005_020c2f8c;
OamAttr *sMiniGmCaption = data_ov005_020c2fd0;
}
/* ---- arm9 / cross-overlay externals ---- */
void func_02013568(int a, int b, int v);
int  func_02013580(int a, int b);
void func_02013598(int x);
int  func_020135d0(int r0);
int IsMinigameUnlocked(int n);
extern "C" void *LoadFile(int id);
extern "C" void DecompressLZ16(void *src, void *dst);

dScMiniGm_c *dScMiniGm_c_classInit() {
    return new dScMiniGm_c();
}

s32 dScMiniGm_c::InitResources()
{
    int i, j;
    const MgEntry *tbl;
    int i2;
    int flag;
    int j2, j3;
    const MgEntry *tbl2;
    volatile u16 charFill;
    volatile u16 scrFill;
    char *dst;
    void *f;

    LoadTextNarcs();
    LoadArchive(0);
    data_0209f5f8 = 0;
    Sound::LoadGroupAndSetBank(0x27, 5);
    Disable3dEngines();

    if (!(gSaveBlock.mg.unlocked & 0x80000000)) {
        tbl = sMiniGmRows;
        for (i = 0; i < 36; i++) {
            for (j = 0; j < 5; j++) {
                SetRecord(i, j, tbl->recordDefaults.v[j]);
            }
            tbl++;
        }
        gSaveBlock.mg.unlocked |= 0x80000000;
        gMgSaveDirty = 1;
    }

    tbl2 = sMiniGmRows;
    for (i2 = 0; i2 < 36; i2++) {
        flag = 0;
        if (tbl2->scoreCap != 0) {
            if (tbl2->ownRecords == 1) {
                if ((u32)GetRecord(i2, 0) > (u32)tbl2->scoreCap) {
                    flag = 1;
                }
            } else {
                for (j2 = 0; j2 < 5; j2++) {
                    if ((u32)GetRecord(i2, j2) > (u32)tbl2->scoreCap) {
                        flag = 1;
                    }
                }
            }
        }
        if (flag != 0) {
            for (j3 = 0; j3 < 5; j3++) {
                SetRecord(i2, j3, tbl2->recordDefaults.v[j3]);
            }
            gMgSaveDirty = 1;
        }
        tbl2++;
    }

    if (gMgMenuPage == 1) {
        mSubBgScrollX = 0xb0;
    } else {
        mSubBgScrollX = 0;
    }
    mGfxSlot = 0;

    GX::DisableAllBanks();
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    GX_SetBankForTex(GX_VRAM_TEX_0_A);
    GX_SetBankForBG(GX_VRAM_BG_128_B);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForOBJ(GX_VRAM_OBJ_64_E);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_F);

    gMainScreenLayers &= ~(DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG2 |
                           DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ);
    gSubScreenLayers &= ~0x1f;
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GXS_SetGraphicsMode(GX_BGMODE_0);

    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GX_SetBGCharOffset(0);
    GX_SetBGScrOffset(0);
    GX_DispOn();
    GXS_DispOn();
    G3X_SetFog(false, 0, 2, 0x1000);
    InitialiseVramGlobals();
    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    G3_LightColor(GX_LIGHTID_1, GX_RGB(31, 31, 31));
    func_020233f4();

    G2_SetBG2Priority(1);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                         GX_BG_CHARBASE_0x10000);
    G2_BG2Mosaic(false);
    G2_SetBG2Offset(0, 0);
    SetBg2Offset(0, 0);

    f = LoadFile(0x4e);
    DecompressLZ16(f, G2_GetBG2CharPtr());
    Deallocate(f);

    f = LoadFile(0x58);
    DC_FlushRange(f, 0x200);
    GX_LoadBGPltt(f, 0, 0x200);
    Deallocate(f);

    f = LoadFile(0x59);
    DecompressLZ16(f, G2_GetBG2ScrPtr());
    Deallocate(f);

    if (GetOwnerLanguage() == 5) {
        f = LoadFile(0xda);
    } else if (GetOwnerLanguage() == 4) {
        f = LoadFile(0xd7);
    } else if (GetOwnerLanguage() == 3) {
        f = LoadFile(0xd6);
    } else if (GetOwnerLanguage() == 2) {
        f = LoadFile(0xd5);
    } else {
        f = LoadFile(0xd8);
    }
    DecompressLZ16(f, G2_GetOBJCharPtr());
    Deallocate(f);

    f = LoadFile(0xd9);
    DC_FlushRange(f, 0x100);
    GX_LoadOBJPltt(f, 0, 0x100);
    Deallocate(f);

    G2S_SetBG2Priority(1);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                          GX_BG_CHARBASE_0x04000);
    G2S_BG2Mosaic(false);
    G2S_SetBG2Offset(0, 0);
    SetSubBg2Offset(0, 0);

    G2S_SetBG3Priority(3);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_512x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000,
                          GX_BG_CHARBASE_0x04000);
    G2S_BG3Mosaic(false);
    G2S_SetBG3Offset(mSubBgScrollX, 0);
    SetSubBg3Offset(mSubBgScrollX, 0);

    f = LoadFile(0x53);
    DecompressLZ16(f, G2S_GetBG2CharPtr());
    Deallocate(f);

    f = LoadFile(0x5a);
    DC_FlushRange(f, 0x200);
    GXS_LoadBGPltt(f, 0, 0x200);
    Deallocate(f);

    G2S_SetBG0Priority(2);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_512x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_23);
    G2S_BG0Mosaic(false);
    G2S_SetBG0Offset(mSubBgScrollX, 0);
    SetSubBg0Offset(mSubBgScrollX, 0);

    dst = (char *)G2S_GetBG0CharPtr();
    charFill = 0;
    MI_CpuFill16(dst, charFill, 0x8000);
    dst = (char *)G2S_GetBG0ScrPtr();
    scrFill = 0x1f;
    MI_CpuFill16(dst, scrFill, 0x1000);

    LoadGroupIcons();

    if (GetOwnerLanguage() == 5) {
        f = LoadFile(0xda);
    } else if (GetOwnerLanguage() == 4) {
        f = LoadFile(0xd7);
    } else if (GetOwnerLanguage() == 3) {
        f = LoadFile(0xd6);
    } else if (GetOwnerLanguage() == 2) {
        f = LoadFile(0xd5);
    } else {
        f = LoadFile(0xd8);
    }
    DecompressLZ16(f, G2S_GetOBJCharPtr());
    Deallocate(f);

    f = LoadFile(0xd9);
    DC_FlushRange(f, 0x100);
    GXS_LoadOBJPltt(f, 0, 0x100);
    Deallocate(f);

    mPageFlipped = 0;
    mArrowBobPhase = 0;
    mPrevPageTimer = 0;
    mNextPageTimer = 0;
    mExitTimer = 0;
    mIconBlinkPhase = 0;
    mStartLevel = 0;
    unk_060 = 0;
    unk_0a4 = 0;
    unk_0a8 = 0;
    mScrollDelay = 0;
    mGroupBase = gMgGroupBase;
    mExiting = 0;

    if (gMgSaveDirty == 1) {
        gSoftResetDisabled = 1;
    }
    gFrameStep = 1;
    if (data_0209b2fc != 0) {
        dScene_c::SetFaders(&gSceneWipe);
    }

    data_0209b2fc = 0;
    gMainScreenLayers |= DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ;
    gSubScreenLayers |= 0x11;
    gMainScreenLayers &= ~DISPLAY_LAYER_BG1;
    gSubScreenLayers &= ~2;
    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    gMainScreenLayers &= ~DISPLAY_LAYER_BG3;
    gSubScreenLayers |= 0xc;
    Sound::LoadAndSetMusic_Layer1(5);
    return 1;
}

void dScMiniGm_c::LoadGroupIcons() {
    int i;
    int rowB;
    int j;
    int n1;
    int n2;
    int m;
    int copyLen;
    int flushSize;
    int plttSize;
    int pInit;
    int qInit;
    int resetVal;
    volatile u16 valA; /* pin MIi_CpuClear16 src */
    volatile u16 valB; /* pin MIi_CpuClear16 src */
    struct Off9 tbl;
    void *f;
    int fill;
    int nbytes;
    int t;

    f = LoadFile(sMiniGmBgFiles[gMgGroupBase]);
    DecompressLZ16(f, G2S_GetBG2ScrPtr());
    Deallocate(f);

    f = LoadFile(sMiniGmBgFiles[gMgGroupBase + 4]);
    DecompressLZ16(f, G2S_GetBG3ScrPtr());
    Deallocate(f);

    i = 0;
    j = 0;
    n2 = 0;
    qInit = 0;
    n1 = 0;
    m = 0;
    pInit = 0;
    resetVal = 0;
    copyLen = 0xc0;
    flushSize = 0x200;
    t = 0x20;
    fill = 0x1f;
    nbytes = 2;
    plttSize = t;

    do {
        tbl = sMiniGmIconScreenOffsets;
        if (IsUnlocked(gMgGroupBase + j) == 0) {
            int off;
            int row;
            int col;
            int destOff;
            char *scr;

            off = tbl.v[i];
            row = n1;
            do {
                col = n2;
                destOff = off << 1;
                do {
                    char *dstp;
                    scr = (char *)G2S_GetBG0ScrPtr();
                    dstp = scr + destOff;
                    dstp = destOff ? dstp : dstp; /* register-dest pin; arms identical */
                    valA = (u16)fill;
                    MI_CpuFill16(dstp, valA, nbytes);
                    destOff += 2;
                    off += 1;
                    col += 1;
                } while (col < 6);
                off += 0x1a;
                row += 1;
            } while (row < 9);
        } else {
            int row;
            int destOff;
            int idx;
            char *ch;
            char *scr;
            int off;
            int q;
            int p;

            f = LoadFile(sMiniGmIconChars[gMgGroupBase + j]);
            DecompressLZ16(f, (char *)G2S_GetBG0CharPtr() + 0x8000);
            Deallocate(f);

            row = m;
            destOff = row;
            do {
                idx = mGfxSlot;
                ch = (char *)G2S_GetBG0CharPtr();
                scr = ch + (idx / 4) * 0x2400 + (idx % 4) * 0xc0 + destOff;
                ch = (char *)G2S_GetBG0CharPtr();
                MI_CpuCopy32(ch + 0x8000 + destOff, scr, copyLen);
                destOff += 0x400;
            } while (++row < 9);

            f = LoadFile(sMiniGmIconPalettes[gMgGroupBase + j]);
            DC_FlushRange(f, flushSize);
            GXS_LoadBGPltt(f, mGfxSlot << 5, plttSize);
            Deallocate(f);

            mGfxSlots[i] = mGfxSlot;

            off = tbl.v[i];
            rowB = pInit;
            p = pInit;
            do {
                q = qInit;
                destOff = off << 1;
                do {
                    char *dstp;
                    idx = mGfxSlot;
                    scr = (char *)G2S_GetBG0ScrPtr();
                    dstp = scr + destOff;
                    dstp = idx ? dstp : dstp; /* register-dest pin; arms identical */
                    valB = (u16)(q + (p + ((idx % 4) * 6 + ((idx / 4) * 0x120 + (idx << 12)))));
                    MI_CpuFill16(dstp, valB, nbytes);
                    destOff += 2;
                    off += 1;
                    q += 1;
                } while (q < 6);
                off += 0x1a;
                p += 0x20;
                rowB += 1;
            } while (rowB < 9);

            {
                mGfxSlot++;
                if (mGfxSlot >= 10)
                    mGfxSlot = resetVal;
            }
        }
        j += 4;
        i += 1;
    } while (i < 9);
}

void dScMiniGm_c::LoadIconPalette(int index) {
    void *file = LoadFile(sMiniGmIconPalettes[index]);
    DC_FlushRange(file, 0x200);
    u32 slot = mGfxSlots[index / 4];
    GXS_LoadBGPltt((u16 *)file + 16, slot << 5, 0x20);
    Deallocate(file);
}

s32 dScMiniGm_c::CleanupResources()
{
    if (gMgLastSelected[0] >= 0) {
        ClearNew(gMgLastSelected[0]);
    }
    Sound::UnsetPlayerVoiceGroup();
    return 1;
}

s32 dScMiniGm_c::Behavior()
{
    if ((data_020a0e5a[gActivePlayerSlot << 1] & 0xfff) != 0) {
        func_02012790(0xe);
    }
    if (gMgSaveDirty == 1) {
        SaveIfDirty();
        return 1;
    }
    RandomIntInternal(&data_0209d4b8.value);
    RandomIntInternal(&data_0209e650.value);

    if (mPrevPageTimer > 0) {
        mPrevPageTimer -= 1;
        if (mPrevPageTimer == 0) {
            gMgMenuPage = 0;
            mPageFlipped = 1;
        }
    } else if (mNextPageTimer > 0) {
        mNextPageTimer -= 1;
        if (mNextPageTimer == 0) {
            gMgMenuPage = 1;
            mPageFlipped = 1;
        }
    } else if (mExitTimer > 1) {
        mExitTimer -= 1;
        if (mExitTimer == 1) {
            dScene_c::SetAndStopColorFader();
            ExitMinigameMenu();
            Sound::StopLoadedMusic_Layer1(30);
            mExiting = 1;
            return 1;
        }
    } else {
        mArrowBobPhase += 1;
        if (mArrowBobPhase >= 0x40) mArrowBobPhase = 0;
        mIconBlinkPhase += 1;
        if (mIconBlinkPhase >= 0x40) mIconBlinkPhase = 0;
    }
    UpdateGroupTabs();
    UpdatePageArrows();
    UpdateGameSelect();
    UpdateExitButton();
    UpdatePageScroll();
    return 1;
}

void dScMiniGm_c::DrawRecords() {
    int end;
    int i;
    int type;
    int x, y;
    int val;
    unsigned int u;
    unsigned int major;
    unsigned int rem;
    unsigned int pct;

    if (mPageFlipped == 1)
        return;

    if (gMgMenuPage == 1) {
        end = 8;
        i = 6;
    } else {
        end = 5;
        i = 0;
    }
    for (; i <= end; i++) {
        if (IsUnlocked(gMgGroupBase + i * 4) != 0) {
            type = sMiniGmRows[gMgGroupBase + i * 4].scoringMode;
            if (type != 0) {
                if (type == 1) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    OAM::RenderSub(sMiniGmRecordLabels[0], x - 16, y, -1, 0);
                    val = GetRecord(gMgGroupBase + i * 4, 0);
                    DrawNumber(x - 4, y, val);
                } else if (type == 2) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    OAM::RenderSub(sMiniGmRecordLabels[1], x - 16, y, -1, 0);
                    val = GetRecord(gMgGroupBase + i * 4, 0);
                    DrawNumber(x - 4, y, val);
                } else if (type == 4) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    OAM::RenderSub(sMiniGmRecordLabels[2], x - 16, y, -1, 0);
                    val = GetRecord(gMgGroupBase + i * 4, 0);
                    DrawNumber(x - 4, y, val);
                } else if (type == 3) {
                    val = GetRecord(gMgGroupBase + i * 4, 0);
                    x = sMiniGmRecordPos[i].x;
                    y = sMiniGmRecordPos[i].y;
                    DrawNumber(x - 0x14, y, val);
                } else if (type == 5) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    val = GetRecord(gMgGroupBase + i * 4, 0);
                    u = val;
                    major = u / 60u;
                    DrawNumber(x - 0x10, y, major);
                    OAM::RenderSub(sMiniGmRecordLabels[4], x, y - 8, -1, 0);

                    val = GetRecord(gMgGroupBase + i * 4, 0);
                    u = val;
                    rem = u % 60u;
                    pct = rem * 100u;
                    DrawNumber(x + 8, y, pct / 600u);

                    val = GetRecord(gMgGroupBase + i * 4, 0);
                    u = val;
                    rem = u % 60u;
                    pct = rem * 100u / 60u;
                    DrawNumber(x + 0x10, y, pct % 10u);
                }
            }
        }
    }
}

void dScMiniGm_c::DrawNumber(int x, int y, int val) {
    if (val == 0) {
        OAM::RenderSub(sMiniGmDigits[0], x, y, -1, 0);
        return;
    }
    int n = 1;
    {
        int q = val;
        while (q >= 10) {
            q = q / 10;
            n++;
        }
    }
    int xx = (n - 1) * 8;
    if (val <= 0)
        return;
    do {
        int dig = val % 10;
        OAM::RenderSub(sMiniGmDigits[dig], x + xx, y, -1, 0);
        val = val / 10;
        xx -= 8;
    } while (val > 0);
}

void dScMiniGm_c::DrawNewBadges() {
    int g = mIconBlinkPhase >= 0x20;
    int start, end;
    if (gMgMenuPage == 1) {
        start = 6;
        end = 8;
    } else {
        start = 0;
        end = 5;
    }
    if (mPageFlipped == 1)
        return;
    int i;
    for (i = start; i <= end; i++) {
        if (IsUnlocked(gMgGroupBase + i * 4) == 0)
            continue;
        if (IsNew(gMgGroupBase + i * 4) == 0)
            continue;
        OAM::RenderSub(sMiniGmNewBadge[g], sMiniGmBadgePos[i].x, sMiniGmBadgePos[i].y + 0x30, -1,
                       0);
    }
}

s32 dScMiniGm_c::Render()
{
    int i;
    int blink;
    int y1;
    int y0;
    int t;
    int scale;

    if (gMgSaveDirty == 1) {
        OAM::Render(0, sMiniGmSavingCaption, 128, 164, -1, 0, 0x1000, 0x1000, 0, -1);
    } else {
        OAM::Render(0, sMiniGmCaption, 128, 164, -1, 0, 0x1000, 0x1000, 0, -1);
    }

    y0 = 0x18;
    y1 = 0x28;
    for (i = 0; i < 4; i++) {
        int k = i;
        int j;
        if (mIconBlinkPhase >= 0x20)
            blink = 1;
        else
            blink = 0;
        for (j = 0; j < 9; j++) {
            if (IsUnlocked(sMiniGmGroupBases[i] + j * 4) != 0 &&
                IsNew(j * 4 + sMiniGmGroupBases[i]) != 0) {
                OAM::RenderSub(sMiniGmNewBadge[blink], 24, y1, -1, 0);
                break;
            }
        }
        if (mGroupBase != sMiniGmGroupBases[i])
            k += 4;
        OAM::RenderSub(sMiniGmGroupTabs[k], 24, y0, -1, 0);
        y1 += 0x30;
        y0 += 0x30;
    }

    if (mPageFlipped == 0) {
        if (gMgMenuPage != 0) {
            t = mPrevPageTimer;
            if (t < 0xa)
                scale = 0x1000;
            else if (t < 0xe)
                scale = (((t - 0xa) << 12) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 12) / 16) + 0x1000;
            if (mArrowBobPhase < 0x20)
                OAM::Render(1, sMiniGmPageArrows[0], 80 - mArrowBobPhase / 2, 96, -1, 0, scale, 0);
            else
                OAM::Render(1, sMiniGmPageArrows[0], 80 - (64 - mArrowBobPhase) / 2, 0x60, -1, 0,
                            scale, 0);
        } else if (HasSecondPage() != 0) {
            t = mNextPageTimer;
            if (t < 0xa)
                scale = 0x1000;
            else if (t < 0xe)
                scale = (((t - 0xa) << 12) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 12) / 16) + 0x1000;
            if (mArrowBobPhase < 0x20)
                OAM::Render(1, sMiniGmPageArrows[1], mArrowBobPhase / 2 + 224, 96, -1, 0, scale, 0);
            else
                OAM::Render(1, sMiniGmPageArrows[1], (64 - mArrowBobPhase) / 2 + 224, 0x60, -1, 0,
                            scale, 0);
        }
    }

    DrawRecords();
    DrawNewBadges();

    t = mExitTimer;
    if (t < 0x14)
        scale = 0x1000;
    else if (t < 0x18)
        scale = (((t - 0x14) << 12) / 16) + 0x1000;
    else
        scale = (((0x1c - t) << 12) / 16) + 0x1000;
    OAM::Render(1, sMiniGmExitButton, 232, 160, -1, 0, scale, 0);
    return 1;
}

void dScMiniGm_c::OnPendingDestroy()
{
}

void dScMiniGm_c::UpdateGroupTabs() {
    if (gSceneFader->IsAtStart() == 0)
        return;
    if (mExiting != 0)
        return;
    if (mPageFlipped == 1)
        return;
    if (mPrevPageTimer > 0)
        return;
    if (mNextPageTimer > 0)
        return;
    if (mExitTimer > 0)
        return;

    if (mScrollDelay > 0) {
        mScrollDelay--;
        if (mScrollDelay == 0) {
            int f58 = mGroupBase;
            if (f58 != gMgGroupBase) {
                gMgGroupBase = f58;
                LoadGroupIcons();
                mScrollDelay = 30;
            }
        }
    }

    {
        int flag = 0;
        unsigned char idx = gActivePlayerSlot;
        unsigned char v = gTouchInfo[idx].held;
        if (v != 0) {
            if (gTouchInfo[idx].edge != 0)
                flag = 1;
        }
        if (flag != 0 || (mScrollDelay <= 0 && v != 0)) {
            int i;
            int lo;
            int hi;
            for (i = 0, lo = 2, hi = 0x2e; i < 4; i++, lo += 0x30, hi += 0x30) {
                int e = sMiniGmGroupBases[i];
                if (gMgGroupBase != e) {
                    unsigned char ix = gActivePlayerSlot;
                    TouchInfo *touch = &gTouchInfo[ix];
                    if (touch->x <= 0x30 && touch->y >= lo && touch->y <= hi) {
                        gMgGroupBase = e;
                        mGroupBase = e;
                        func_02012790(0);
                        LoadGroupIcons();
                        mScrollDelay = 30;
                    }
                }
            }
            return;
        }

        if (mScrollDelay <= 0)
            return;
        if (v == 0)
            return;
        {
            int i;
            int lo;
            int hi;
            for (i = 0, lo = 2, hi = 0x2e; i < 4; i++, lo += 0x30, hi += 0x30) {
                int e = sMiniGmGroupBases[i];
                if (mGroupBase != e) {
                    unsigned char ix = gActivePlayerSlot;
                    TouchInfo *touch = &gTouchInfo[ix];
                    if (touch->x <= 0x30 && touch->y >= lo && touch->y <= hi) {
                        mGroupBase = e;
                        func_02012790(0);
                        mScrollDelay = 30;
                    }
                }
            }
        }
    }
}

void dScMiniGm_c::UpdatePageArrows() {
    if (gSceneFader->IsAtStart() == 0)
        return;
    if (mExiting != 0)
        return;
    if (mPageFlipped == 1)
        return;
    if (mPrevPageTimer > 0)
        return;
    if (mNextPageTimer > 0)
        return;
    if (mExitTimer > 0)
        return;
    if (HasSecondPage() == 0)
        return;

    {
        int has = 0;
        u8 idx = gActivePlayerSlot;
        if (gTouchInfo[idx].held) {
            if (gTouchInfo[idx].edge)
                has = 1;
        }
        if (has == 0) return;
        if (gMgMenuPage == 0) {
            u8 v = gTouchInfo[idx].x;
            if (v < 0xd8) return;
            if (v > 0xf8) return;
            if (gTouchInfo[idx].y < 0x40)
                return;
            if (gTouchInfo[idx].y > 0x80)
                return;
            func_02012790(0);
            mNextPageTimer = 18;
            return;
        } else {
            u8 v = gTouchInfo[idx].x;
            if (v < 0x38) return;
            if (v > 0x58) return;
            if (gTouchInfo[idx].y < 0x40)
                return;
            if (gTouchInfo[idx].y > 0x80)
                return;
            func_02012790(0);
            mPrevPageTimer = 18;
            return;
        }
    }
}

void dScMiniGm_c::UpdateGameSelect()
{
    int sel;
    int ok;
    int row;

    if (gSceneFader->IsAtStart() == 0)
        return;
    if (mExiting != 0) return;
    if (mPageFlipped == 1) return;
    if (mPrevPageTimer > 0) return;
    if (mNextPageTimer > 0) return;
    if (mExitTimer > 0) return;

    sel = -1;
    ok = 0;
    if (gTouchInfo[gActivePlayerSlot].held != 0) {
        if (gTouchInfo[gActivePlayerSlot].edge != 0)
            ok = 1;
    }
    if (ok != 0) {
        if (gMgMenuPage == 0) {
            for (row = 0; row < 2; row++) {
                for (int col = 0; col < 3; col++) {
                    if (gTouchInfo[gActivePlayerSlot].x >= 0x40 + col * 0x30 &&
                        gTouchInfo[gActivePlayerSlot].x < 0x70 + col * 0x30 &&
                        gTouchInfo[gActivePlayerSlot].y >= 0x18 + row * 0x48 &&
                        gTouchInfo[gActivePlayerSlot].y < 0x60 + row * 0x48)
                        sel = gMgGroupBase + (row * 0xc + col * 4);
                }
            }
        } else {
            for (int col = 0; col < 3; col++) {
                if (gTouchInfo[gActivePlayerSlot].x >= 0x60 + col * 0x30 &&
                    gTouchInfo[gActivePlayerSlot].x < 0x90 + col * 0x30 &&
                    gTouchInfo[gActivePlayerSlot].y >= 0x38 &&
                    gTouchInfo[gActivePlayerSlot].y < 0x80)
                    sel = gMgGroupBase + (0x18 + col * 4);
            }
        }
    }

    if (sel < 0)
        return;
    if (IsUnlocked(sel) == 0)
        return;

    {
        const MgEntry *src = &sMiniGmRows[sel];
        gMgLastSelected[0] = sel;
        gMgMenuRow.id = src->id;
        gMgMenuRow.sceneParam = src->sceneParam;
        gMgMenuRow.scoringMode = src->scoringMode;
        gMgMenuRow.scoreCap = src->scoreCap;
        gMgMenuRow.ownRecords = src->ownRecords;
        gMgMenuRow.recordDefaults = src->recordDefaults;
        gMgMenuRow.musicTrack = src->musicTrack;
        gMgMenuRow.musicMode = src->musicMode;
        gMgMenuRow.level = src->level;
    }
    LoadIconPalette(sel);
    func_02012790(0x1e);
    dScene_c::SetFaders(&gSceneWipe);
    gSceneWipe.SetType(0);
    gMgMenuRow.level = mStartLevel;
    dScene_c::StartSceneFade(sMiniGmRows[gMgLastSelected[0]].id,
                             sMiniGmRows[gMgLastSelected[0]].sceneParam, 0);
    Sound::StopLoadedMusic_Layer1(30);
    mExiting = 1;
    data_0209b2fc = 1;
}

void dScMiniGm_c::UpdateExitButton() {
    if (gSceneFader->IsAtStart() == 0)
        return;
    if (mExiting != 0)
        return;
    if (mPageFlipped == 1)
        return;
    if (mPrevPageTimer > 0)
        return;
    if (mNextPageTimer > 0)
        return;
    if (mExitTimer > 0)
        return;
    unsigned int i = gActivePlayerSlot;
    bool ok = false;
    if (gTouchInfo[i].held != 0) {
        if (gTouchInfo[i].edge != 0)
            ok = true;
    }
    if (!ok) return;
    unsigned char x = gTouchInfo[i].x;
    if (x < 0xd8) return;
    if (x > 0xf8) return;
    unsigned char y = gTouchInfo[i].y;
    if (y < 0x90) return;
    if (y > 0xb0) return;
    func_02012790(0x63);
    mExitTimer = 28;
}

void dScMiniGm_c::UpdatePageScroll() {
    if (gSceneFader->IsAtStart() == 0)
        return;
    if (mExiting != 0)
        return;
    if (mPrevPageTimer > 0)
        return;
    if (mNextPageTimer > 0)
        return;
    if (mExitTimer > 0)
        return;
    if (gMgMenuPage == 0) {
        if (mSubBgScrollX >= 0) {
            mSubBgScrollX -= 8;
            if (mSubBgScrollX <= 0) {
                mSubBgScrollX = 0;
                mPageFlipped = 0;
            }
        }
    } else {
        if (mSubBgScrollX <= 0xb0) {
            mSubBgScrollX += 8;
            if (mSubBgScrollX >= 0xb0) {
                mSubBgScrollX = 0xb0;
                mPageFlipped = 0;
            }
        }
    }
    gSubBg0HOffset = (short)mSubBgScrollX;
    gSubBg3HOffset = (short)mSubBgScrollX;
}

int dScMiniGm_c::HasSecondPage() {
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 6; j < 9; j++) {
            if (IsUnlocked(j * 4 + i))
                return 1;
        }
    }
    return 0;
}

int dScMiniGm_c::IsUnlocked(int n) {
    if (n < 8) {
        return 1;
    }
    return IsMinigameUnlocked(n - 8);
}

int dScMiniGm_c::IsNew(int n) {
    if (n < 8) {
        return func_020135d0(n + 0x1c);
    } else {
        return func_020135d0(n - 8);
    }
}

void dScMiniGm_c::ClearNew(int n) {
    int seen = IsNew(n);
    if (seen == 0)
        return;
    if (n < 8)
        func_02013598(n + 0x1c);
    else
        func_02013598(n - 8);
    gMgSaveDirty = 1;
}

int dScMiniGm_c::GetSaveTable(int i) {
    return ((u32)sMiniGmRows[i].sceneParam >> 8) & 0xff;
}

int dScMiniGm_c::GetRecord(int b, int c) {
    int res = GetSaveTable(b);
    return func_02013580(res, c);
}

void dScMiniGm_c::SetRecord(int a1, int r2, int r3) {
    int r5 = r2;
    int r4 = r3;
    int r0 = GetSaveTable(a1);
    func_02013568(r0, r5, r4);
}

void dScMiniGm_c::SaveIfDirty() {
    if (gMgSaveDirty != 1) return;
    if (gSceneFader->IsAtStart() == 0)
        return;
    SaveData::SaveMinigames(&data_0209cae4);
    gMgSaveDirty = 0;
    gSoftResetDisabled = 0;
}
