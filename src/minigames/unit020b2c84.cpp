//cpp
/* unit020b2c84 -- the 44 functions of the ov004 linker unit 0x020b2c84..0x020b4aa4.
 *
 * This is the unlabelled run between the end of dScMgBase_c's unit and the unit
 * that starts at 0x020b4aa4. No class label, RTTI record or vtable falls in it,
 * so the file keeps the unit<addr> name. What the functions do, from their
 * bodies: the first group manages a table of 29 loaded files (Mg_FreeBannerFiles
 * frees them, Mg_LoadBannerFiles loads them by language, Banner_Init
 * decompresses one into a graphics slot and sets up an MgBanner to show it), and
 * the rest are the behavior and render handlers of MgBanner, the 0x134-byte record
 * of which gMgBanners holds three. An MgBanner draws its graphic as a
 * list of OAM records; its behavior moves it toward mTargetX/mTargetY or
 * animates it against mTimer.
 *
 * Edges. 0x020b2c84 is the first function after func_ov004_020b2c80, the last
 * of the three destructor helpers (func_ov004_020b2c58, func_ov004_020b2c7c,
 * func_ov004_020b2c80) that __sinit_ov004_020b948c registers for the preceding
 * unit; those stay with dScMgBase_c. 0x020b4aa4 is the next function after
 * _ZN8MgBannerD1Ev, the last of the three destructor helpers
 * (_ZN8MgBannerC1Ev, __arraydtor$954, _ZN8MgBannerD1Ev) that
 * __sinit_ov004_020b955c registers for this unit's static array at
 * gMgBanners; __arraydtor$954 runs __cxa_vec_cleanup over it.
 * The unit emits one data object of its own, the anonymous initializer template
 * of Mg_LoadBannerFiles's local array at 0x020bc6e8 (0xe8 bytes), which this
 * file's .data claim covers. Everything else it touches is reached by extern.
 *
 * Emission order. `#pragma defer_codegen off` is load-bearing here: with codegen
 * deferred mwccarm emits the functions' sections in reverse source order
 * (measured: Mg_FreeBannerFiles comes out last), and the ROM has them
 * ascending. Source is therefore ROM-ascending.
 *
 * The bodies were matched as one-function C or C++ sources and keep that
 * spelling under extern "C" and their ROM names.
 *
 * Numbered names. mwccarm numbers its generated names from one counter that
 * every named parameter (prototypes included), local and temporary before a
 * definition advances. The array gMgBanners at the end of the file
 * gets an __arraydtor$N thunk and an @N registration node (the manifest's
 * defined_symbol_renames hold the current N); symbols.txt names the thunk
 * __arraydtor$954, and the manifest renames both onto their configured
 * names, so an edit here that moves the count re-keys those two rows. The
 * two function-local Vector2 statics (Banner_StartWaveBurst,
 * Banner_SetupZoomBurst) are renamed the same way, onto the ov004 .bss names
 * at 0x020bf3ec..0x020bf41c that this unit owns.
 *
 * Member-pointer records. The pmf_Banner_* words are the {function, 0}
 * records mwccarm emits for each `&MgBanner::handler` use, and data_02086b58
 * is __ptmf_null; they are declared here as ElemFn, so the handlers assign,
 * compare and index the three per-style tables as member pointers.
 *
 * Retired one-function sources (ROM address order):
 *   0x020b2c84  Mg_FreeBannerFiles
 *   0x020b2cb8  Mg_LoadBannerFiles
 *   0x020b3194  Banner_Free
 *   0x020b31b4  Banner_Draw
 *   0x020b321c  Banner_Update
 *   0x020b3278  Banner_Init
 *   0x020b35d8  Banner_DrawLevelNumber
 *   0x020b3698  Banner_SetupNone
 *   0x020b369c  Banner_DrawSpin
 *   0x020b37c4  Banner_UpdateSpin
 *   0x020b37f0  Banner_StartSpin
 *   0x020b3834  Banner_UpdateDrop
 *   0x020b3888  Banner_SetupDrop
 *   0x020b38ac  Banner_DrawWave
 *   0x020b3978  Banner_UpdateWave
 *   0x020b39a4  Banner_StartWaveBurst
 *   0x020b3b38  Banner_DrawSqueeze
 *   0x020b3c58  Banner_UpdateFall
 *   0x020b3c9c  Banner_SetupFall
 *   0x020b3cb8  Banner_DrawPopSpinA
 *   0x020b3e9c  Banner_DrawPopSpinB
 *   0x020b4080  Banner_UpdatePopSpin
 *   0x020b40ac  Banner_SetupPopSpin
 *   0x020b40c0  Banner_IsPressFinished
 *   0x020b410c  Banner_DrawPressed
 *   0x020b4214  Banner_UpdatePressed
 *   0x020b422c  Banner_Press
 *   0x020b42c0  Banner_DrawPlain
 *   0x020b433c  Banner_SetupZoom
 *   0x020b4360  Banner_DrawZoomA
 *   0x020b45c0  Banner_DrawZoomB
 *   0x020b4820  Banner_UpdateZoom
 *   0x020b484c  Banner_SetupZoomBurst
 *   0x020b49b8  Banner_UpdateSlideFromRight
 *   0x020b49e4  Banner_SetupSlideFromRight
 *   0x020b49f0  Banner_UpdateSlideFromLeft
 *   0x020b4a1c  Banner_SetupSlideFromLeft
 *   0x020b4a28  Banner_UpdateSlideFromBottom
 *   0x020b4a40  Banner_SetupSlideFromBottom
 *   0x020b4a4c  Banner_UpdateSlideFromTop
 *   0x020b4a64  Banner_SetupSlideFromTop
 *   0x020b4a70  _ZN8MgBannerC1Ev
 *   0x020b4a7c  __arraydtor$954
 *   0x020b4aa0  _ZN8MgBannerD1Ev
 */

#pragma defer_codegen off

#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Vector2.h"
#include "Sound.h"
#include "math/Approach.h"
#include "nitro/gx/gxcommon.h"
#include "nitro/gx/gx_bgcnt.h"
#include "MWCPlusLib.h"
#include "dScMgBase_c.h"
#include "LevelRng.h"
#include "MgBanner.h"
#include "nitro/fx/fx_cp.h"
#include "math/CalcLib.h"

struct M { int _00, _01, _10, _11; };
struct SlotCharNos {
    int v[3];
};
/* The banner files: 29 graphics, loaded together. An MgBanner whose mFileIndex
   is MG_BANNER_FILE_NUM shows nothing. */
#define MG_BANNER_FILE_NUM 29

struct Entry {
    int file;
    int *sprites;
};
struct EntryTable {
    Entry e[MG_BANNER_FILE_NUM];
};
struct OamAttr;

extern "C" {
void Deallocate(void *ptr);
int GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
int Mg_CopySpriteList(int a, void *b, int c, int d);
void func_0203d704(int *o, int *a, int *b);
void func_0203d388(int *p, int angle);
void Mg_SpawnFx(int state, short timer, Vector2 *pos, Vector2 *vel, Vector2 *accel);
void Mg_RenderSpriteOnMtx(void *a0, int a1, int a2, int a3, int a4, M *a5);
void Mg_RenderSpriteOn(OamAttr *a, int b, int c, int d, int e);
int Mg_GetLevel(void);
void Mg_DrawBigNumber(int a, int b, int c, int d, int e, int f, int g);
}
void Banner_DrawPlain(MgBanner *e);
void Banner_StartSpin(MgBanner *e);
void Banner_StartWaveBurst(MgBanner *e);

/* The first OBJ character number of each graphics slot, {0x380, 0x300, 0x340}
   (MG_BANNER_SLOTn_CHARS / 32). Banner_Init copies it whole to the stack and
   indexes the copy: the code of a local `int charNo[3] = {...}`, whose
   initializer template the ROM keeps here in .data. Written that way, mwccarm
   emits the same Banner_Init but puts the template in an anonymous .rodata
   object outside this unit's claimed .data (measured), so the copy stays. */
SlotCharNos sMgBannerGfxSlotCharNo = {{0x380, 0x300, 0x340}};
extern "C" {
extern int data_ov004_020bb16c;
extern int data_ov004_020ba1f4;
extern int data_ov004_020bb554;
extern int data_ov004_020bb1ac;
extern int data_ov004_020ba214;
extern int data_ov004_020ba734;
extern int data_ov004_020bb59c;
extern int data_ov004_020ba134;
extern int data_ov004_020ba174;
extern int data_ov004_020babec;
extern int data_ov004_020bbc44;
extern int data_ov004_020ba824;
extern int data_ov004_020bab44;
extern int data_ov004_020ba464;
extern int data_ov004_020ba1d4;
extern int data_ov004_020ba764;
extern int data_ov004_020ba48c;
extern int data_ov004_020ba4b4;
extern int data_ov004_020ba7c4;
extern int data_ov004_020bb434;
extern int data_ov004_020ba43c;
extern int data_ov004_020ba414;
extern int data_ov004_020bb70c;
extern int data_ov004_020bb02c;
extern int data_ov004_020ba4dc;
extern int data_ov004_020bb06c;
extern int data_ov004_020bb674;
extern int data_ov004_020ba354;
extern int data_ov004_020bb3ac;
int sMgMsgSuperEsperSpritesEnglish = (int)&data_ov004_020bb16c;
int sMgMsgNoPairSpritesEnglish = (int)&data_ov004_020ba1f4;
int sMgMsgFullHouseSpritesEnglish = (int)&data_ov004_020bb554;
int sMgMsgTimeOverSpritesEnglish = (int)&data_ov004_020bb1ac;
int sMgMsgQuitSpritesEnglish = (int)&data_ov004_020ba214;
int sMgMsgYouWinSpritesEnglish = (int)&data_ov004_020ba734;
int sMgMsgRetrySpritesEnglish = (int)&data_ov004_020bb59c;
int sMgMsgGoalSpritesEnglish = (int)&data_ov004_020ba134;
int sMgMsgHateSpritesEnglish = (int)&data_ov004_020ba174;
int sMgMsgPerfectSpritesEnglish = (int)&data_ov004_020babec;
int sMgMsgWhereSpritesEnglish = (int)&data_ov004_020bbc44;
int sMgMsgTooBadSpritesEnglish = (int)&data_ov004_020ba824;
int sMgMsgLikeSpritesEnglish = (int)&data_ov004_020bab44;
int sMgMsgFiveCardSpritesEnglish = (int)&data_ov004_020ba464;
int sMgMsgLoveSpritesEnglish = (int)&data_ov004_020ba1d4;
int sMgMsgTwoPairSpritesEnglish = (int)&data_ov004_020ba764;
int sMgMsgEsperSpritesEnglish = (int)&data_ov004_020ba48c;
int sMgMsgLevelClearSpritesEnglish = (int)&data_ov004_020ba4b4;
int sMgMsgGiveUpSprites = (int)&data_ov004_020ba7c4;
int sMgMsgResultHiScoreSpritesEnglish = (int)&data_ov004_020bb434;
int sMgMsgOnePairSpritesEnglish = (int)&data_ov004_020ba43c;
int sMgMsgStartSpritesEnglish = (int)&data_ov004_020ba414;
int sMgMsgDislikeSpritesEnglish = (int)&data_ov004_020bb70c;
int sMgMsgThreeCardSpritesEnglish = (int)&data_ov004_020bb02c;
int sMgMsgLevelSpritesEnglish = (int)&data_ov004_020ba4dc;
int sMgMsgFourCardSpritesEnglish = (int)&data_ov004_020bb06c;
int sMgMsgHiScoreSpritesEnglish = (int)&data_ov004_020bb674;
int sMgMsgDrawSpritesEnglish = (int)&data_ov004_020ba354;
int sMgMsgGameOverSpritesEnglish = (int)&data_ov004_020bb3ac;
}
static EntryTable sMgBannerFileTemplate = {{{0, 0}, {0x140, &sMgMsgGiveUpSprites}}};
extern OamAttr *data_ov006_021346bc;
extern int sMgTopScreenYOffset;
extern int sMgBannerButtonsArmed;
extern ElemFn pmf_Banner_UpdateWave;
extern ElemFn pmf_Banner_DrawWave;
extern ElemFn pmf_Banner_UpdatePressed;
extern ElemFn pmf_Banner_DrawPressed;
extern ElemFn pmf_Banner_DrawSpin;
extern ElemFn pmf_Banner_UpdateSpin;
extern ElemFn pmf_Banner_UpdatePressed_2;
extern unsigned char sMgBannerGfxSlotUsed[];
/* Where Banner_Init decompresses a banner for each graphics slot: the byte
   offset into the OBJ character VRAM of each screen it draws on. */
enum MgBannerCharOffset {
    MG_BANNER_SLOT0_CHARS = 0x7000,
    MG_BANNER_SLOT1_CHARS = 0x6000,
    MG_BANNER_SLOT2_CHARS = 0x6800
};
extern ElemFn sBannerUpdateTable[];
extern ElemFn sBannerSetupTable[];
extern ElemFn sBannerDrawTable[];
extern void *sMgBannerFiles[];
extern int sMgBannerSprites[];

extern int data_ov004_020b9f68;
extern int data_ov004_020b9f28;
extern int data_ov004_020b9ec8;
extern int data_ov004_020b9e64;
extern int data_ov004_020b9e34;
extern int data_ov004_020b9ec0;
extern int data_ov004_020b9e28;
extern int data_ov004_020b9e10;
extern int data_ov004_020ba004;
extern int data_ov004_020b9fe8;
extern int data_ov004_020ba01c;
extern int data_ov004_020b9fe4;
extern int data_ov004_020b9e74;
extern int data_ov004_020b9ff0;
extern int data_ov004_020b9f9c;
extern int data_ov004_020ba008;
extern int data_ov004_020b9fa8;
extern int data_ov004_020b9f14;
extern int data_ov004_020b9ff8;
extern int data_ov004_020ba020;
extern int data_ov004_020b9e4c;
extern int data_ov004_020ba018;
extern int data_ov004_020b9fd4;
extern int data_ov004_020b9fb8;
extern int data_ov004_020b9f10;
extern int data_ov004_020b9f70;
extern int data_ov004_020b9f80;
extern int data_ov004_020b9fc0;
extern int data_ov004_020b9fcc;
extern int data_ov004_020b9f48;
extern int data_ov004_020b9eac;
extern int data_ov004_020b9fb0;
extern int data_ov004_020ba028;
extern int data_ov004_020b9eb4;
extern int data_ov004_020b9f50;
extern int data_ov004_020b9ec4;
extern int data_ov004_020b9fc4;
extern int data_ov004_020b9ff4;
extern int data_ov004_020b9f4c;
extern int data_ov004_020b9eb0;
extern int data_ov004_020b9e40;
extern int data_ov004_020b9e58;
extern int data_ov004_020b9e80;
extern int data_ov004_020b9e3c;
extern int data_ov004_020b9e24;
extern int data_ov004_020b9e44;
extern int data_ov004_020b9e08;
extern int data_ov004_020b9e20;
extern int data_ov004_020b9e60;
extern int data_ov004_020b9e00;
extern int data_ov004_020b9f90;
extern int data_ov004_020b9e5c;
extern int data_ov004_020b9e14;
extern int data_ov004_020b9e2c;
extern int data_ov004_020b9ee0;
extern int data_ov004_020b9e30;
extern int data_ov004_020ba014;
extern int data_ov004_020ba030;
extern int data_ov004_020b9fd0;
extern int data_ov004_020b9ea0;
extern int data_ov004_020b9e0c;
extern int data_ov004_020b9fe0;
extern int data_ov004_020b9f6c;
extern int data_ov004_020b9f44;
extern int data_ov004_020b9ef8;
extern int data_ov004_020b9fbc;
extern int data_ov004_020ba02c;
extern int data_ov004_020b9e04;
extern int data_ov004_020b9eec;
extern int data_ov004_020b9e94;
extern int data_ov004_020b9fa4;
extern int data_ov004_020b9ef4;
extern int data_ov004_020b9e54;
extern int data_ov004_020b9e38;
extern int data_ov004_020b9e18;
extern int data_ov004_020b9e1c;
extern int data_ov004_020b9fb4;
extern int data_ov004_020b9f98;
extern int data_ov004_020b9f58;
extern int data_ov004_020b9f00;
extern int data_ov004_020b9f8c;
extern int data_ov004_020b9f38;
extern int data_ov004_020b9ed8;
extern int data_ov004_020b9f94;
extern int data_ov004_020b9f20;
extern int data_ov004_020b9fac;
extern int data_ov004_020ba024;
extern int data_ov004_020b9f1c;
extern int data_ov004_020b9e48;
extern int data_ov004_020b9f74;
extern int data_ov004_020b9f64;
extern int data_ov004_020b9e84;
extern int data_ov004_020ba00c;
extern int data_ov004_020b9f34;
extern int data_ov004_020b9f24;
extern int data_ov004_020b9f3c;
extern int data_ov004_020b9fec;
extern int data_ov004_020b9f08;
extern int data_ov004_020b9ed0;
extern int data_ov004_020b9f84;
extern int data_ov004_020b9ecc;
extern int data_ov004_020b9edc;
extern int data_ov004_020b9f04;
extern int data_ov004_020b9e9c;
extern int data_ov004_020b9e6c;
extern int data_ov004_020b9fdc;
extern int data_ov004_020b9e88;
extern int data_ov004_020b9e68;
extern int data_ov004_020b9f40;
extern int data_ov004_020ba000;
extern int data_ov004_020b9f30;
extern int data_ov004_020b9f18;

int *sMgMsgWhereSpritesByLanguage[] = {&sMgMsgWhereSpritesEnglish, &data_ov004_020b9f68,
                                       &data_ov004_020b9f28, &data_ov004_020b9ec8,
                                       &data_ov004_020b9e64};
int *sMgMsgLikeSpritesByLanguage[] = {&sMgMsgLikeSpritesEnglish, &data_ov004_020b9e34,
                                      &data_ov004_020b9ec0, &data_ov004_020b9e28,
                                      &data_ov004_020b9e10};
int *sMgMsgLoveSpritesByLanguage[] = {&sMgMsgLoveSpritesEnglish, &data_ov004_020ba004,
                                      &data_ov004_020b9fe8, &data_ov004_020ba01c,
                                      &data_ov004_020b9fe4};
int *sMgMsgDislikeSpritesByLanguage[] = {&sMgMsgDislikeSpritesEnglish, &data_ov004_020b9e74,
                                         &data_ov004_020b9ff0, &data_ov004_020b9f9c,
                                         &data_ov004_020ba008};
int *sMgMsgEsperSpritesByLanguage[] = {&sMgMsgEsperSpritesEnglish, &data_ov004_020b9fa8,
                                       &data_ov004_020b9f14, &data_ov004_020b9ff8,
                                       &data_ov004_020ba020};
int *sMgMsgSuperEsperSpritesByLanguage[] = {&sMgMsgSuperEsperSpritesEnglish, &data_ov004_020b9e4c,
                                            &data_ov004_020ba018, &data_ov004_020b9fd4,
                                            &data_ov004_020b9fb8};
int *sMgMsgGoalSpritesByLanguage[] = {&sMgMsgGoalSpritesEnglish, &data_ov004_020b9f10,
                                      &data_ov004_020b9f70, &data_ov004_020b9f80,
                                      &data_ov004_020b9fc0};
int sMgMsgOnePairNcgByLanguage[] = {273, 274, 270, 271, 272};
int sMgMsgTwoPairNcgByLanguage[] = {278, 279, 275, 276, 277};
int sMgMsgFourCardNcgByLanguage[] = {288, 289, 285, 286, 287};
int *sMgMsgHateSpritesByLanguage[] = {&sMgMsgHateSpritesEnglish, &data_ov004_020b9fcc,
                                      &data_ov004_020b9f48, &data_ov004_020b9eac,
                                      &data_ov004_020b9fb0};
int sMgMsgFiveCardNcgByLanguage[] = {293, 294, 290, 291, 292};
int sMgMsgDrawNcgByLanguage[] = {303, 304, 300, 301, 302};
int sMgMsgNoPairNcgByLanguage[] = {359, 360, 356, 357, 358};
int *sMgMsgTimeOverSpritesByLanguage[] = {&sMgMsgTimeOverSpritesEnglish, &data_ov004_020ba028,
                                          &data_ov004_020b9eb4, &data_ov004_020b9f50,
                                          &data_ov004_020b9ec4};
int sMgMsgGameOverNcgByLanguage[] = {318, 319, 315, 316, 317};
int sMgMsgLevelClearNcgByLanguage[] = {339, 340, 336, 337, 338};
int sMgMsgHiScoreNcgByLanguage[] = {334, 335, 331, 332, 333};
int sMgMsgPerfectNcgByLanguage[] = {364, 365, 361, 362, 363};
int sMgMsgResultHiScoreNcgByLanguage[] = {374, 375, 371, 372, 373};
int sMgMsgQuitNcgByLanguage[] = {369, 370, 366, 367, 368};
int sMgMsgRetryNcgByLanguage[] = {379, 380, 376, 377, 378};
int sMgMsgLevelNcgByLanguage[] = {344, 345, 341, 342, 343};
int sMgMsgTooBadNcgByLanguage[] = {399, 400, 396, 397, 398};
int sMgMsgYouWinNcgByLanguage[] = {410, 411, 407, 408, 409};
int sMgMsgWhereNcgByLanguage[] = {404, 405, 401, 402, 403};
int sMgMsgLikeNcgByLanguage[] = {349, 350, 346, 347, 348};
int sMgMsgLoveNcgByLanguage[] = {354, 355, 351, 352, 353};
int sMgMsgDislikeNcgByLanguage[] = {298, 299, 295, 296, 297};
int sMgMsgEsperNcgByLanguage[] = {308, 309, 305, 306, 307};
int sMgMsgSuperEsperNcgByLanguage[] = {389, 390, 386, 387, 388};
int sMgMsgGoalNcgByLanguage[] = {324, 325, 321, 322, 323};
int *sMgMsgNoPairSpritesByLanguage[] = {&sMgMsgNoPairSpritesEnglish, &data_ov004_020b9fc4,
                                        &data_ov004_020b9ff4, &data_ov004_020b9f4c,
                                        &data_ov004_020b9eb0};
int *sMgMsgOnePairSpritesByLanguage[] = {&sMgMsgOnePairSpritesEnglish, &data_ov004_020b9e40,
                                         &data_ov004_020b9e58, &data_ov004_020b9e80,
                                         &data_ov004_020b9e3c};
int *sMgMsgTwoPairSpritesByLanguage[] = {&sMgMsgTwoPairSpritesEnglish, &data_ov004_020b9e24,
                                         &data_ov004_020b9e44, &data_ov004_020b9e08,
                                         &data_ov004_020b9e20};
int *sMgMsgThreeCardSpritesByLanguage[] = {&sMgMsgThreeCardSpritesEnglish, &data_ov004_020b9e60,
                                           &data_ov004_020b9e00, &data_ov004_020b9f90,
                                           &data_ov004_020b9e5c};
int *sMgMsgFourCardSpritesByLanguage[] = {&sMgMsgFourCardSpritesEnglish, &data_ov004_020b9e14,
                                          &data_ov004_020b9e2c, &data_ov004_020b9ee0,
                                          &data_ov004_020b9e30};
int *sMgMsgFiveCardSpritesByLanguage[] = {&sMgMsgFiveCardSpritesEnglish, &data_ov004_020ba014,
                                          &data_ov004_020ba030, &data_ov004_020b9fd0,
                                          &data_ov004_020b9ea0};
int sMgMsgHateNcgByLanguage[] = {329, 330, 326, 327, 328};
int *sMgMsgDrawSpritesByLanguage[] = {&sMgMsgDrawSpritesEnglish, &data_ov004_020b9e0c,
                                      &data_ov004_020b9fe0, &data_ov004_020b9f6c,
                                      &data_ov004_020b9f44};
int *sMgMsgFullHouseSpritesByLanguage[] = {&sMgMsgFullHouseSpritesEnglish, &data_ov004_020b9ef8,
                                           &data_ov004_020b9fbc, &data_ov004_020ba02c,
                                           &data_ov004_020b9e04};
int *sMgMsgGameOverSpritesByLanguage[] = {&sMgMsgGameOverSpritesEnglish, &data_ov004_020b9eec,
                                          &data_ov004_020b9e94, &data_ov004_020b9fa4,
                                          &data_ov004_020b9ef4};
int sMgMsgTimeOverNcgByLanguage[] = {394, 395, 391, 392, 393};
int *sMgMsgLevelClearSpritesByLanguage[] = {&sMgMsgLevelClearSpritesEnglish, &data_ov004_020b9e54,
                                            &data_ov004_020b9e38, &data_ov004_020b9e18,
                                            &data_ov004_020b9e1c};
int *sMgMsgHiScoreSpritesByLanguage[] = {&sMgMsgHiScoreSpritesEnglish, &data_ov004_020b9fb4,
                                         &data_ov004_020b9f98, &data_ov004_020b9f58,
                                         &data_ov004_020b9f00};
int *sMgMsgPerfectSpritesByLanguage[] = {&sMgMsgPerfectSpritesEnglish, &data_ov004_020b9f8c,
                                         &data_ov004_020b9f38, &data_ov004_020b9ed8,
                                         &data_ov004_020b9f94};
int sMgMsgStartNcgByLanguage[] = {384, 385, 381, 382, 383};
int *sMgMsgResultHiScoreSpritesByLanguage[] = {&sMgMsgResultHiScoreSpritesEnglish,
                                               &data_ov004_020b9f20, &data_ov004_020b9fac,
                                               &data_ov004_020ba024, &data_ov004_020b9f1c};
int *sMgMsgQuitSpritesByLanguage[] = {&sMgMsgQuitSpritesEnglish, &data_ov004_020b9e48,
                                      &data_ov004_020b9f74, &data_ov004_020b9f64,
                                      &data_ov004_020b9e84};
int *sMgMsgRetrySpritesByLanguage[] = {&sMgMsgRetrySpritesEnglish, &data_ov004_020ba00c,
                                       &data_ov004_020b9f34, &data_ov004_020b9f24,
                                       &data_ov004_020b9f3c};
int *sMgMsgStartSpritesByLanguage[] = {&sMgMsgStartSpritesEnglish, &data_ov004_020b9fec,
                                       &data_ov004_020b9f08, &data_ov004_020b9ed0,
                                       &data_ov004_020b9f84};
int *sMgMsgLevelSpritesByLanguage[] = {&sMgMsgLevelSpritesEnglish, &data_ov004_020b9ecc,
                                       &data_ov004_020b9edc, &data_ov004_020b9f04,
                                       &data_ov004_020b9e9c};
int *sMgMsgTooBadSpritesByLanguage[] = {&sMgMsgTooBadSpritesEnglish, &data_ov004_020b9e6c,
                                        &data_ov004_020b9fdc, &data_ov004_020b9e88,
                                        &data_ov004_020b9e68};
int *sMgMsgYouWinSpritesByLanguage[] = {&sMgMsgYouWinSpritesEnglish, &data_ov004_020b9f40,
                                        &data_ov004_020ba000, &data_ov004_020b9f30,
                                        &data_ov004_020b9f18};
int sMgMsgFullHouseNcgByLanguage[] = {313, 314, 310, 311, 312};
int sMgMsgThreeCardNcgByLanguage[] = {283, 284, 280, 281, 282};

void Mg_FreeBannerFiles(void) {
    int i;
    for (i = 0; i < MG_BANNER_FILE_NUM; i++) {
        Deallocate(sMgBannerFiles[i]);
        sMgBannerFiles[i] = 0;
    }
}

void Mg_LoadBannerFiles(void) {
    int i;
    /* The ROM's template for this array is at 0x020bc6e8 and is all zero except
       entry 1. What stood here was 29 invented pairs {1,100}, {2,101} ... {29,128};
       none of them were ever compared to anything, because mwccarm puts a local
       initializer in an anonymous .rodata object (`@7`) that objisolate drops and
       match.py wildcards the relocation to.

       It matters for exactly one entry. Every index except 1 is overwritten below
       before the loop reads it -- entry 1 is not, so its initial value is live, and
       it was wrong. Entry 1 is GIVE UP (handle 0x140, msg_give_up_ncg): the one
       banner with no per-language files. */
    EntryTable entries = sMgBannerFileTemplate;

    entries.e[0].file = sMgMsgRetryNcgByLanguage[GetGameLanguage()];
    entries.e[0].sprites = sMgMsgRetrySpritesByLanguage[GetGameLanguage()];
    entries.e[2].file = sMgMsgQuitNcgByLanguage[GetGameLanguage()];
    entries.e[2].sprites = sMgMsgQuitSpritesByLanguage[GetGameLanguage()];
    entries.e[3].file = sMgMsgLevelClearNcgByLanguage[GetGameLanguage()];
    entries.e[3].sprites = sMgMsgLevelClearSpritesByLanguage[GetGameLanguage()];
    entries.e[4].file = sMgMsgGoalNcgByLanguage[GetGameLanguage()];
    entries.e[4].sprites = sMgMsgGoalSpritesByLanguage[GetGameLanguage()];
    entries.e[5].file = sMgMsgHiScoreNcgByLanguage[GetGameLanguage()];
    entries.e[5].sprites = sMgMsgHiScoreSpritesByLanguage[GetGameLanguage()];
    entries.e[6].file = sMgMsgPerfectNcgByLanguage[GetGameLanguage()];
    entries.e[6].sprites = sMgMsgPerfectSpritesByLanguage[GetGameLanguage()];
    entries.e[7].file = sMgMsgResultHiScoreNcgByLanguage[GetGameLanguage()];
    entries.e[7].sprites = sMgMsgResultHiScoreSpritesByLanguage[GetGameLanguage()];
    entries.e[8].file = sMgMsgGameOverNcgByLanguage[GetGameLanguage()];
    entries.e[8].sprites = sMgMsgGameOverSpritesByLanguage[GetGameLanguage()];
    entries.e[9].file = sMgMsgYouWinNcgByLanguage[GetGameLanguage()];
    entries.e[9].sprites = sMgMsgYouWinSpritesByLanguage[GetGameLanguage()];
    entries.e[10].file = sMgMsgTooBadNcgByLanguage[GetGameLanguage()];
    entries.e[10].sprites = sMgMsgTooBadSpritesByLanguage[GetGameLanguage()];
    entries.e[11].file = sMgMsgDrawNcgByLanguage[GetGameLanguage()];
    entries.e[11].sprites = sMgMsgDrawSpritesByLanguage[GetGameLanguage()];
    entries.e[12].file = sMgMsgStartNcgByLanguage[GetGameLanguage()];
    entries.e[12].sprites = sMgMsgStartSpritesByLanguage[GetGameLanguage()];
    entries.e[13].file = sMgMsgLevelNcgByLanguage[GetGameLanguage()];
    entries.e[13].sprites = sMgMsgLevelSpritesByLanguage[GetGameLanguage()];
    entries.e[14].file = sMgMsgTimeOverNcgByLanguage[GetGameLanguage()];
    entries.e[14].sprites = sMgMsgTimeOverSpritesByLanguage[GetGameLanguage()];
    entries.e[15].file = sMgMsgWhereNcgByLanguage[GetGameLanguage()];
    entries.e[15].sprites = sMgMsgWhereSpritesByLanguage[GetGameLanguage()];
    entries.e[16].file = sMgMsgLikeNcgByLanguage[GetGameLanguage()];
    entries.e[16].sprites = sMgMsgLikeSpritesByLanguage[GetGameLanguage()];
    entries.e[17].file = sMgMsgHateNcgByLanguage[GetGameLanguage()];
    entries.e[17].sprites = sMgMsgHateSpritesByLanguage[GetGameLanguage()];
    entries.e[18].file = sMgMsgLoveNcgByLanguage[GetGameLanguage()];
    entries.e[18].sprites = sMgMsgLoveSpritesByLanguage[GetGameLanguage()];
    entries.e[19].file = sMgMsgDislikeNcgByLanguage[GetGameLanguage()];
    entries.e[19].sprites = sMgMsgDislikeSpritesByLanguage[GetGameLanguage()];
    entries.e[20].file = sMgMsgEsperNcgByLanguage[GetGameLanguage()];
    entries.e[20].sprites = sMgMsgEsperSpritesByLanguage[GetGameLanguage()];
    entries.e[21].file = sMgMsgSuperEsperNcgByLanguage[GetGameLanguage()];
    entries.e[21].sprites = sMgMsgSuperEsperSpritesByLanguage[GetGameLanguage()];
    entries.e[22].file = sMgMsgNoPairNcgByLanguage[GetGameLanguage()];
    entries.e[22].sprites = sMgMsgNoPairSpritesByLanguage[GetGameLanguage()];
    entries.e[23].file = sMgMsgOnePairNcgByLanguage[GetGameLanguage()];
    entries.e[23].sprites = sMgMsgOnePairSpritesByLanguage[GetGameLanguage()];
    entries.e[24].file = sMgMsgTwoPairNcgByLanguage[GetGameLanguage()];
    entries.e[24].sprites = sMgMsgTwoPairSpritesByLanguage[GetGameLanguage()];
    entries.e[25].file = sMgMsgThreeCardNcgByLanguage[GetGameLanguage()];
    entries.e[25].sprites = sMgMsgThreeCardSpritesByLanguage[GetGameLanguage()];
    entries.e[26].file = sMgMsgFullHouseNcgByLanguage[GetGameLanguage()];
    entries.e[26].sprites = sMgMsgFullHouseSpritesByLanguage[GetGameLanguage()];
    entries.e[27].file = sMgMsgFourCardNcgByLanguage[GetGameLanguage()];
    entries.e[27].sprites = sMgMsgFourCardSpritesByLanguage[GetGameLanguage()];
    entries.e[28].file = sMgMsgFiveCardNcgByLanguage[GetGameLanguage()];
    entries.e[28].sprites = sMgMsgFiveCardSpritesByLanguage[GetGameLanguage()];

    for (i = 0; i < MG_BANNER_FILE_NUM; i++) {
        sMgBannerFiles[i] = LoadFile(entries.e[i].file);
        sMgBannerSprites[i] = *entries.e[i].sprites;
    }
}

void Banner_Free(MgBanner *e) {
    e->mFileIndex = MG_BANNER_FILE_NUM;
    sMgBannerGfxSlotUsed[e->mGfxSlot] = 0;
}

void Banner_Draw(MgBanner *e) {
    if (e->mFileIndex == MG_BANNER_FILE_NUM)
        return;
    if (e->mRender != 0) {
        (e->*(e->mRender))();
        return;
    }
    Banner_DrawPlain(e);
}

void Banner_Update(MgBanner *e) {
    if (e->mFileIndex == MG_BANNER_FILE_NUM)
        return;
    if (e->mBehavior == 0) return;
    (e->*(e->mBehavior))();
}

extern "C" void Banner_Init(MgBanner *e, int file, short x, short y, int screen, int priority,
                            short style) {
    /* Where the graphic is decompressed: in display mode 2 into the main
       engine's OBJ VRAM only, otherwise into the sub engine's with a copy in
       the main engine's. */
    u8 *chars, *mirrorChars;

    if (gMgScene->GetDisplayMode() == 2) {
        chars = (u8 *)HW_OBJ_VRAM;
        mirrorChars = 0;
    } else {
        chars = (u8 *)HW_DB_OBJ_VRAM;
        mirrorChars = (u8 *)HW_OBJ_VRAM;
    }

    switch (file) {
    case 3: case 4: case 5: case 6:
    case 8: case 9: case 10: case 11: case 12:
    case 14: case 16: case 17: case 18: case 19: case 20: case 21:
        if (sMgBannerGfxSlotUsed[0] != 0)
            return;
        DecompressLZ16(sMgBannerFiles[file], chars + MG_BANNER_SLOT0_CHARS);
        if (mirrorChars != 0)
            DecompressLZ16(sMgBannerFiles[file], mirrorChars + MG_BANNER_SLOT0_CHARS);
        e->mGfxSlot = 0;
        sMgBannerGfxSlotUsed[0] = 1;
        break;
    default:
        if (sMgBannerGfxSlotUsed[1] == 0) {
            DecompressLZ16(sMgBannerFiles[file], chars + MG_BANNER_SLOT1_CHARS);
            if (mirrorChars != 0)
                DecompressLZ16(sMgBannerFiles[file], mirrorChars + MG_BANNER_SLOT1_CHARS);
            e->mGfxSlot = 1;
            sMgBannerGfxSlotUsed[1] = 1;
            break;
        }
        if (sMgBannerGfxSlotUsed[2] == 0) {
            DecompressLZ16(sMgBannerFiles[file], chars + MG_BANNER_SLOT2_CHARS);
            if (mirrorChars != 0)
                DecompressLZ16(sMgBannerFiles[file], mirrorChars + MG_BANNER_SLOT2_CHARS);
            e->mGfxSlot = 2;
            sMgBannerGfxSlotUsed[2] = 1;
            break;
        }
        return;
    }

    e->mFileIndex = file;
    e->mX = x;
    e->mY = y;
    e->mTargetX = e->mX;
    e->mTargetY = e->mY;
    e->mScreen = screen;
    e->mPriority = priority;
    e->mArrived = 0;

    if (style != 0xd) {
        e->mStyle = style;
    } else {
        switch (file) {
        case 11:
            e->mStyle = 7;
            break;
        case 3: case 4: case 5: case 6: case 20: case 21:
            e->mStyle = 8;
            break;
        case 8: case 14:
            e->mStyle = 9;
            break;
        case 0:
            e->mStyle = 3;
            break;
        case 1: case 2:
            e->mStyle = 4;
            break;
        case 13:
            e->mStyle = 0xc;
            break;
        default:
            e->mStyle = 0;
            break;
        }
    }

    {
        SlotCharNos charNo = sMgBannerGfxSlotCharNo;
        e->mOamCount = Mg_CopySpriteList(sMgBannerSprites[e->mFileIndex], e->mOam,
                                         charNo.v[e->mGfxSlot], e->mFileIndex);
    }

    if (sBannerSetupTable[e->mStyle])
        (e->*sBannerSetupTable[e->mStyle])();

    e->mBehavior = sBannerUpdateTable[e->mStyle];
    e->mRender = sBannerDrawTable[e->mStyle];
}

void Banner_DrawLevelNumber(MgBanner *e) {
    int x;
    int originX = e->mX;
    int n;
    /* the first OAM record's 9-bit signed x, made positive */
    int oamX = ((GXOamAttr *)e->mOam)->x;
    int w;
    unsigned long left;
    n = Mg_GetLevel() + 1;
    if (oamX > 0x100) {
        oamX -= 0x200;
    }
    if (oamX < 0) {
        oamX = -oamX;
    }
    x = e->mX + oamX + 0x10;
    w = 0x10;
    {
        int p = 1;
        while (p < n) {
            p *= 10;
            w += 0x10;
        }
    }
    /* both draws shift left by w/2; w grows 0x10 per power of ten below n */
    x = x - (w >> 1);
    /* The two-step `left` is load-bearing: `left = originX - (w >> 1)`, an int
       `left` and a shared `half` all evaluate it after the OAM pointer, 4 words
       off (measured). */
    left = w >> 1;
    left = originX - left;
    Mg_RenderSpriteOn((OamAttr *)e->mOam, left, e->mY, e->mScreen, e->mPriority);
    Mg_DrawBigNumber(x, e->mY, n, e->mScreen, e->mPriority, 2, 0);
}

void Banner_SetupNone(void) {}

void Banner_DrawSpin(MgBanner *e) {
    GXOamAttr *oam = (GXOamAttr *)e->mOam;
    int angle = (u16)e->mTimer;
    M m;
    u16 flag;
    do {
        int mm = FX_MUL(FX_SinIdx(angle >> 4), 0x800);
        int cos1 = FX_CosIdx((u16)mm >> 4);
        int sin1 = FX_SinIdx((u16)mm >> 4);
        int fc = FX_MUL(cos1, 0x1000);
        int fs = FX_MUL(sin1, 0x1000);
        m._00 = fc;
        m._11 = fc;
        m._01 = fs;
        m._10 = -fs;
        Mg_RenderSpriteOnMtx(oam, e->mX, e->mY, e->mScreen, e->mPriority, &m);
        angle = (u16)(angle + 0x4000);
        flag = oam->attr3;
        oam++;
    } while (flag != 0xffff);
}

void Banner_UpdateSpin(MgBanner *e) {
    if (ApproachLinear(e->mTimer, e->mTimerEnd, 0x1000)) {
        e->mTimer = 0;
    }
}

void Banner_StartSpin(MgBanner *e) {
    e->mTimer = 0;
    e->mTimerEnd = 0x27000;
    e->mBehavior = pmf_Banner_UpdateSpin;
    e->mRender = pmf_Banner_DrawSpin;
}

void Banner_UpdateDrop(MgBanner *e) {
    if (e->mTimer < 48) {
        e->mTimer++;
        return;
    }
    if (ApproachLinear2(e->mY, e->mTargetY, 16)) {
        Banner_StartSpin(e);
    }
}

void Banner_SetupDrop(MgBanner *e) {
    e->mTimer = 0;
    e->mY = -(sMgTopScreenYOffset + 96);
}

void Banner_DrawWave(MgBanner *e) {
    GXOamAttr *oam = (GXOamAttr *)e->mOam;
    int x = e->mTimer >> 2;
    int t = 0x1000;
    u16 flag;
    do {
        int v = 0x1000;
        int d = x - t;
        if (d < 0) d = -d;
        if (d < 0x1000)
            v += FX_MUL((0x1000 - d), 0x400);
        v = FX_Inv(v);
        {
            M m = {0};
            m._00 = v;
            m._11 = v;
            Mg_RenderSpriteOnMtx(oam, e->mX, e->mY, e->mScreen, e->mPriority, &m);
        }
        t += 0x1000;
        flag = oam->attr3;
        oam++;
    } while (flag != 0xffff);
}

void Banner_UpdateWave(MgBanner *e) {
    if (ApproachLinear(e->mTimer, e->mTimerEnd, 0x1000)) {
        e->mTimer = 0;
    }
}

/* A random int in [0, n) from the actors' random state, the top 12 bits of the
   positive draw scaled by n (d_s_mg_pachinko.cpp's PACHINKO_RAND does the same
   from the minigame state). */
#define BANNER_RAND(n) ((int)((u32)(RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 19) * (n) >> 12)

void Banner_StartWaveBurst(MgBanner *e) {
    int i;

    e->mTimer = 0;
    e->mTimerEnd = (e->mOamCount + 1) << 14;

    e->mBehavior = pmf_Banner_UpdateWave;
    e->mRender = pmf_Banner_DrawWave;

    Vector2 burstVel(0, -0x2000);
    Vector2 liftVel(0, -0x1800);
    Vector2 ringOffset(0, -0x4000);

    Vector2 center(e->mX << 12, e->mY << 12);
    Vector2 spawnPos, spawnVel;

    /* The particles' acceleration, which MgFx::UpdateDrift adds to their
       velocity every frame: built once, on the first burst, then registered
       for destruction at exit. */
    static Vector2 sBannerWaveFxGravity(0, 0xc0);

    for (i = 0; i < 32; i++) {
        func_0203d704((int *)&spawnPos, (int *)&center, (int *)&ringOffset);
        func_0203d388((int *)&burstVel, 0x800);
        func_0203d388((int *)&ringOffset, 0x800);
        func_0203d704((int *)&spawnVel, (int *)&liftVel, (int *)&burstVel);

        Mg_SpawnFx(0, BANNER_RAND(60) + 90, &spawnPos, &spawnVel, &sBannerWaveFxGravity);
    }
}

void Banner_DrawSqueeze(MgBanner *e) {
    int m, t, v;
    unsigned short term;
    if (e->mTimer == 0) return;
    m = FX_MUL(e->mTimer, 0xcc);
    {
        GXOamAttr *oam = (GXOamAttr *)e->mOam;
        t = FX_MUL(FX_Div(m, 0x1000), 0x1000);
        v = FX_Inv(m);
        {
            M buf = {0};
            buf._00 = v;
            buf._11 = v;
            t = 0x1000 - t;
            do {
                s16 x = oam->x;
                if (x > 0x100)
                    x -= 0x200;
                x = x * t >> 12;
                Mg_RenderSpriteOnMtx(oam, e->mX - x, e->mY, e->mScreen, e->mPriority, &buf);
                term = oam->attr3;
                oam++;
            } while (term != 0xffff);
        }
    }
}

void Banner_UpdateFall(MgBanner *e) {
    ApproachLinear(e->mTimer, e->mTimerEnd, 0x1000);
    if (ApproachLinear2(e->mY, e->mTargetY, 0x10) == 0)
        return;
    Banner_StartWaveBurst(e);
}

void Banner_SetupFall(MgBanner *e) {
    e->mTimer = 0;
    e->mTimerEnd = 0x14000;
    e->mY = 96;
}

void Banner_DrawPopSpinA(MgBanner *e) {
    u16 flag;
    if (e->mTimer == 0) return;
    {
        GXOamAttr *oam = (GXOamAttr *)e->mOam;
        if (e->mTimer < 0x18000) {
            int v = 0x2000;
            if (e->mTimer >= 0x14000) {
                v = 0x1000;
            } else {
                int t = FX_Div(0x1000, 0x18000);
                v -= FX_MUL(e->mTimer, t);
                v = FX_Inv(v);
            }
            {
                M m = {0};
                m._00 = v;
                m._11 = v;
                do {
                    Mg_RenderSpriteOnMtx(oam, e->mX, e->mY, e->mScreen, e->mPriority, &m);
                    flag = oam->attr3;
                    oam++;
                } while (flag != 0xffff);
            }
        } else {
            int angle = (u16)(e->mTimer - 0x18000);
            M m2;
            do {
                int mm = FX_MUL(FX_SinIdx(angle >> 4), 0x800);
                int cos1 = FX_CosIdx((u16)mm >> 4);
                int sin1 = FX_SinIdx((u16)mm >> 4);
                int fc = FX_MUL(cos1, 0x1000);
                int fs = FX_MUL(sin1, 0x1000);
                m2._00 = fc;
                m2._11 = fc;
                m2._01 = fs;
                m2._10 = -fs;
                Mg_RenderSpriteOnMtx(oam, e->mX, e->mY, e->mScreen, e->mPriority, &m2);
                angle = (u16)(angle + 0x4000);
                flag = oam->attr3;
                oam++;
            } while (flag != 0xffff);
        }
    }
}

void Banner_DrawPopSpinB(MgBanner *e) {
    u16 flag;
    if (e->mTimer == 0) return;
    {
        GXOamAttr *oam = (GXOamAttr *)e->mOam;
        if (e->mTimer < 0x18000) {
            int v = 0x2000;
            if (e->mTimer >= 0x14000) {
                v = 0x1000;
            } else {
                int t = FX_Div(0x1000, 0x18000);
                v -= FX_MUL(e->mTimer, t);
                v = FX_Inv(v);
            }
            {
                M m = {0};
                m._00 = v;
                m._11 = v;
                do {
                    Mg_RenderSpriteOnMtx(oam, e->mX, e->mY, e->mScreen, e->mPriority, &m);
                    flag = oam->attr3;
                    oam++;
                } while (flag != 0xffff);
            }
        } else {
            int angle = (u16)(e->mTimer - 0x18000);
            M m2;
            do {
                int mm = FX_MUL(FX_SinIdx(angle >> 4), 0x800);
                int cos1 = FX_CosIdx((u16)mm >> 4);
                int sin1 = FX_SinIdx((u16)mm >> 4);
                int fc = FX_MUL(cos1, 0x1000);
                int fs = FX_MUL(sin1, 0x1000);
                m2._00 = fc;
                m2._11 = fc;
                m2._01 = fs;
                m2._10 = -fs;
                Mg_RenderSpriteOnMtx(oam, e->mX, e->mY, e->mScreen, e->mPriority, &m2);
                angle = (u16)(angle + 0x4000);
                flag = oam->attr3;
                oam++;
            } while (flag != 0xffff);
        }
    }
}

void Banner_UpdatePopSpin(MgBanner *e) {
    if (ApproachLinear(e->mTimer, e->mTimerEnd, 0x1000)) {
        e->mTimer = 0x18000;
    }
}

void Banner_SetupPopSpin(MgBanner *e) {
    e->mTimer = 0;
    e->mTimerEnd = 0x27000;
}

int Banner_IsPressFinished(MgBanner *e) {
    if (e->mBehavior == pmf_Banner_UpdatePressed_2 && e->mTimer == e->mTimerEnd)
        return 1;
    return 0;
}

void Banner_DrawPressed(MgBanner *e) {
    GXOamAttr *oam = (GXOamAttr *)e->mOam;
    int x = e->mTimer;
    u16 ang = (unsigned int)x >> 1;
    fx16 s = FX_SinIdx(ang >> 4);
    int v = FX_Inv(FX_MUL(s, 0x800) + 0x1000);
    M m = {0};
    u16 flag;
    m._00 = v;
    m._11 = v;
    do {
        Mg_RenderSpriteOnMtx(oam, e->mX, e->mY, e->mScreen, e->mPriority, &m);
        flag = oam->attr3;
        oam++;
    } while (flag != 0xffff);

    x = e->mFileIndex;
    if (x != 0 && x != 1 && x != 2)
        return;
    Mg_RenderSpriteOn(data_ov006_021346bc,
        e->mX, e->mY,
        e->mScreen, e->mPriority);
}

void Banner_UpdatePressed(MgBanner *e) {
    ApproachLinear(e->mTimer, e->mTimerEnd, 0x1000);
}

void Banner_Press(MgBanner *e) {
    sMgBannerButtonsArmed = 0;
    switch (e->mFileIndex) {
    case 0:
        func_02012790(0x62);
        break;
    case 1:
    case 2:
        func_02012790(0x63);
        break;
    }
    e->mTimer = 0;
    e->mTimerEnd = 0x10000;
    e->mBehavior = pmf_Banner_UpdatePressed;
    e->mRender = pmf_Banner_DrawPressed;
}

void Banner_DrawPlain(MgBanner *e) {
    Mg_RenderSpriteOn((OamAttr *)e->mOam, e->mX, e->mY, e->mScreen, e->mPriority);
    if (e->mFileIndex != 0 && e->mFileIndex != 1 && e->mFileIndex != 2)
        return;
    Mg_RenderSpriteOn(data_ov006_021346bc, e->mX, e->mY, e->mScreen, e->mPriority);
}

void Banner_SetupZoom(MgBanner *e) {
    e->mTimer = 0;
    e->mTimerEnd = ((e->mOamCount + 1) << 2) + 0x1e << 0xc;
}

void Banner_DrawZoomA(MgBanner *e) {
    u16 flag;
    int val = e->mTimer;
    if (val == 0) return;
    {
        GXOamAttr *oam = (GXOamAttr *)e->mOam;
        if (val < 0x1e000) {
            int d;
            int scale;
            if (val < 0x14000) {
                int t;
                d = FX_MUL(val, 0x100);
                t = FX_Div(d, 0x1400);
                scale = FX_MUL(t, 0x1400);
            } else {
                int a = FX_MUL((val - 0x14000), 0x66);
                int b = 0x1400 - a;
                int t = FX_Div(b - 0x1000, 0x400);
                scale = (0x1000 - t) + FX_MUL(t, 0x1400);
                d = b;
            }
            d = FX_Inv(d);
            {
                M m = {0};
                m._00 = d;
                m._11 = d;
                scale = 0x1000 - scale;
                do {
                    s16 x = oam->x;
                    if (x > 0x100)
                        x -= 0x200;
                    x = x * scale >> 12;
                    Mg_RenderSpriteOnMtx(oam, e->mX - x, e->mY, e->mScreen, e->mPriority, &m);
                    flag = oam->attr3;
                    oam++;
                } while (flag != 0xffff);
            }
        } else {
            int idx = (val - 0x1e000) >> 2;
            int i = 0x1000;
            do {
                int diff = idx - i;
                int r = 0x1000;
                if (diff < 0) diff = -diff;
                if (diff < 0x1000) {
                    r += FX_MUL((0x1000 - diff), 0x400);
                }
                r = FX_Inv(r);
                {
                    M m2 = {0};
                    m2._00 = r;
                    m2._11 = r;
                    Mg_RenderSpriteOnMtx(oam, e->mX, e->mY, e->mScreen, e->mPriority, &m2);
                }
                i += 0x1000;
                flag = oam->attr3;
                oam++;
            } while (flag != 0xffff);
        }
    }
}

void Banner_DrawZoomB(MgBanner *e) {
    u16 flag;
    int val = e->mTimer;
    if (val == 0) return;
    {
        GXOamAttr *oam = (GXOamAttr *)e->mOam;
        if (val < 0x1e000) {
            int d;
            int scale;
            if (val < 0x14000) {
                int t;
                d = FX_MUL(val, 0x100);
                t = FX_Div(d, 0x1400);
                scale = FX_MUL(t, 0x1400);
            } else {
                int a = FX_MUL((val - 0x14000), 0x66);
                int b = 0x1400 - a;
                int t = FX_Div(b - 0x1000, 0x400);
                scale = (0x1000 - t) + FX_MUL(t, 0x1400);
                d = b;
            }
            d = FX_Inv(d);
            {
                M m = {0};
                m._00 = d;
                m._11 = d;
                scale = 0x1000 - scale;
                do {
                    s16 x = oam->x;
                    if (x > 0x100)
                        x -= 0x200;
                    x = x * scale >> 12;
                    Mg_RenderSpriteOnMtx(oam, e->mX - x, e->mY, e->mScreen, e->mPriority, &m);
                    flag = oam->attr3;
                    oam++;
                } while (flag != 0xffff);
            }
        } else {
            int idx = (val - 0x1e000) >> 2;
            int i = 0x1000;
            do {
                int diff = idx - i;
                int r = 0x1000;
                if (diff < 0) diff = -diff;
                if (diff < 0x1000) {
                    r += FX_MUL((0x1000 - diff), 0x400);
                }
                r = FX_Inv(r);
                {
                    M m2 = {0};
                    m2._00 = r;
                    m2._11 = r;
                    Mg_RenderSpriteOnMtx(oam, e->mX, e->mY, e->mScreen, e->mPriority, &m2);
                }
                i += 0x1000;
                flag = oam->attr3;
                oam++;
            } while (flag != 0xffff);
        }
    }
}

void Banner_UpdateZoom(MgBanner *e) {
    if (ApproachLinear(e->mTimer, e->mTimerEnd, 0x1000)) {
        e->mTimer = 0x1e000;
    }
}

void Banner_SetupZoomBurst(MgBanner *e) {
    int i;

    e->mTimer = 0;
    e->mTimerEnd = (((e->mOamCount + 1) << 2) + 0x1e) << 0xc;

    Vector2 burstVel(0, -0x2000);
    Vector2 liftVel(0, -0x1800);
    Vector2 ringOffset(0, -0x4000);

    Vector2 center(e->mX << 12, e->mY << 12);
    Vector2 spawnPos, spawnVel;

    static Vector2 sBannerZoomFxGravity(0, 0xc0);

    for (i = 0; i < 32; i++) {
        func_0203d704((int *)&spawnPos, (int *)&center, (int *)&ringOffset);
        func_0203d388((int *)&burstVel, 0x800);
        func_0203d388((int *)&ringOffset, 0x800);
        func_0203d704((int *)&spawnVel, (int *)&liftVel, (int *)&burstVel);

        Mg_SpawnFx(0, BANNER_RAND(60) + 90, &spawnPos, &spawnVel, &sBannerZoomFxGravity);
    }
}

void Banner_UpdateSlideFromRight(MgBanner *e) {
    if (ApproachLinear2(e->mX, e->mTargetX, 0x10))
        e->mArrived = 1;
}

void Banner_SetupSlideFromRight(MgBanner *e) {
    e->mX = 320;
}

void Banner_UpdateSlideFromLeft(MgBanner *e) {
    if (ApproachLinear2(e->mX, e->mTargetX, 0x10))
        e->mArrived = 1;
}

void Banner_SetupSlideFromLeft(MgBanner *e) {
    e->mX = -0x40;
}

void Banner_UpdateSlideFromBottom(MgBanner *e) {
    ApproachLinear2(e->mY, e->mTargetY, 4);
}

void Banner_SetupSlideFromBottom(MgBanner *e) {
    e->mY = 200;
}

void Banner_UpdateSlideFromTop(MgBanner *e) {
    ApproachLinear2(e->mY, e->mTargetY, 4);
}

void Banner_SetupSlideFromTop(MgBanner *e) {
    e->mY = -8;
}

MgBanner::MgBanner() {
    mFileIndex = MG_BANNER_FILE_NUM;
}

/* __sinit_ov004_020b955c sources: the member-pointer records the three
   per-style tables copy (setup sBannerSetupTable, behavior
   sBannerUpdateTable, render sBannerDrawTable), and the words the
   29-entry file-id table copies. */
extern "C" {
extern ElemFn data_02086b58; /* __ptmf_null's address: the null member pointer */
extern ElemFn pmf_Banner_SetupSlideFromTop, pmf_Banner_SetupSlideFromBottom,
    pmf_Banner_SetupSlideFromLeft, pmf_Banner_SetupSlideFromRight;
extern ElemFn pmf_Banner_SetupZoomBurst, pmf_Banner_SetupPopSpin, pmf_Banner_SetupZoom,
    pmf_Banner_SetupZoomBurst_2;
extern ElemFn pmf_Banner_SetupPopSpin_2, pmf_Banner_SetupFall, pmf_Banner_SetupDrop,
    pmf_Banner_SetupNone;
extern ElemFn pmf_Banner_UpdateSlideFromTop, pmf_Banner_UpdateSlideFromBottom,
    pmf_Banner_UpdateSlideFromLeft, pmf_Banner_UpdateSlideFromRight;
extern ElemFn pmf_Banner_UpdateZoom_2, pmf_Banner_UpdatePopSpin_2, pmf_Banner_UpdateZoom_3,
    pmf_Banner_UpdateZoom;
extern ElemFn pmf_Banner_UpdatePopSpin, pmf_Banner_UpdateFall, pmf_Banner_UpdateDrop;
extern ElemFn pmf_Banner_DrawZoomB, pmf_Banner_DrawPopSpinB, pmf_Banner_DrawZoomB_2,
    pmf_Banner_DrawZoomA;
extern ElemFn pmf_Banner_DrawPopSpinA, pmf_Banner_DrawSqueeze, pmf_Banner_DrawLevelNumber;
extern int sMgMsgRetrySpritesEnglish, sMgMsgQuitSpritesEnglish, sMgMsgLevelClearSpritesEnglish;
extern int sMgMsgGoalSpritesEnglish, sMgMsgHiScoreSpritesEnglish, sMgMsgPerfectSpritesEnglish,
    sMgMsgResultHiScoreSpritesEnglish;
extern int sMgMsgGameOverSpritesEnglish, sMgMsgYouWinSpritesEnglish, sMgMsgTooBadSpritesEnglish,
    sMgMsgDrawSpritesEnglish;
extern int sMgMsgStartSpritesEnglish, sMgMsgLevelSpritesEnglish, sMgMsgTimeOverSpritesEnglish,
    sMgMsgWhereSpritesEnglish;
extern int sMgMsgLikeSpritesEnglish, sMgMsgHateSpritesEnglish, sMgMsgLoveSpritesEnglish,
    sMgMsgDislikeSpritesEnglish;
extern int sMgMsgEsperSpritesEnglish, sMgMsgSuperEsperSpritesEnglish, sMgMsgNoPairSpritesEnglish,
    sMgMsgOnePairSpritesEnglish;
extern int sMgMsgTwoPairSpritesEnglish, sMgMsgThreeCardSpritesEnglish,
    sMgMsgFullHouseSpritesEnglish, sMgMsgFourCardSpritesEnglish;
extern int sMgMsgFiveCardSpritesEnglish;
}

namespace s20b955c {
extern "C" {
MgBanner gMgBanners[3];
}
}

extern "C" {
int sMgBannerSprites[MG_BANNER_FILE_NUM] = {
    sMgMsgRetrySpritesEnglish,     sMgMsgGiveUpSprites,
    sMgMsgQuitSpritesEnglish,      sMgMsgLevelClearSpritesEnglish,
    sMgMsgGoalSpritesEnglish,      sMgMsgHiScoreSpritesEnglish,
    sMgMsgPerfectSpritesEnglish,   sMgMsgResultHiScoreSpritesEnglish,
    sMgMsgGameOverSpritesEnglish,  sMgMsgYouWinSpritesEnglish,
    sMgMsgTooBadSpritesEnglish,    sMgMsgDrawSpritesEnglish,
    sMgMsgStartSpritesEnglish,     sMgMsgLevelSpritesEnglish,
    sMgMsgTimeOverSpritesEnglish,  sMgMsgWhereSpritesEnglish,
    sMgMsgLikeSpritesEnglish,      sMgMsgHateSpritesEnglish,
    sMgMsgLoveSpritesEnglish,      sMgMsgDislikeSpritesEnglish,
    sMgMsgEsperSpritesEnglish,     sMgMsgSuperEsperSpritesEnglish,
    sMgMsgNoPairSpritesEnglish,    sMgMsgOnePairSpritesEnglish,
    sMgMsgTwoPairSpritesEnglish,   sMgMsgThreeCardSpritesEnglish,
    sMgMsgFullHouseSpritesEnglish, sMgMsgFourCardSpritesEnglish,
    sMgMsgFiveCardSpritesEnglish};
ElemFn sBannerSetupTable[13] = {data_02086b58,
                                pmf_Banner_SetupSlideFromTop,
                                pmf_Banner_SetupSlideFromBottom,
                                pmf_Banner_SetupSlideFromLeft,
                                pmf_Banner_SetupSlideFromRight,
                                pmf_Banner_SetupZoomBurst,
                                pmf_Banner_SetupPopSpin,
                                pmf_Banner_SetupZoom,
                                pmf_Banner_SetupZoomBurst_2,
                                pmf_Banner_SetupPopSpin_2,
                                pmf_Banner_SetupFall,
                                pmf_Banner_SetupDrop,
                                pmf_Banner_SetupNone};
ElemFn sBannerUpdateTable[13] = {data_02086b58,
                                 pmf_Banner_UpdateSlideFromTop,
                                 pmf_Banner_UpdateSlideFromBottom,
                                 pmf_Banner_UpdateSlideFromLeft,
                                 pmf_Banner_UpdateSlideFromRight,
                                 pmf_Banner_UpdateZoom_2,
                                 pmf_Banner_UpdatePopSpin_2,
                                 pmf_Banner_UpdateZoom_3,
                                 pmf_Banner_UpdateZoom,
                                 pmf_Banner_UpdatePopSpin,
                                 pmf_Banner_UpdateFall,
                                 pmf_Banner_UpdateDrop,
                                 data_02086b58};
ElemFn sBannerDrawTable[13] = {data_02086b58,
                               data_02086b58,
                               data_02086b58,
                               data_02086b58,
                               data_02086b58,
                               pmf_Banner_DrawZoomB,
                               pmf_Banner_DrawPopSpinB,
                               pmf_Banner_DrawZoomB_2,
                               pmf_Banner_DrawZoomA,
                               pmf_Banner_DrawPopSpinA,
                               pmf_Banner_DrawSqueeze,
                               data_02086b58,
                               pmf_Banner_DrawLevelNumber};
void *sMgBannerFiles[MG_BANNER_FILE_NUM];
}
