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
 * Data. Besides the anonymous initializer template of Mg_LoadBannerFiles's local
 * array at 0x020bc6e8 (0xe8 bytes) and the {0x380, 0x300, 0x340} slot table at
 * 0x020bc27c, the unit owns the banner graphics: 141 OAM sprite lists at
 * 0x020ba034..0x020bbf4c and the 141 pointer cells naming them at
 * 0x020b9e00..0x020ba034, among which the 29 English cells sit. Everything else
 * it touches is reached by extern.
 *
 * Emission order. `#pragma defer_codegen off` is load-bearing here: with codegen
 * deferred mwccarm emits the functions' sections in reverse source order
 * (measured: Mg_FreeBannerFiles comes out last), and the ROM has them
 * ascending. Source is therefore ROM-ascending.
 *
 * The bodies were matched as one-function C or C++ sources. The entry points
 * dScMgBase_c's unit calls (Banner_Init, Banner_Update, Banner_Draw,
 * Banner_Free, Banner_Press, Banner_IsPressFinished) stay free functions; the
 * handlers the per-style tables hold are MgBanner methods.
 *
 * Numbered names. mwccarm numbers its generated names from one counter that
 * every named parameter (prototypes included), local and temporary before a
 * definition advances. The array gMgBanners at the end of the file
 * gets an __arraydtor$N thunk and an @N registration node (the manifest's
 * defined_symbol_renames hold the current N); symbols.txt names the thunk
 * __arraydtor$954, and the manifest renames both onto their configured
 * names, so an edit here that moves the count re-keys those two rows. The
 * two function-local Vector2 statics (MgBanner::StartWaveBurst,
 * MgBanner::SetupZoomBurst) are renamed the same way, onto the ov004 .bss names
 * at 0x020bf3ec..0x020bf41c that this unit owns.
 *
 * Member pointers. The setup, behavior and render handlers are MgBanner
 * methods, and the handlers and the three per-style tables name them as
 * &MgBanner::handler. mwccarm emits each such use as its own {function, 0}
 * .data constant, which is the run of 37 pmf_Banner_* records at
 * 0x020bc154..0x020bc27c just below this unit's .data; data_02086b58 is
 * __ptmf_null, the null member pointer the tables copy.
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
#include "OamAttr.h"
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
#include "Matrix2x2.h"
#include "Vector2Math.h"

struct SlotCharNos {
    int v[3];
};
/* The banner files: 29 graphics, loaded together. An MgBanner whose mFileIndex
   is MG_BANNER_FILE_NUM shows nothing. */
#define MG_BANNER_FILE_NUM 29

struct Entry {
    int file;
    OamAttr **sprites;
};
struct EntryTable {
    Entry e[MG_BANNER_FILE_NUM];
};

extern "C" {
void Deallocate(void *ptr);
int GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
int Mg_CopySpriteList(int a, void *b, int c, int d);
void func_0203d388(int *p, s16 angle);
void Mg_SpawnFx(int state, short timer, Vector2 *pos, Vector2 *vel, Vector2 *accel);
void Mg_RenderSpriteOnMtx(void *a0, int a1, int a2, int a3, int a4, Matrix2x2 *a5);
void Mg_RenderSpriteOn(OamAttr *a, int b, int c, int d, int e);
int Mg_GetLevel(void);
void Mg_DrawBigNumber(int a, int b, int c, int d, int e, int f, int g);
}

/* The first OBJ character number of each graphics slot, {0x380, 0x300, 0x340}
   (MG_BANNER_SLOTn_CHARS / 32). Banner_Init copies it whole to the stack and
   indexes the copy: the code of a local `int charNo[3] = {...}`, whose
   initializer template the ROM keeps here in .data. Written that way, mwccarm
   emits the same Banner_Init but puts the template in an anonymous .rodata
   object outside this unit's claimed .data (measured), so the copy stays. */
SlotCharNos sMgBannerGfxSlotCharNo = {{0x380, 0x300, 0x340}};
extern "C" {
/* The per-message banner graphics: one OAM sprite list for each message and
   language. The pointer cells sit together above the lists; the English
   cells are named after their message, the rest keep their ROM addresses, as
   do the lists themselves. Each list ends at the record whose attr3 is
   0xffff, which is the terminator Mg_CopySpriteList stops on. */
OamAttr data_ov004_020ba034[4] = {{0x00f8, 0x41e3, 0xc000, 0x0000},
                                  {0x00f8, 0x41f1, 0xc002, 0x0000},
                                  {0x00f8, 0x41ff, 0xc004, 0x0000},
                                  {0x00f8, 0x400d, 0xc006, 0xffff}};
OamAttr data_ov004_020ba054[4] = {{0x00f8, 0x41e5, 0xc000, 0x0000},
                                  {0x00f8, 0x41f3, 0xc002, 0x0000},
                                  {0x00f8, 0x4001, 0xc004, 0x0000},
                                  {0x00f8, 0x400e, 0xc006, 0xffff}};
OamAttr data_ov004_020ba074[4] = {{0x00f8, 0x41e3, 0xc000, 0x0000},
                                  {0x00f8, 0x41f1, 0xc002, 0x0000},
                                  {0x00f8, 0x41ff, 0xc004, 0x0000},
                                  {0x00f8, 0x400d, 0xc006, 0xffff}};
OamAttr data_ov004_020ba094[4] = {{0x00f8, 0x41e3, 0xc000, 0x0000},
                                  {0x00f8, 0x41f1, 0xc002, 0x0000},
                                  {0x00f8, 0x41ff, 0xc004, 0x0000},
                                  {0x00f8, 0x400d, 0xc006, 0xffff}};
OamAttr data_ov004_020ba0b4[4] = {{0x40f0, 0xc180, 0x9000, 0x0000},
                                  {0x40f0, 0xc1c0, 0x9008, 0x0000},
                                  {0x40f0, 0xc000, 0x9010, 0x0000},
                                  {0x40f0, 0xc040, 0x9018, 0xffff}};
OamAttr data_ov004_020ba0d4[4] = {{0x00f0, 0x81ca, 0x8000, 0x0000},
                                  {0x00f0, 0x81e2, 0x9004, 0x0000},
                                  {0x00f0, 0x81fb, 0xa008, 0x0000},
                                  {0x00f0, 0x8017, 0xb00c, 0xffff}};
OamAttr data_ov004_020ba0f4[4] = {{0x00f0, 0x81c7, 0x8000, 0x0000},
                                  {0x00f0, 0x81e4, 0x9004, 0x0000},
                                  {0x00f0, 0x8001, 0xa008, 0x0000},
                                  {0x00f0, 0x8019, 0xb00c, 0xffff}};
OamAttr data_ov004_020ba114[4] = {{0x40f0, 0xc180, 0x9000, 0x0000},
                                  {0x40f0, 0xc1c0, 0x9008, 0x0000},
                                  {0x40f0, 0xc000, 0x9010, 0x0000},
                                  {0x40f0, 0xc040, 0x9018, 0xffff}};
OamAttr data_ov004_020ba134[4] = {{0x00f0, 0x81c3, 0x8000, 0x0000},
                                  {0x00f0, 0x81e1, 0x9004, 0x0000},
                                  {0x00f0, 0x81ff, 0xa008, 0x0000},
                                  {0x00f0, 0x801d, 0xb00c, 0xffff}};
OamAttr data_ov004_020ba154[4] = {{0x40f0, 0xc180, 0x9000, 0x0000},
                                  {0x40f0, 0xc1c0, 0x9008, 0x0000},
                                  {0x40f0, 0xc000, 0x9010, 0x0000},
                                  {0x40f0, 0xc040, 0x9018, 0xffff}};
OamAttr data_ov004_020ba174[4] = {{0x40f0, 0xc180, 0x9000, 0x0000},
                                  {0x40f0, 0xc1c0, 0x9008, 0x0000},
                                  {0x40f0, 0xc000, 0x9010, 0x0000},
                                  {0x40f0, 0xc040, 0x9018, 0xffff}};
OamAttr data_ov004_020ba194[4] = {{0x40f0, 0xc180, 0x9000, 0x0000},
                                  {0x40f0, 0xc1c0, 0x9008, 0x0000},
                                  {0x40f0, 0xc000, 0x9010, 0x0000},
                                  {0x40f0, 0xc040, 0x9018, 0xffff}};
OamAttr data_ov004_020ba1b4[4] = {{0x40f0, 0xc180, 0x9000, 0x0000},
                                  {0x40f0, 0xc1c0, 0x9008, 0x0000},
                                  {0x40f0, 0xc000, 0x9010, 0x0000},
                                  {0x40f0, 0xc040, 0x9018, 0xffff}};
OamAttr data_ov004_020ba1d4[4] = {{0x40f0, 0xc180, 0xa000, 0x0000},
                                  {0x40f0, 0xc1c0, 0xa008, 0x0000},
                                  {0x40f0, 0xc000, 0xa010, 0x0000},
                                  {0x40f0, 0xc040, 0xa018, 0xffff}};
OamAttr data_ov004_020ba1f4[4] = {{0x00f8, 0x41e3, 0xc000, 0x0000},
                                  {0x00f8, 0x41f1, 0xc002, 0x0000},
                                  {0x00f8, 0x41ff, 0xc004, 0x0000},
                                  {0x00f8, 0x400d, 0xc006, 0xffff}};
OamAttr data_ov004_020ba214[4] = {{0x00f8, 0x41e3, 0xc000, 0x0000},
                                  {0x00f8, 0x41f2, 0xc002, 0x0000},
                                  {0x00f8, 0x41fe, 0xc004, 0x0000},
                                  {0x00f8, 0x4009, 0xc006, 0xffff}};
OamAttr data_ov004_020ba234[4] = {{0x00f8, 0x41e7, 0xc000, 0x0000},
                                  {0x00f8, 0x41f4, 0xc002, 0x0000},
                                  {0x00f8, 0x4003, 0xc004, 0x0000},
                                  {0x00f8, 0x4011, 0xc006, 0xffff}};
OamAttr data_ov004_020ba254[4] = {{0x00f0, 0x81c8, 0x8000, 0x0000},
                                  {0x00f0, 0x81e0, 0x9004, 0x0000},
                                  {0x00f0, 0x81f7, 0xa008, 0x0000},
                                  {0x00f0, 0x8016, 0xb00c, 0xffff}};
OamAttr data_ov004_020ba274[4] = {{0x40f0, 0xc180, 0xa000, 0x0000},
                                  {0x40f0, 0xc1c0, 0xa008, 0x0000},
                                  {0x40f0, 0xc000, 0xa010, 0x0000},
                                  {0x40f0, 0xc040, 0xa018, 0xffff}};
OamAttr data_ov004_020ba294[4] = {{0x40f0, 0xc180, 0x9000, 0x0000},
                                  {0x40f0, 0xc1c0, 0x9008, 0x0000},
                                  {0x40f0, 0xc000, 0x9010, 0x0000},
                                  {0x40f0, 0xc040, 0x9018, 0xffff}};
OamAttr data_ov004_020ba2b4[4] = {{0x40f0, 0xc180, 0xa000, 0x0000},
                                  {0x40f0, 0xc1c0, 0xa008, 0x0000},
                                  {0x40f0, 0xc000, 0xa010, 0x0000},
                                  {0x40f0, 0xc040, 0xa018, 0xffff}};
OamAttr data_ov004_020ba2d4[4] = {{0x00f8, 0x41e6, 0xc000, 0x0000},
                                  {0x00f8, 0x41f4, 0xc002, 0x0000},
                                  {0x00f8, 0x4002, 0xc004, 0x0000},
                                  {0x00f8, 0x400b, 0xc006, 0xffff}};
OamAttr data_ov004_020ba2f4[4] = {{0x40f0, 0xc180, 0xa000, 0x0000},
                                  {0x40f0, 0xc1c0, 0xa008, 0x0000},
                                  {0x40f0, 0xc000, 0xa010, 0x0000},
                                  {0x40f0, 0xc040, 0xa018, 0xffff}};
OamAttr data_ov004_020ba314[4] = {{0x40f0, 0xc180, 0xa000, 0x0000},
                                  {0x40f0, 0xc1c0, 0xa008, 0x0000},
                                  {0x40f0, 0xc000, 0xa010, 0x0000},
                                  {0x40f0, 0xc040, 0xa018, 0xffff}};
OamAttr data_ov004_020ba334[4] = {{0x40f0, 0xc180, 0xa000, 0x0000},
                                  {0x40f0, 0xc1c0, 0xa008, 0x0000},
                                  {0x40f0, 0xc000, 0xa010, 0x0000},
                                  {0x40f0, 0xc040, 0xa018, 0xffff}};
OamAttr data_ov004_020ba354[4] = {{0x00f0, 0x81c3, 0xc000, 0x0000},
                                  {0x00f0, 0x81e2, 0xc004, 0x0000},
                                  {0x00f0, 0x8001, 0xc008, 0x0000},
                                  {0x00f0, 0x801c, 0xc00c, 0xffff}};
OamAttr data_ov004_020ba374[4] = {{0x40f0, 0xc180, 0xa000, 0x0000},
                                  {0x40f0, 0xc1c0, 0xa008, 0x0000},
                                  {0x40f0, 0xc000, 0xa010, 0x0000},
                                  {0x40f0, 0xc040, 0xa018, 0xffff}};
OamAttr data_ov004_020ba394[4] = {{0x40f0, 0xc180, 0x9000, 0x0000},
                                  {0x40f0, 0xc1c0, 0x9008, 0x0000},
                                  {0x40f0, 0xc000, 0x9010, 0x0000},
                                  {0x40f0, 0xc040, 0x9018, 0xffff}};
OamAttr data_ov004_020ba3b4[4] = {{0x40f0, 0xc180, 0xa000, 0x0000},
                                  {0x40f0, 0xc1c0, 0xa008, 0x0000},
                                  {0x40f0, 0xc000, 0xa010, 0x0000},
                                  {0x40f0, 0xc040, 0xa018, 0xffff}};
OamAttr data_ov004_020ba3d4[4] = {{0x40f0, 0xc180, 0x9000, 0x0000},
                                  {0x40f0, 0xc1c0, 0x9008, 0x0000},
                                  {0x40f0, 0xc000, 0x9010, 0x0000},
                                  {0x40f0, 0xc040, 0x9018, 0xffff}};
OamAttr data_ov004_020ba3f4[4] = {{0x00f8, 0x41e7, 0xc000, 0x0000},
                                  {0x00f8, 0x41f5, 0xc002, 0x0000},
                                  {0x00f8, 0x41fe, 0xc004, 0x0000},
                                  {0x00f8, 0x400b, 0xc006, 0xffff}};
OamAttr data_ov004_020ba414[5] = {{0x00f0, 0x81ba, 0xa000, 0x0000},
                                  {0x00f0, 0x81d7, 0xa004, 0x0000},
                                  {0x00f0, 0x81ef, 0xa008, 0x0000},
                                  {0x00f0, 0x800b, 0xa00c, 0x0000},
                                  {0x00f0, 0x8027, 0xa004, 0xffff}};
OamAttr data_ov004_020ba43c[5] = {{0x00f8, 0x41d8, 0xc000, 0x0000},
                                  {0x00f8, 0x41f0, 0xc002, 0x0000},
                                  {0x00f8, 0x41fd, 0xc004, 0x0000},
                                  {0x00f8, 0x400a, 0xc006, 0x0000},
                                  {0x00f8, 0x4017, 0xc008, 0xffff}};
OamAttr data_ov004_020ba464[5] = {{0x00f8, 0x41dc, 0xc000, 0x0000},
                                  {0x00f8, 0x41e9, 0xc002, 0x0000},
                                  {0x00f8, 0x41f7, 0xc004, 0x0000},
                                  {0x00f8, 0x4006, 0xc006, 0x0000},
                                  {0x00f8, 0x4014, 0xc008, 0xffff}};
OamAttr data_ov004_020ba48c[5] = {{0x00f0, 0x81b5, 0x9000, 0x0000},
                                  {0x00f0, 0x81d2, 0x9004, 0x0000},
                                  {0x00f0, 0x81ef, 0x9008, 0x0000},
                                  {0x00f0, 0x800b, 0x900c, 0x0000},
                                  {0x00f0, 0x8029, 0x9010, 0xffff}};
OamAttr data_ov004_020ba4b4[5] = {{0x00f0, 0x81b5, 0x8000, 0x0000},
                                  {0x00f0, 0x81d3, 0x9004, 0x0000},
                                  {0x00f0, 0x81ef, 0xa008, 0x0000},
                                  {0x00f0, 0x800e, 0xb00c, 0x0000},
                                  {0x00f0, 0x802b, 0xc010, 0xffff}};
OamAttr data_ov004_020ba4dc[5] = {{0x00f8, 0x41db, 0x8000, 0x0000},
                                  {0x00f8, 0x41e9, 0x8002, 0x0000},
                                  {0x00f8, 0x41f7, 0x8004, 0x0000},
                                  {0x00f8, 0x4006, 0x8006, 0x0000},
                                  {0x00f8, 0x4013, 0x8008, 0xffff}};
OamAttr data_ov004_020ba504[5] = {{0x00f8, 0x41db, 0x8000, 0x0000},
                                  {0x00f8, 0x41e9, 0x8002, 0x0000},
                                  {0x00f8, 0x41f7, 0x8004, 0x0000},
                                  {0x00f8, 0x4006, 0x8006, 0x0000},
                                  {0x00f8, 0x4013, 0x8008, 0xffff}};
OamAttr data_ov004_020ba52c[5] = {{0x00f8, 0x41df, 0x8000, 0x0000},
                                  {0x00f8, 0x41ed, 0x8002, 0x0000},
                                  {0x00f8, 0x41f5, 0x8004, 0x0000},
                                  {0x00f8, 0x4004, 0x8006, 0x0000},
                                  {0x00f8, 0x4013, 0x8008, 0xffff}};
OamAttr data_ov004_020ba554[5] = {{0x00f8, 0x41db, 0xc000, 0x0000},
                                  {0x00f8, 0x41ee, 0xc002, 0x0000},
                                  {0x00f8, 0x41fb, 0xc004, 0x0000},
                                  {0x00f8, 0x4009, 0xc006, 0x0000},
                                  {0x00f8, 0x4017, 0xc008, 0xffff}};
OamAttr data_ov004_020ba57c[5] = {{0x00f8, 0x41df, 0xc000, 0x0000},
                                  {0x00f8, 0x41ee, 0xc002, 0x0000},
                                  {0x00f8, 0x41fc, 0xc004, 0x0000},
                                  {0x00f8, 0x400a, 0xc006, 0x0000},
                                  {0x00f8, 0x4013, 0xc008, 0xffff}};
OamAttr data_ov004_020ba5a4[5] = {{0x00f8, 0x41de, 0xc000, 0x0000},
                                  {0x00f8, 0x41ec, 0xc002, 0x0000},
                                  {0x00f8, 0x41fa, 0xc004, 0x0000},
                                  {0x00f8, 0x4008, 0xc006, 0x0000},
                                  {0x00f8, 0x4016, 0xc008, 0xffff}};
OamAttr data_ov004_020ba5cc[5] = {{0x00f0, 0x81be, 0xa000, 0x0000},
                                  {0x00f0, 0x81d3, 0xa004, 0x0000},
                                  {0x00f0, 0x81e6, 0xa008, 0x0000},
                                  {0x00f0, 0x8004, 0xa00c, 0x0000},
                                  {0x00f0, 0x8023, 0xa010, 0xffff}};
OamAttr data_ov004_020ba5f4[5] = {{0x00f8, 0x41dc, 0xc000, 0x0000},
                                  {0x00f8, 0x41e9, 0xc002, 0x0000},
                                  {0x00f8, 0x41f7, 0xc004, 0x0000},
                                  {0x00f8, 0x4006, 0xc006, 0x0000},
                                  {0x00f8, 0x4014, 0xc008, 0xffff}};
OamAttr data_ov004_020ba61c[5] = {{0x00f8, 0x41dc, 0xc000, 0x0000},
                                  {0x00f8, 0x41e9, 0xc002, 0x0000},
                                  {0x00f8, 0x41f7, 0xc004, 0x0000},
                                  {0x00f8, 0x4006, 0xc006, 0x0000},
                                  {0x00f8, 0x4014, 0xc008, 0xffff}};
OamAttr data_ov004_020ba644[6] = {
    {0x00f8, 0x41d7, 0xa000, 0x0000}, {0x00f8, 0x41e5, 0xa002, 0x0000},
    {0x00f8, 0x41f2, 0xa004, 0x0000}, {0x00f8, 0x4000, 0xa006, 0x0000},
    {0x00f8, 0x400f, 0xa008, 0x0000}, {0x00f8, 0x401d, 0xa00a, 0xffff}};
OamAttr data_ov004_020ba674[6] = {
    {0x00f8, 0x41d8, 0xc000, 0x0000}, {0x00f8, 0x41e6, 0xc002, 0x0000},
    {0x00f8, 0x41ef, 0xc004, 0x0000}, {0x00f8, 0x41fd, 0xc006, 0x0000},
    {0x00f8, 0x400b, 0xc008, 0x0000}, {0x00f8, 0x4019, 0xc00a, 0xffff}};
OamAttr data_ov004_020ba6a4[6] = {
    {0x00f0, 0x81b2, 0xa000, 0x0000}, {0x00f0, 0x81ca, 0xa004, 0x0000},
    {0x00f0, 0x81e3, 0xa008, 0x0000}, {0x00f0, 0x81fc, 0xa00c, 0x0000},
    {0x00f0, 0x8014, 0xa010, 0x0000}, {0x00f0, 0x8028, 0xa014, 0xffff}};
OamAttr data_ov004_020ba6d4[6] = {
    {0x00f0, 0x81a9, 0xa000, 0x0000}, {0x00f0, 0x81c7, 0xa004, 0x0000},
    {0x00f0, 0x81e4, 0xa008, 0x0000}, {0x00f0, 0x8002, 0xa00c, 0x0000},
    {0x00f0, 0x8021, 0xa010, 0x0000}, {0x00f0, 0x803d, 0xa014, 0xffff}};
OamAttr data_ov004_020ba704[6] = {
    {0x00f0, 0x81a7, 0x9000, 0x0000}, {0x00f0, 0x81c5, 0x9004, 0x0000},
    {0x00f0, 0x81e2, 0x9008, 0x0000}, {0x00f0, 0x8000, 0x900c, 0x0000},
    {0x00f0, 0x801c, 0x9010, 0x0000}, {0x00f0, 0x803b, 0x9014, 0xffff}};
OamAttr data_ov004_020ba734[6] = {
    {0x00f0, 0x81a2, 0xa000, 0x0000}, {0x00f0, 0x81bc, 0xa004, 0x0000},
    {0x00f0, 0x81da, 0xa008, 0x0000}, {0x00f0, 0x8008, 0xa00c, 0x0000},
    {0x00f0, 0x8023, 0xa010, 0x0000}, {0x00f0, 0x803e, 0xa014, 0xffff}};
OamAttr data_ov004_020ba764[6] = {
    {0x00f8, 0x41d2, 0xc000, 0x0000}, {0x00f8, 0x41ea, 0xc002, 0x0000},
    {0x00f8, 0x41f6, 0xc004, 0x0000}, {0x00f8, 0x4002, 0xc006, 0x0000},
    {0x00f8, 0x400d, 0xc008, 0x0000}, {0x00f8, 0x401c, 0xc00a, 0xffff}};
OamAttr data_ov004_020ba794[6] = {
    {0x00f0, 0x81ac, 0x8000, 0x0000}, {0x00f0, 0x81ca, 0x9004, 0x0000},
    {0x00f0, 0x81e8, 0xa008, 0x0000}, {0x00f0, 0x8000, 0xb00c, 0x0000},
    {0x00f0, 0x8019, 0xc010, 0x0000}, {0x00f0, 0x8033, 0x8014, 0xffff}};
OamAttr data_ov004_020ba7c4[6] = {
    {0x00f8, 0x41d1, 0xa000, 0x0000}, {0x00f8, 0x41de, 0xa002, 0x0000},
    {0x00f8, 0x41ea, 0xa004, 0x0000}, {0x00f8, 0x41f9, 0xa006, 0x0000},
    {0x00f8, 0x400e, 0xa008, 0x0000}, {0x00f8, 0x401d, 0xa00a, 0xffff}};
OamAttr data_ov004_020ba7f4[6] = {
    {0x00f8, 0x41d9, 0x8000, 0x0000}, {0x00f8, 0x41e7, 0x8002, 0x0000},
    {0x00f8, 0x41ef, 0x8004, 0x0000}, {0x00f8, 0x41fe, 0x8006, 0x0000},
    {0x00f8, 0x400d, 0x8008, 0x0000}, {0x00f8, 0x401b, 0x800a, 0xffff}};
OamAttr data_ov004_020ba824[6] = {
    {0x00f0, 0x81a1, 0x9000, 0x0000}, {0x00f0, 0x81ba, 0x9004, 0x0000},
    {0x00f0, 0x81d7, 0x9004, 0x0000}, {0x00f0, 0x8007, 0x9008, 0x0000},
    {0x00f0, 0x8022, 0x900c, 0x0000}, {0x00f0, 0x803d, 0x9010, 0xffff}};
OamAttr data_ov004_020ba854[6] = {
    {0x00f8, 0x41d6, 0xc000, 0x0000}, {0x00f8, 0x41ec, 0xc002, 0x0000},
    {0x00f8, 0x41f9, 0xc004, 0x0000}, {0x00f8, 0x4007, 0xc006, 0x0000},
    {0x00f8, 0x4010, 0xc008, 0x0000}, {0x00f8, 0x401e, 0xc00a, 0xffff}};
OamAttr data_ov004_020ba884[6] = {
    {0x00f8, 0x41d8, 0xc000, 0x0000}, {0x00f8, 0x41e6, 0xc002, 0x0000},
    {0x00f8, 0x41f5, 0xc004, 0x0000}, {0x00f8, 0x4004, 0xc006, 0x0000},
    {0x00f8, 0x4013, 0xc008, 0x0000}, {0x00f8, 0x401b, 0xc00a, 0xffff}};
OamAttr data_ov004_020ba8b4[6] = {
    {0x00f8, 0x41d7, 0xc000, 0x0000}, {0x00f8, 0x41e4, 0xc002, 0x0000},
    {0x00f8, 0x41f2, 0xc004, 0x0000}, {0x00f8, 0x4000, 0xc006, 0x0000},
    {0x00f8, 0x400d, 0xc008, 0x0000}, {0x00f8, 0x401c, 0xc00a, 0xffff}};
OamAttr data_ov004_020ba8e4[6] = {
    {0x00f0, 0x81a6, 0x8000, 0x0000}, {0x00f0, 0x81c6, 0x9004, 0x0000},
    {0x00f0, 0x81e2, 0xa008, 0x0000}, {0x00f0, 0x81ff, 0xb00c, 0x0000},
    {0x00f0, 0x801d, 0xc010, 0x0000}, {0x00f0, 0x8036, 0x8014, 0xffff}};
OamAttr data_ov004_020ba914[6] = {
    {0x00f8, 0x41d2, 0xc000, 0x0000}, {0x00f8, 0x41ea, 0xc002, 0x0000},
    {0x00f8, 0x41f7, 0xc004, 0x0000}, {0x00f8, 0x4005, 0xc006, 0x0000},
    {0x00f8, 0x4013, 0xc008, 0x0000}, {0x00f8, 0x4021, 0xc00a, 0xffff}};
OamAttr data_ov004_020ba944[6] = {
    {0x00f8, 0x41d7, 0xc000, 0x0000}, {0x00f8, 0x41e5, 0xc002, 0x0000},
    {0x00f8, 0x41f3, 0xc004, 0x0000}, {0x00f8, 0x4000, 0xc006, 0x0000},
    {0x00f8, 0x400e, 0xc008, 0x0000}, {0x00f8, 0x401c, 0xc00a, 0xffff}};
OamAttr data_ov004_020ba974[6] = {
    {0x00f8, 0x41d5, 0xc000, 0x0000}, {0x00f8, 0x41e4, 0xc002, 0x0000},
    {0x00f8, 0x41f3, 0xc004, 0x0000}, {0x00f8, 0x4003, 0xc006, 0x0000},
    {0x00f8, 0x4011, 0xc008, 0x0000}, {0x00f8, 0x401e, 0xc00a, 0xffff}};
OamAttr data_ov004_020ba9a4[6] = {
    {0x00f0, 0x81b0, 0xc000, 0x0000}, {0x00f0, 0x81c9, 0xc004, 0x0000},
    {0x00f0, 0x81e7, 0xc008, 0x0000}, {0x00f0, 0x81ff, 0xc00c, 0x0000},
    {0x00f0, 0x8017, 0xc010, 0x0000}, {0x00f0, 0x802f, 0xc014, 0xffff}};
OamAttr data_ov004_020ba9d4[6] = {
    {0x00f0, 0x81a5, 0x8000, 0x0000}, {0x00f0, 0x81c3, 0x9004, 0x0000},
    {0x00f0, 0x81e1, 0xa008, 0x0000}, {0x00f0, 0x81fe, 0xb00c, 0x0000},
    {0x00f0, 0x801d, 0xc010, 0x0000}, {0x00f0, 0x803a, 0x8014, 0xffff}};
OamAttr data_ov004_020baa04[6] = {
    {0x00f0, 0x81a6, 0x8000, 0x0000}, {0x00f0, 0x81c4, 0x9004, 0x0000},
    {0x00f0, 0x81e0, 0xa008, 0x0000}, {0x00f0, 0x81fd, 0xb00c, 0x0000},
    {0x00f0, 0x801c, 0xc010, 0x0000}, {0x00f0, 0x8039, 0x8014, 0xffff}};
OamAttr data_ov004_020baa34[6] = {
    {0x00f0, 0x81a6, 0x9000, 0x0000}, {0x00f0, 0x81c3, 0x9004, 0x0000},
    {0x00f0, 0x81e1, 0x9008, 0x0000}, {0x00f0, 0x81fe, 0x900c, 0x0000},
    {0x00f0, 0x801d, 0x9010, 0x0000}, {0x00f0, 0x803a, 0x9014, 0xffff}};
OamAttr data_ov004_020baa64[7] = {
    {0x00f8, 0x41cf, 0xa000, 0x0000}, {0x00f8, 0x41dd, 0xa002, 0x0000},
    {0x00f8, 0x41ea, 0xa004, 0x0000}, {0x00f8, 0x41f8, 0xa006, 0x0000},
    {0x00f8, 0x4007, 0xa008, 0x0000}, {0x00f8, 0x4015, 0xa00a, 0x0000},
    {0x00f8, 0x4022, 0xa00c, 0xffff}};
OamAttr data_ov004_020baa9c[7] = {
    {0x00f0, 0x8199, 0xa000, 0x0000}, {0x00f0, 0x81b5, 0xa004, 0x0000},
    {0x00f0, 0x81d2, 0xa008, 0x0000}, {0x00f0, 0x81ef, 0xa00c, 0x0000},
    {0x00f0, 0x800c, 0xa010, 0x0000}, {0x00f0, 0x802b, 0xa014, 0x0000},
    {0x00f0, 0x8048, 0xa018, 0xffff}};
OamAttr data_ov004_020baad4[7] = {
    {0x00f0, 0x819c, 0x8000, 0x0000}, {0x00f0, 0x81ba, 0x9004, 0x0000},
    {0x00f0, 0x81d8, 0xa008, 0x0000}, {0x00f0, 0x81f0, 0xb00c, 0x0000},
    {0x00f0, 0x8009, 0xc010, 0x0000}, {0x00f0, 0x8027, 0x8014, 0x0000},
    {0x00f0, 0x8045, 0x9018, 0xffff}};
OamAttr data_ov004_020bab0c[7] = {
    {0x00f0, 0x81a1, 0x8000, 0x0000}, {0x00f0, 0x81ba, 0x9004, 0x0000},
    {0x00f0, 0x81d8, 0xa008, 0x0000}, {0x00f0, 0x81f5, 0xb00c, 0x0000},
    {0x00f0, 0x800e, 0xc010, 0x0000}, {0x00f0, 0x8026, 0x8014, 0x0000},
    {0x00f0, 0x803e, 0x9018, 0xffff}};
OamAttr data_ov004_020bab44[7] = {
    {0x00f0, 0x818a, 0xa000, 0x0000}, {0x00f0, 0x81a6, 0xa004, 0x0000},
    {0x00f0, 0x81c1, 0xa008, 0x0000}, {0x00f0, 0x81de, 0xa00c, 0x0000},
    {0x00f0, 0x81fb, 0xa010, 0x0000}, {0x00f0, 0x8031, 0xa014, 0x0000},
    {0x00f0, 0x804e, 0xa00c, 0xffff}};
OamAttr data_ov004_020bab7c[7] = {
    {0x00f8, 0x41d1, 0xc000, 0x0000}, {0x00f8, 0x41e1, 0xc002, 0x0000},
    {0x00f8, 0x41ef, 0xc004, 0x0000}, {0x00f8, 0x41f8, 0xc006, 0x0000},
    {0x00f8, 0x4006, 0xc008, 0x0000}, {0x00f8, 0x4014, 0xc00a, 0x0000},
    {0x00f8, 0x4021, 0xc00c, 0xffff}};
OamAttr data_ov004_020babb4[7] = {
    {0x00f0, 0x819a, 0x9000, 0x0000}, {0x00f0, 0x81b8, 0x9004, 0x0000},
    {0x00f0, 0x81d6, 0x9008, 0x0000}, {0x00f0, 0x81f4, 0x900c, 0x0000},
    {0x00f0, 0x8010, 0x9010, 0x0000}, {0x00f0, 0x802d, 0x9014, 0x0000},
    {0x00f0, 0x8046, 0x9018, 0xffff}};
OamAttr data_ov004_020babec[7] = {
    {0x00f0, 0x819a, 0x8000, 0x0000}, {0x00f0, 0x81b7, 0x9004, 0x0000},
    {0x00f0, 0x81d4, 0xa008, 0x0000}, {0x00f0, 0x81f0, 0xb00c, 0x0000},
    {0x00f0, 0x800d, 0xc010, 0x0000}, {0x00f0, 0x802a, 0x8014, 0x0000},
    {0x00f0, 0x8046, 0x9018, 0xffff}};
OamAttr data_ov004_020bac24[7] = {
    {0x00f8, 0x41d1, 0xc000, 0x0000}, {0x00f8, 0x41df, 0xc002, 0x0000},
    {0x00f8, 0x41ec, 0xc004, 0x0000}, {0x00f8, 0x41f9, 0xc006, 0x0000},
    {0x00f8, 0x4007, 0xc008, 0x0000}, {0x00f8, 0x4015, 0xc00a, 0x0000},
    {0x00f8, 0x4022, 0xc00c, 0xffff}};
OamAttr data_ov004_020bac5c[7] = {
    {0x00f8, 0x41d1, 0x8000, 0x0000}, {0x00f8, 0x41df, 0x8002, 0x0000},
    {0x00f8, 0x41e7, 0x8004, 0x0000}, {0x00f8, 0x41f6, 0x8006, 0x0000},
    {0x00f8, 0x4005, 0x8008, 0x0000}, {0x00f8, 0x4013, 0x800a, 0x0000},
    {0x00f8, 0x4021, 0x800c, 0xffff}};
OamAttr data_ov004_020bac94[7] = {
    {0x00f0, 0x819a, 0x8000, 0x0000}, {0x00f0, 0x81b7, 0x9004, 0x0000},
    {0x00f0, 0x81d7, 0xa008, 0x0000}, {0x00f0, 0x81f5, 0xb00c, 0x0000},
    {0x00f0, 0x800e, 0xc010, 0x0000}, {0x00f0, 0x8026, 0x8014, 0x0000},
    {0x00f0, 0x8043, 0x9018, 0xffff}};
OamAttr data_ov004_020baccc[7] = {
    {0x00f8, 0x41cf, 0xc000, 0x0000}, {0x00f8, 0x41e5, 0xc002, 0x0000},
    {0x00f8, 0x41f2, 0xc004, 0x0000}, {0x00f8, 0x4000, 0xc006, 0x0000},
    {0x00f8, 0x4009, 0xc008, 0x0000}, {0x00f8, 0x4017, 0xc00a, 0x0000},
    {0x00f8, 0x4024, 0xc00c, 0xffff}};
OamAttr data_ov004_020bad04[7] = {
    {0x00f8, 0x41d0, 0xc000, 0x0000}, {0x00f8, 0x41de, 0xc002, 0x0000},
    {0x00f8, 0x41eb, 0xc004, 0x0000}, {0x00f8, 0x41fa, 0xc006, 0x0000},
    {0x00f8, 0x4009, 0xc008, 0x0000}, {0x00f8, 0x4017, 0xc00a, 0x0000},
    {0x00f8, 0x4024, 0xc00c, 0xffff}};
OamAttr data_ov004_020bad3c[7] = {
    {0x00f8, 0x41cd, 0xc000, 0x0000}, {0x00f8, 0x41e4, 0xc002, 0x0000},
    {0x00f8, 0x41f2, 0xc004, 0x0000}, {0x00f8, 0x4001, 0xc006, 0x0000},
    {0x00f8, 0x400f, 0xc008, 0x0000}, {0x00f8, 0x401d, 0xc00a, 0x0000},
    {0x00f8, 0x402b, 0xc00c, 0xffff}};
OamAttr data_ov004_020bad74[7] = {
    {0x00f8, 0x41ce, 0xc000, 0x0000}, {0x00f8, 0x41dc, 0xc002, 0x0000},
    {0x00f8, 0x41eb, 0xc004, 0x0000}, {0x00f8, 0x41f9, 0xc006, 0x0000},
    {0x00f8, 0x4006, 0xc008, 0x0000}, {0x00f8, 0x4016, 0xc00a, 0x0000},
    {0x00f8, 0x4024, 0xc00c, 0xffff}};
OamAttr data_ov004_020badac[7] = {
    {0x00f8, 0x41cd, 0xc000, 0x0000}, {0x00f8, 0x41e4, 0xc002, 0x0000},
    {0x00f8, 0x41f2, 0xc004, 0x0000}, {0x00f8, 0x4001, 0xc006, 0x0000},
    {0x00f8, 0x400f, 0xc008, 0x0000}, {0x00f8, 0x401d, 0xc00a, 0x0000},
    {0x00f8, 0x402b, 0xc00c, 0xffff}};
OamAttr data_ov004_020bade4[7] = {
    {0x00f8, 0x41d3, 0xc000, 0x0000}, {0x00f8, 0x41e1, 0xc002, 0x0000},
    {0x00f8, 0x41ea, 0xc004, 0x0000}, {0x00f8, 0x41f9, 0xc006, 0x0000},
    {0x00f8, 0x4002, 0xc008, 0x0000}, {0x00f8, 0x4011, 0xc00a, 0x0000},
    {0x00f8, 0x401f, 0xc00c, 0xffff}};
OamAttr data_ov004_020bae1c[7] = {
    {0x00f0, 0x819c, 0xc000, 0x0000}, {0x00f0, 0x81b9, 0xc004, 0x0000},
    {0x00f0, 0x81d7, 0xc008, 0x0000}, {0x00f0, 0x81f5, 0xc00c, 0x0000},
    {0x00f0, 0x800d, 0xc010, 0x0000}, {0x00f0, 0x8025, 0xc014, 0x0000},
    {0x00f0, 0x8044, 0xc018, 0xffff}};
OamAttr data_ov004_020bae54[7] = {
    {0x00f8, 0x41cd, 0xc000, 0x0000}, {0x00f8, 0x41e4, 0xc002, 0x0000},
    {0x00f8, 0x41f2, 0xc004, 0x0000}, {0x00f8, 0x4001, 0xc006, 0x0000},
    {0x00f8, 0x400f, 0xc008, 0x0000}, {0x00f8, 0x401d, 0xc00a, 0x0000},
    {0x00f8, 0x402b, 0xc00c, 0xffff}};
OamAttr data_ov004_020bae8c[7] = {
    {0x00f0, 0x8196, 0xc000, 0x0000}, {0x00f0, 0x81ae, 0xc004, 0x0000},
    {0x00f0, 0x81cd, 0xc008, 0x0000}, {0x00f0, 0x81ea, 0xc00c, 0x0000},
    {0x00f0, 0x8008, 0xc010, 0x0000}, {0x00f0, 0x8026, 0xc014, 0x0000},
    {0x00f0, 0x8044, 0xc018, 0xffff}};
OamAttr data_ov004_020baec4[7] = {
    {0x00f8, 0x41cf, 0xc000, 0x0000}, {0x00f8, 0x41dd, 0xc002, 0x0000},
    {0x00f8, 0x41ea, 0xc004, 0x0000}, {0x00f8, 0x41f9, 0xc006, 0x0000},
    {0x00f8, 0x4008, 0xc008, 0x0000}, {0x00f8, 0x4018, 0xc00c, 0x0000},
    {0x00f8, 0x4025, 0xc00e, 0xffff}};
OamAttr data_ov004_020baefc[7] = {
    {0x00f8, 0x41ca, 0xc000, 0x0000}, {0x00f8, 0x41d9, 0xc002, 0x0000},
    {0x00f8, 0x41e7, 0xc004, 0x0000}, {0x00f8, 0x41f5, 0xc006, 0x0000},
    {0x00f8, 0x400b, 0xc008, 0x0000}, {0x00f8, 0x401b, 0xc00a, 0x0000},
    {0x00f8, 0x4028, 0xc00c, 0xffff}};
OamAttr data_ov004_020baf34[7] = {
    {0x00f0, 0x8195, 0x9000, 0x0000}, {0x00f0, 0x81b3, 0x9004, 0x0000},
    {0x00f0, 0x81d1, 0x9008, 0x0000}, {0x00f0, 0x81f0, 0x900c, 0x0000},
    {0x00f0, 0x800f, 0x9010, 0x0000}, {0x00f0, 0x802c, 0x9014, 0x0000},
    {0x00f0, 0x804b, 0x9018, 0xffff}};
OamAttr data_ov004_020baf6c[8] = {
    {0x00f0, 0x818c, 0xa000, 0x0000}, {0x00f0, 0x81aa, 0xa004, 0x0000},
    {0x00f0, 0x81c7, 0xa008, 0x0000}, {0x00f0, 0x81e0, 0xa00c, 0x0000},
    {0x00f0, 0x81f8, 0xa010, 0x0000}, {0x00f0, 0x8016, 0xa014, 0x0000},
    {0x00f0, 0x8035, 0xa018, 0x0000}, {0x00f0, 0x8052, 0xa01c, 0xffff}};
OamAttr data_ov004_020bafac[8] = {
    {0x00f0, 0x8187, 0xa000, 0x0000}, {0x00f0, 0x81a4, 0xa004, 0x0000},
    {0x00f0, 0x81c2, 0xa008, 0x0000}, {0x00f0, 0x81e0, 0xa00c, 0x0000},
    {0x00f0, 0x81fd, 0xa010, 0x0000}, {0x00f0, 0x801d, 0xa014, 0x0000},
    {0x00f0, 0x803b, 0xa018, 0x0000}, {0x00f0, 0x8059, 0xa01c, 0xffff}};
OamAttr data_ov004_020bafec[8] = {
    {0x00f0, 0x818c, 0xa000, 0x0000}, {0x00f0, 0x81aa, 0xa004, 0x0000},
    {0x00f0, 0x81c2, 0xa008, 0x0000}, {0x00f0, 0x81ea, 0xa00c, 0x0000},
    {0x00f0, 0x8002, 0xa010, 0x0000}, {0x00f0, 0x801a, 0xa014, 0x0000},
    {0x00f0, 0x8038, 0xa018, 0x0000}, {0x00f0, 0x8051, 0xa01c, 0xffff}};
OamAttr data_ov004_020bb02c[8] = {
    {0x00f8, 0x400c, 0xc008, 0x0000}, {0x00f8, 0x41b8, 0xc000, 0x0000},
    {0x00f8, 0x41d0, 0xc002, 0x0000}, {0x00f8, 0x41de, 0xc004, 0x0000},
    {0x00f8, 0x41f4, 0xc006, 0x0000}, {0x00f8, 0x4018, 0xc00a, 0x0000},
    {0x00f8, 0x4023, 0xc00c, 0x0000}, {0x00f8, 0x4031, 0xc00e, 0xffff}};
OamAttr data_ov004_020bb06c[8] = {
    {0x00f8, 0x41b8, 0xc000, 0x0000}, {0x00f8, 0x41d0, 0xc002, 0x0000},
    {0x00f8, 0x41de, 0xc004, 0x0000}, {0x00f8, 0x41f4, 0xc006, 0x0000},
    {0x00f8, 0x400c, 0xc008, 0x0000}, {0x00f8, 0x4018, 0xc00a, 0x0000},
    {0x00f8, 0x4023, 0xc00c, 0x0000}, {0x00f8, 0x4031, 0xc00e, 0xffff}};
OamAttr data_ov004_020bb0ac[8] = {
    {0x00f0, 0x818f, 0x8000, 0x0000}, {0x00f0, 0x81ab, 0x9004, 0x0000},
    {0x00f0, 0x81cb, 0xa008, 0x0000}, {0x00f0, 0x81e8, 0xb00c, 0x0000},
    {0x00f0, 0x8004, 0xc010, 0x0000}, {0x00f0, 0x8023, 0x8014, 0x0000},
    {0x00f0, 0x8041, 0x9018, 0x0000}, {0x00f0, 0x8059, 0xa01c, 0xffff}};
OamAttr data_ov004_020bb0ec[8] = {
    {0x00f0, 0x818a, 0x8000, 0x0000}, {0x00f0, 0x81a6, 0x9004, 0x0000},
    {0x00f0, 0x81c6, 0xa008, 0x0000}, {0x00f0, 0x81e3, 0xb00c, 0x0000},
    {0x00f0, 0x81ff, 0xc010, 0x0000}, {0x00f0, 0x801e, 0x8014, 0x0000},
    {0x00f0, 0x803c, 0x9018, 0x0000}, {0x00f0, 0x8055, 0xa01c, 0xffff}};
OamAttr data_ov004_020bb12c[8] = {
    {0x00f0, 0x818b, 0x8000, 0x0000}, {0x00f0, 0x81a7, 0x9004, 0x0000},
    {0x00f0, 0x81c7, 0xa008, 0x0000}, {0x00f0, 0x81e4, 0xb00c, 0x0000},
    {0x00f0, 0x8000, 0xc010, 0x0000}, {0x00f0, 0x8020, 0x8014, 0x0000},
    {0x00f0, 0x803d, 0x9018, 0x0000}, {0x00f0, 0x8056, 0xa01c, 0xffff}};
OamAttr data_ov004_020bb16c[8] = {
    {0x00f0, 0x8196, 0x9000, 0x0000}, {0x00f0, 0x81b3, 0x9004, 0x0000},
    {0x00f0, 0x81d1, 0x9008, 0x0000}, {0x00f0, 0x81ee, 0x900c, 0x0000},
    {0x00f0, 0x800a, 0x9010, 0x0000}, {0x00f0, 0x8022, 0x9014, 0x0000},
    {0x00f0, 0x803a, 0x900c, 0x0000}, {0x00ef, 0x8050, 0x9018, 0xffff}};
OamAttr data_ov004_020bb1ac[8] = {
    {0x00f0, 0x8187, 0x9000, 0x0000}, {0x00f0, 0x819f, 0x9004, 0x0000},
    {0x00f0, 0x81b4, 0x9008, 0x0000}, {0x00f0, 0x81cf, 0x900c, 0x0000},
    {0x00f0, 0x81f7, 0x9004, 0x0000}, {0x00f0, 0x800f, 0x9010, 0x0000},
    {0x00f0, 0x8038, 0x9014, 0x0000}, {0x00f0, 0x8054, 0x9018, 0xffff}};
OamAttr data_ov004_020bb1ec[8] = {
    {0x00f8, 0x41c5, 0xc000, 0x0000}, {0x00f8, 0x41de, 0xc002, 0x0000},
    {0x00f8, 0x41ed, 0xc004, 0x0000}, {0x00f8, 0x41fb, 0xc006, 0x0000},
    {0x00f8, 0x4004, 0xc008, 0x0000}, {0x00f8, 0x4011, 0xc00a, 0x0000},
    {0x00f8, 0x401f, 0xc00c, 0x0000}, {0x00f8, 0x402d, 0xc00e, 0xffff}};
OamAttr data_ov004_020bb22c[8] = {
    {0x00f8, 0x41c5, 0xc000, 0x0000}, {0x00f8, 0x41de, 0xc002, 0x0000},
    {0x00f8, 0x41ed, 0xc004, 0x0000}, {0x00f8, 0x41fb, 0xc006, 0x0000},
    {0x00f8, 0x4004, 0xc008, 0x0000}, {0x00f8, 0x4011, 0xc00a, 0x0000},
    {0x00f8, 0x401f, 0xc00c, 0x0000}, {0x00f8, 0x402d, 0xc00e, 0xffff}};
OamAttr data_ov004_020bb26c[8] = {
    {0x00f0, 0x8190, 0x9000, 0x0000}, {0x00f0, 0x81ab, 0x9004, 0x0000},
    {0x00f0, 0x81c4, 0x9008, 0x0000}, {0x00f0, 0x81dd, 0x900c, 0x0000},
    {0x00f0, 0x81f9, 0x9010, 0x0000}, {0x00f0, 0x8015, 0x9014, 0x0000},
    {0x00f0, 0x8032, 0x9018, 0x0000}, {0x00f0, 0x804f, 0x901c, 0xffff}};
OamAttr data_ov004_020bb2ac[8] = {
    {0x00f0, 0x8187, 0x9000, 0x0000}, {0x00f0, 0x81a3, 0x9004, 0x0000},
    {0x00f0, 0x81bd, 0x9008, 0x0000}, {0x00f0, 0x81d6, 0x900c, 0x0000},
    {0x00f0, 0x8007, 0x9010, 0x0000}, {0x00f0, 0x8021, 0x9014, 0x0000},
    {0x00f0, 0x803a, 0x900c, 0x0000}, {0x00f0, 0x8058, 0x9018, 0xffff}};
OamAttr data_ov004_020bb2ec[8] = {
    {0x00f0, 0x8187, 0x9000, 0x0000}, {0x00f0, 0x81a3, 0x9004, 0x0000},
    {0x00f0, 0x81bd, 0x9008, 0x0000}, {0x00f0, 0x81d6, 0x900c, 0x0000},
    {0x00f0, 0x8007, 0x9010, 0x0000}, {0x00f0, 0x8021, 0x9014, 0x0000},
    {0x00f0, 0x803a, 0x900c, 0x0000}, {0x00f0, 0x8058, 0x9018, 0xffff}};
OamAttr data_ov004_020bb32c[8] = {
    {0x00f0, 0x8187, 0x9000, 0x0000}, {0x00f0, 0x81a3, 0x9004, 0x0000},
    {0x00f0, 0x81bd, 0x9008, 0x0000}, {0x00f0, 0x81d6, 0x900c, 0x0000},
    {0x00f0, 0x8007, 0x9010, 0x0000}, {0x00f0, 0x8021, 0x9014, 0x0000},
    {0x00f0, 0x803a, 0x900c, 0x0000}, {0x00f0, 0x8058, 0x9018, 0xffff}};
OamAttr data_ov004_020bb36c[8] = {
    {0x00f0, 0x8187, 0x9000, 0x0000}, {0x00f0, 0x81a3, 0x9004, 0x0000},
    {0x00f0, 0x81bd, 0x9008, 0x0000}, {0x00f0, 0x81d6, 0x900c, 0x0000},
    {0x00f0, 0x8007, 0x9010, 0x0000}, {0x00f0, 0x8021, 0x9014, 0x0000},
    {0x00f0, 0x803a, 0x900c, 0x0000}, {0x00f0, 0x8058, 0x9018, 0xffff}};
OamAttr data_ov004_020bb3ac[8] = {
    {0x00f0, 0x8187, 0x9000, 0x0000}, {0x00f0, 0x81a3, 0x9004, 0x0000},
    {0x00f0, 0x81bd, 0x9008, 0x0000}, {0x00f0, 0x81d6, 0x900c, 0x0000},
    {0x00f0, 0x8007, 0x9010, 0x0000}, {0x00f0, 0x8021, 0x9014, 0x0000},
    {0x00f0, 0x803a, 0x900c, 0x0000}, {0x00f0, 0x8058, 0x9018, 0xffff}};
OamAttr data_ov004_020bb3ec[9] = {
    {0x00f8, 0x41c1, 0xa000, 0x0000}, {0x00f8, 0x41cd, 0xa002, 0x0000},
    {0x00f8, 0x41d9, 0xa004, 0x0000}, {0x00f8, 0x41e7, 0xa000, 0x0000},
    {0x00f8, 0x41f6, 0xa006, 0x0000}, {0x00f8, 0x4004, 0xa008, 0x0000},
    {0x00f8, 0x4013, 0xa00a, 0x0000}, {0x00f8, 0x4021, 0xa00c, 0x0000},
    {0x00f8, 0x402f, 0xa00e, 0xffff}};
OamAttr data_ov004_020bb434[9] = {
    {0x00f8, 0x41be, 0xa000, 0x0000}, {0x00f8, 0x41ca, 0xa002, 0x0000},
    {0x00f8, 0x41d6, 0xa004, 0x0000}, {0x00f8, 0x41e4, 0xa000, 0x0000},
    {0x00f8, 0x41fe, 0xa006, 0x0000}, {0x00f8, 0x400c, 0xa008, 0x0000},
    {0x00f8, 0x401b, 0xa00a, 0x0000}, {0x00f8, 0x4029, 0xa00c, 0x0000},
    {0x00f8, 0x4037, 0xa00e, 0xffff}};
OamAttr data_ov004_020bb47c[9] = {
    {0x00f0, 0x81a6, 0xa000, 0x0000}, {0x00f0, 0x81b8, 0xa004, 0x0000},
    {0x00f0, 0x81ca, 0xa008, 0x0000}, {0x00f0, 0x81df, 0xa008, 0x0000},
    {0x00f0, 0x81f3, 0xa014, 0x0000}, {0x00f0, 0x8005, 0xa00c, 0x0000},
    {0x00f0, 0x8016, 0xa010, 0x0000}, {0x00f0, 0x8027, 0xa014, 0x0000},
    {0x00f0, 0x8039, 0xa018, 0xffff}};
OamAttr data_ov004_020bb4c4[9] = {
    {0x00f8, 0x41bb, 0xc000, 0x0000}, {0x00f8, 0x41c9, 0xc002, 0x0000},
    {0x00f8, 0x41d7, 0xc004, 0x0000}, {0x00f8, 0x41e5, 0xc006, 0x0000},
    {0x00f8, 0x41fe, 0xc008, 0x0000}, {0x00f8, 0x400d, 0xc00a, 0x0000},
    {0x00f8, 0x401b, 0xc00c, 0x0000}, {0x00f8, 0x402a, 0xc00e, 0x0000},
    {0x00f8, 0x4038, 0xc010, 0xffff}};
OamAttr data_ov004_020bb50c[9] = {
    {0x00f0, 0x818d, 0x9000, 0x0000}, {0x00f0, 0x81ac, 0x9004, 0x0000},
    {0x00f0, 0x81c5, 0x9008, 0x0000}, {0x80f0, 0x81e4, 0x900c, 0x0000},
    {0x00f0, 0x81f0, 0x900e, 0x0000}, {0x80f0, 0x800d, 0x900c, 0x0000},
    {0x00f0, 0x801a, 0x9012, 0x0000}, {0x00f0, 0x8035, 0x9016, 0x0000},
    {0x00f0, 0x8055, 0x901a, 0xffff}};
OamAttr data_ov004_020bb554[9] = {
    {0x00f8, 0x41bb, 0xc000, 0x0000}, {0x00f8, 0x41c9, 0xc002, 0x0000},
    {0x00f8, 0x41d7, 0xc004, 0x0000}, {0x00f8, 0x41e5, 0xc006, 0x0000},
    {0x00f8, 0x41fe, 0xc008, 0x0000}, {0x00f8, 0x400d, 0xc00a, 0x0000},
    {0x00f8, 0x401b, 0xc00c, 0x0000}, {0x00f8, 0x402a, 0xc00e, 0x0000},
    {0x00f8, 0x4038, 0xc010, 0xffff}};
OamAttr data_ov004_020bb59c[9] = {
    {0x00f8, 0x41c8, 0xc000, 0x0000}, {0x00f8, 0x41d2, 0xc002, 0x0000},
    {0x00f8, 0x41dd, 0xc004, 0x0000}, {0x00f8, 0x41ea, 0xc006, 0x0000},
    {0x00f8, 0x41fc, 0xc008, 0x0000}, {0x00f8, 0x4009, 0xc00a, 0x0000},
    {0x00f8, 0x4015, 0xc008, 0x0000}, {0x00f8, 0x401f, 0xc00c, 0x0000},
    {0x00f8, 0x4029, 0xc00e, 0xffff}};
OamAttr data_ov004_020bb5e4[9] = {
    {0x00f0, 0x81a9, 0x8000, 0x0000}, {0x00f0, 0x81be, 0x9004, 0x0000},
    {0x00f0, 0x81cd, 0xa008, 0x0000}, {0x00f0, 0x81df, 0xb00c, 0x0000},
    {0x00f0, 0x81f2, 0xc010, 0x0000}, {0x00f0, 0x8003, 0x8014, 0x0000},
    {0x00f0, 0x801b, 0x9018, 0x0000}, {0x00f0, 0x802f, 0xa018, 0x0000},
    {0x00f0, 0x8040, 0xb01c, 0xffff}};
OamAttr data_ov004_020bb62c[9] = {
    {0x00f0, 0x8188, 0x8000, 0x0000}, {0x00f0, 0x819f, 0x9004, 0x0000},
    {0x00f0, 0x81b4, 0xa008, 0x0000}, {0x00f0, 0x81ce, 0xb000, 0x0000},
    {0x00f0, 0x81ea, 0xc00c, 0x0000}, {0x00f0, 0x8005, 0x9010, 0x0000},
    {0x00f0, 0x801f, 0xa014, 0x0000}, {0x00f0, 0x803b, 0xb018, 0x0000},
    {0x00f0, 0x8055, 0xc01c, 0xffff}};
OamAttr data_ov004_020bb674[9] = {
    {0x00f0, 0x8181, 0x8000, 0x0000}, {0x00f0, 0x8198, 0x9004, 0x0000},
    {0x00f0, 0x81ad, 0xa008, 0x0000}, {0x00f0, 0x81c8, 0xb000, 0x0000},
    {0x00f0, 0x81f2, 0xc00c, 0x0000}, {0x00f0, 0x800d, 0x9010, 0x0000},
    {0x00f0, 0x8027, 0xa014, 0x0000}, {0x00f0, 0x8043, 0xb018, 0x0000},
    {0x00f0, 0x805d, 0xc01c, 0xffff}};
OamAttr data_ov004_020bb6bc[10] = {
    {0x00f0, 0x819e, 0x9000, 0x0000}, {0x00f0, 0x81b7, 0x9004, 0x0000},
    {0x00f0, 0x81c9, 0x9008, 0x0000}, {0x80f0, 0x81e2, 0x900c, 0x0000},
    {0x00f0, 0x81e9, 0x900e, 0x0000}, {0x00f0, 0x81fa, 0x9012, 0x0000},
    {0x00f0, 0x800c, 0x9008, 0x0000}, {0x00f0, 0x801d, 0x9016, 0x0000},
    {0x80f0, 0x8035, 0x901a, 0x0000}, {0x00f0, 0x8042, 0x901c, 0xffff}};
OamAttr data_ov004_020bb70c[10] = {
    {0x00f0, 0x8180, 0x9000, 0x0000}, {0x00f0, 0x8194, 0x9004, 0x0000},
    {0x00f0, 0x81a9, 0x9008, 0x0000}, {0x00f0, 0x81c1, 0x900c, 0x0000},
    {0x00f0, 0x81d7, 0x9010, 0x0000}, {0x00f0, 0x81fb, 0x9014, 0x0000},
    {0x00f0, 0x8011, 0x900c, 0x0000}, {0x00f0, 0x8034, 0x9018, 0x0000},
    {0x00f0, 0x8049, 0x9004, 0x0000}, {0x00f0, 0x8060, 0x901c, 0xffff}};
OamAttr data_ov004_020bb75c[10] = {
    {0x00f0, 0x819a, 0x9000, 0x0000}, {0x00f0, 0x81ae, 0x9004, 0x0000},
    {0x00f0, 0x81c0, 0x9008, 0x0000}, {0x00f0, 0x81cf, 0x9004, 0x0000},
    {0x00f0, 0x81f2, 0x900c, 0x0000}, {0x00f0, 0x8005, 0x9010, 0x0000},
    {0x00f0, 0x8016, 0x9014, 0x0000}, {0x00f0, 0x8027, 0x9018, 0x0000},
    {0x00f0, 0x8037, 0x901c, 0x0000}, {0x00f0, 0x8049, 0x9014, 0xffff}};
OamAttr data_ov004_020bb7ac[11] = {
    {0x00f0, 0x819a, 0x9000, 0x0000}, {0x80f0, 0x81b2, 0x9004, 0x0000},
    {0x00f0, 0x81ba, 0x9006, 0x0000}, {0x80f0, 0x81d3, 0x900a, 0x0000},
    {0x80f0, 0x81e3, 0x900c, 0x0000}, {0x00f0, 0x81fb, 0x9000, 0x0000},
    {0x80f0, 0x8013, 0x900e, 0x0000}, {0x80f0, 0x8020, 0x9010, 0x0000},
    {0x00f0, 0x8028, 0x9012, 0x0000}, {0x00f0, 0x8038, 0x9016, 0x0000},
    {0x80f0, 0x8050, 0x901a, 0xffff}};
OamAttr data_ov004_020bb804[11] = {
    {0x80f0, 0x81a5, 0x9000, 0x0000}, {0x80f0, 0x81b3, 0x9002, 0x0000},
    {0x80f0, 0x81c1, 0x9004, 0x0000}, {0x80f0, 0x81dc, 0x9006, 0x0000},
    {0x80f0, 0x81ea, 0x9008, 0x0000}, {0x00f0, 0x81f1, 0x900a, 0x0000},
    {0x00f0, 0x8001, 0x900e, 0x0000}, {0x00f0, 0x8012, 0x9012, 0x0000},
    {0x80f0, 0x802b, 0x9016, 0x0000}, {0x00f0, 0x8034, 0x9018, 0x0000},
    {0x80f0, 0x804e, 0x901c, 0xffff}};
OamAttr data_ov004_020bb85c[11] = {
    {0x00f8, 0x41ae, 0xc000, 0x0000}, {0x00f8, 0x41bc, 0xc002, 0x0000},
    {0x00f8, 0x41d4, 0xc004, 0x0000}, {0x00f8, 0x41e2, 0xc006, 0x0000},
    {0x00f8, 0x41ef, 0xc008, 0x0000}, {0x00f8, 0x4007, 0xc00a, 0x0000},
    {0x00f8, 0x4015, 0xc00c, 0x0000}, {0x00f8, 0x401e, 0xc00e, 0x0000},
    {0x00f8, 0x402b, 0xc010, 0x0000}, {0x00f8, 0x4039, 0xc012, 0x0000},
    {0x00f8, 0x4047, 0xc014, 0xffff}};
OamAttr data_ov004_020bb8b4[11] = {
    {0x00f0, 0x8195, 0x9000, 0x0000}, {0x00f0, 0x81a7, 0x9004, 0x0000},
    {0x80f0, 0x81c0, 0x9008, 0x0000}, {0x80f0, 0x81d7, 0x9008, 0x0000},
    {0x00f0, 0x81e7, 0x900a, 0x0000}, {0x00f0, 0x81fc, 0x900e, 0x0000},
    {0x00f0, 0x800d, 0x9012, 0x0000}, {0x00f0, 0x801d, 0x9016, 0x0000},
    {0x00f0, 0x8030, 0x901a, 0x0000}, {0x80f0, 0x805b, 0x901e, 0x0000},
    {0x80f0, 0x8046, 0x9008, 0xffff}};
OamAttr data_ov004_020bb90c[11] = {
    {0x00f0, 0x8196, 0x9000, 0x0000}, {0x80f0, 0x81af, 0x9004, 0x0000},
    {0x00f0, 0x81b8, 0x9006, 0x0000}, {0x00f0, 0x81cb, 0x900a, 0x0000},
    {0x00f0, 0x81dc, 0x900e, 0x0000}, {0x80f0, 0x8002, 0x9004, 0x0000},
    {0x00f0, 0x800a, 0x9012, 0x0000}, {0x00f0, 0x801c, 0x9016, 0x0000},
    {0x00f0, 0x802e, 0x901a, 0x0000}, {0x80f0, 0x8046, 0x901e, 0x0000},
    {0x80f0, 0x8055, 0x9004, 0xffff}};
OamAttr data_ov004_020bb964[11] = {
    {0x00f8, 0x41a8, 0xa000, 0x0000}, {0x00f8, 0x41b9, 0xa002, 0x0000},
    {0x00f8, 0x41cf, 0xa004, 0x0000}, {0x00f8, 0x41db, 0xa006, 0x0000},
    {0x00f8, 0x41eb, 0xa008, 0x0000}, {0x00f8, 0x4002, 0xa00a, 0x0000},
    {0x00f8, 0x4010, 0xa00c, 0x0000}, {0x00f8, 0x401f, 0xa00e, 0x0000},
    {0x00f8, 0x402d, 0xa010, 0x0000}, {0x00f8, 0x403d, 0xa012, 0x0000},
    {0x00f8, 0x4048, 0xa014, 0xffff}};
OamAttr data_ov004_020bb9bc[11] = {
    {0x00f8, 0x41ae, 0xc000, 0x0000}, {0x00f8, 0x41bc, 0xc002, 0x0000},
    {0x00f8, 0x41cb, 0xc004, 0x0000}, {0x00f8, 0x41d9, 0xc006, 0x0000},
    {0x00f8, 0x41e7, 0xc008, 0x0000}, {0x00f8, 0x41ff, 0xc00a, 0x0000},
    {0x00f8, 0x400c, 0xc00c, 0x0000}, {0x00f8, 0x401a, 0xc00e, 0x0000},
    {0x00f8, 0x4028, 0xc010, 0x0000}, {0x00f8, 0x4035, 0xc012, 0x0000},
    {0x00f8, 0x4044, 0xc014, 0xffff}};
OamAttr data_ov004_020bba14[11] = {
    {0x00f8, 0x41b0, 0xa000, 0x0000}, {0x00f8, 0x41bd, 0xa002, 0x0000},
    {0x00f8, 0x41cd, 0xa004, 0x0000}, {0x00f8, 0x41dd, 0xa006, 0x0000},
    {0x00f8, 0x41e3, 0xa008, 0x0000}, {0x00f8, 0x41fc, 0xa00a, 0x0000},
    {0x00f8, 0x400a, 0xa00c, 0x0000}, {0x00f8, 0x4019, 0xa00e, 0x0000},
    {0x00f8, 0x4027, 0xa010, 0x0000}, {0x00f8, 0x4037, 0xa012, 0x0000},
    {0x00f8, 0x4042, 0xa014, 0xffff}};
OamAttr data_ov004_020bba6c[11] = {
    {0x00f0, 0x819f, 0x9000, 0x0000}, {0x00f0, 0x81b2, 0x9004, 0x0000},
    {0x00f0, 0x81c4, 0x9008, 0x0000}, {0x80f0, 0x81dc, 0x900c, 0x0000},
    {0x00f0, 0x81e5, 0x900e, 0x0000}, {0x00f0, 0x81f6, 0x9012, 0x0000},
    {0x00f0, 0x8007, 0x9004, 0x0000}, {0x00f0, 0x8019, 0x9008, 0x0000},
    {0x00f0, 0x8028, 0x9016, 0x0000}, {0x80f0, 0x803e, 0x901a, 0x0000},
    {0x80f0, 0x804e, 0x901c, 0xffff}};
OamAttr data_ov004_020bbac4[11] = {
    {0x00f8, 0x41a8, 0xa000, 0x0000}, {0x00f8, 0x41b7, 0xa002, 0x0000},
    {0x00f8, 0x41cf, 0xa004, 0x0000}, {0x00f8, 0x41db, 0xa006, 0x0000},
    {0x00f8, 0x41eb, 0xa008, 0x0000}, {0x00f8, 0x4002, 0xa00a, 0x0000},
    {0x00f8, 0x4010, 0xa00c, 0x0000}, {0x00f8, 0x401f, 0xa00e, 0x0000},
    {0x00f8, 0x402d, 0xa010, 0x0000}, {0x00f8, 0x403d, 0xa012, 0x0000},
    {0x00f8, 0x4048, 0xa014, 0xffff}};
OamAttr data_ov004_020bbb1c[12] = {
    {0x00f8, 0x41aa, 0xc000, 0x0000}, {0x00f8, 0x41b8, 0xc002, 0x0000},
    {0x00f8, 0x41c7, 0xc004, 0x0000}, {0x00f8, 0x41d6, 0xc006, 0x0000},
    {0x00f8, 0x41e5, 0xc008, 0x0000}, {0x00f8, 0x41ed, 0xc00a, 0x0000},
    {0x00f8, 0x4006, 0xc00c, 0x0000}, {0x00f8, 0x4014, 0xc00e, 0x0000},
    {0x00f8, 0x4023, 0xc010, 0x0000}, {0x00f8, 0x4032, 0xc012, 0x0000},
    {0x00f8, 0x4041, 0xc014, 0x0000}, {0x00f8, 0x4049, 0xc016, 0xffff}};
OamAttr data_ov004_020bbb7c[12] = {
    {0x00f0, 0x8192, 0x9000, 0x0000}, {0x80f0, 0x81ab, 0x9004, 0x0000},
    {0x00f0, 0x81b3, 0x9006, 0x0000}, {0x80f0, 0x81cd, 0x900a, 0x0000},
    {0x80f0, 0x81db, 0x9014, 0x0000}, {0x00f0, 0x81f0, 0x900c, 0x0000},
    {0x00f0, 0x8001, 0x9010, 0x0000}, {0x00f0, 0x8016, 0x901a, 0x0000},
    {0x80f0, 0x802c, 0x901e, 0x0000}, {0x00f0, 0x8035, 0x9016, 0x0000},
    {0x00f0, 0x8045, 0x9000, 0x0000}, {0x80f0, 0x805c, 0x9014, 0xffff}};
OamAttr data_ov004_020bbbdc[13] = {
    {0x00f8, 0x41a4, 0xa000, 0x0000}, {0x00f8, 0x41b3, 0xa002, 0x0000},
    {0x00f8, 0x41c0, 0xa004, 0x0000}, {0x00f8, 0x41c8, 0xa006, 0x0000},
    {0x00f8, 0x41d6, 0xa008, 0x0000}, {0x00f8, 0x41e4, 0xa00a, 0x0000},
    {0x00f8, 0x41f1, 0xa00c, 0x0000}, {0x00f8, 0x41ff, 0xa00e, 0x0000},
    {0x00f8, 0x4016, 0xa010, 0x0000}, {0x00f8, 0x4025, 0xa012, 0x0000},
    {0x00f8, 0x4033, 0xa014, 0x0000}, {0x00f8, 0x4042, 0xa016, 0x0000},
    {0x00f8, 0x4050, 0xa018, 0xffff}};
OamAttr data_ov004_020bbc44[13] = {
    {0x00f8, 0x41a4, 0xa000, 0x0000}, {0x00f8, 0x41b3, 0xa002, 0x0000},
    {0x00f8, 0x41c1, 0xa004, 0x0000}, {0x00f8, 0x41ce, 0xa006, 0x0000},
    {0x00f8, 0x41dc, 0xa008, 0x0000}, {0x00f8, 0x41e6, 0xa00a, 0x0000},
    {0x00f8, 0x41f1, 0xa00c, 0x0000}, {0x00f8, 0x400b, 0xa00e, 0x0000},
    {0x00f8, 0x4019, 0xa010, 0x0000}, {0x00f8, 0x4028, 0xa012, 0x0000},
    {0x00f8, 0x4036, 0xa014, 0x0000}, {0x00f8, 0x4042, 0xa016, 0x0000},
    {0x00f8, 0x404e, 0xa018, 0xffff}};
OamAttr data_ov004_020bbcac[13] = {
    {0x00f0, 0x8197, 0xc000, 0x0000}, {0x80f0, 0x81b1, 0xc004, 0x0000},
    {0x80f0, 0x81c1, 0xc014, 0x0000}, {0x80f0, 0x81ce, 0xc004, 0x0000},
    {0x00f0, 0x81d6, 0xc006, 0x0000}, {0x00f0, 0x81e7, 0xc00a, 0x0000},
    {0x80f0, 0x81ff, 0xc00e, 0x0000}, {0x80f0, 0x800d, 0xc010, 0x0000},
    {0x80f0, 0x8019, 0xc012, 0x0000}, {0x80f0, 0x8026, 0xc014, 0x0000},
    {0x80f0, 0x8034, 0xc016, 0x0000}, {0x80f0, 0x8044, 0xc014, 0x0000},
    {0x80f0, 0x8052, 0xc004, 0xffff}};
OamAttr data_ov004_020bbd14[13] = {
    {0x00f0, 0x8192, 0x8000, 0x0000}, {0x80f0, 0x81ab, 0x9004, 0x0000},
    {0x80f0, 0x81b6, 0xa006, 0x0000}, {0x80f0, 0x81c3, 0xb008, 0x0000},
    {0x80f0, 0x81d2, 0xc008, 0x0000}, {0x80f0, 0x81e1, 0x8004, 0x0000},
    {0x00f0, 0x81e9, 0x900a, 0x0000}, {0x00f0, 0x81f9, 0xa00e, 0x0000},
    {0x00f0, 0x8017, 0xb012, 0x0000}, {0x80f0, 0x802e, 0xc016, 0x0000},
    {0x80f0, 0x803d, 0x8018, 0x0000}, {0x00f0, 0x8044, 0x900e, 0x0000},
    {0x80f0, 0x805b, 0xa004, 0xffff}};
OamAttr data_ov004_020bbd7c[14] = {
    {0x00f0, 0x818e, 0x9000, 0x0000}, {0x80f0, 0x81a7, 0x9004, 0x0000},
    {0x80f0, 0x81b6, 0x9006, 0x0000}, {0x00f0, 0x81bd, 0x9008, 0x0000},
    {0x80f0, 0x81d5, 0x900c, 0x0000}, {0x00f0, 0x81dd, 0x900e, 0x0000},
    {0x00f0, 0x81ee, 0x9012, 0x0000}, {0x00f0, 0x81ff, 0x9000, 0x0000},
    {0x00f0, 0x8011, 0x9008, 0x0000}, {0x80f0, 0x8028, 0x9016, 0x0000},
    {0x80f0, 0x8035, 0x9018, 0x0000}, {0x80f0, 0x8042, 0x901a, 0x0000},
    {0x80f0, 0x8050, 0x901c, 0x0000}, {0x80f0, 0x805f, 0x901e, 0xffff}};
OamAttr data_ov004_020bbdec[14] = {
    {0x80f0, 0x8187, 0xa000, 0x0000}, {0x00f0, 0x8190, 0xa002, 0x0000},
    {0x80f0, 0x81b3, 0xa006, 0x0000}, {0x00f0, 0x81ba, 0xa008, 0x0000},
    {0x00f0, 0x81cc, 0xa00c, 0x0000}, {0x80f0, 0x81e5, 0xa010, 0x0000},
    {0x00f0, 0x81f8, 0xa012, 0x0000}, {0x80f0, 0x8010, 0xa016, 0x0000},
    {0x00f0, 0x8015, 0xa018, 0x0000}, {0x80f0, 0x802e, 0xa01c, 0x0000},
    {0x80f0, 0x803d, 0xa01e, 0x0000}, {0x80f0, 0x804c, 0xa01e, 0x0000},
    {0x80f0, 0x805b, 0xa016, 0x0000}, {0x80f0, 0x8068, 0xa01e, 0xffff}};
OamAttr data_ov004_020bbe5c[14] = {
    {0x00f0, 0x8182, 0x9000, 0x0000}, {0x80f0, 0x819c, 0x900e, 0x0000},
    {0x80f0, 0x81a8, 0x9004, 0x0000}, {0x00f0, 0x81ae, 0x9006, 0x0000},
    {0x00f0, 0x81cd, 0x9012, 0x0000}, {0x80f0, 0x81e6, 0x900a, 0x0000},
    {0x80f0, 0x81f6, 0x900c, 0x0000}, {0x80f0, 0x8006, 0x900e, 0x0000},
    {0x80f0, 0x8015, 0x9010, 0x0000}, {0x00f0, 0x801d, 0x9012, 0x0000},
    {0x00f0, 0x802f, 0x9016, 0x0000}, {0x80f0, 0x8048, 0x901a, 0x0000},
    {0x80f0, 0x8057, 0x900e, 0x0000}, {0x00f0, 0x805f, 0x901c, 0xffff}};
OamAttr data_ov004_020bbecc[16] = {
    {0x00f8, 0x4025, 0xa014, 0x0000}, {0x00f8, 0x4033, 0xa016, 0x0000},
    {0x00f8, 0x4042, 0xa018, 0x0000}, {0x00f8, 0x4050, 0xa01a, 0x0000},
    {0x00f8, 0x4060, 0xa01c, 0x0000}, {0x00f8, 0x406a, 0xa01e, 0x0000},
    {0x00f8, 0x4186, 0xa000, 0x0000}, {0x00f8, 0x4197, 0xa002, 0x0000},
    {0x00f8, 0x41a5, 0xa004, 0x0000}, {0x00f8, 0x41b4, 0xa006, 0x0000},
    {0x00f8, 0x41c2, 0xa008, 0x0000}, {0x00f8, 0x41d0, 0xa00a, 0x0000},
    {0x00f8, 0x41e9, 0xa00c, 0x0000}, {0x00f8, 0x41f5, 0xa00e, 0x0000},
    {0x00f8, 0x4005, 0xa010, 0x0000}, {0x00f8, 0x400f, 0xa012, 0xffff}};

OamAttr *data_ov004_020b9e00 = data_ov004_020ba944;
OamAttr *data_ov004_020b9e04 = data_ov004_020ba074;
OamAttr *data_ov004_020b9e08 = data_ov004_020ba914;
OamAttr *data_ov004_020b9e0c = data_ov004_020bae8c;
OamAttr *data_ov004_020b9e10 = data_ov004_020ba5cc;
OamAttr *data_ov004_020b9e14 = data_ov004_020ba974;
OamAttr *data_ov004_020b9e18 = data_ov004_020bb5e4;
OamAttr *data_ov004_020b9e1c = data_ov004_020ba8e4;
OamAttr *data_ov004_020b9e20 = data_ov004_020bbb1c;
OamAttr *data_ov004_020b9e24 = data_ov004_020bb9bc;
OamAttr *data_ov004_020b9e28 = data_ov004_020ba2b4;
OamAttr *data_ov004_020b9e2c = data_ov004_020ba5a4;
OamAttr *data_ov004_020b9e30 = data_ov004_020badac;
OamAttr *data_ov004_020b9e34 = data_ov004_020ba2f4;
OamAttr *data_ov004_020b9e38 = data_ov004_020bac94;
OamAttr *data_ov004_020b9e3c = data_ov004_020ba884;
OamAttr *data_ov004_020b9e40 = data_ov004_020ba8b4;
OamAttr *data_ov004_020b9e44 = data_ov004_020baccc;
OamAttr *data_ov004_020b9e48 = data_ov004_020ba57c;
OamAttr *data_ov004_020b9e4c = data_ov004_020bb50c;
OamAttr *sMgMsgSuperEsperSpritesEnglish = data_ov004_020bb16c;
OamAttr *data_ov004_020b9e54 = data_ov004_020ba254;
OamAttr *data_ov004_020b9e58 = data_ov004_020ba854;
OamAttr *data_ov004_020b9e5c = data_ov004_020bad3c;
OamAttr *data_ov004_020b9e60 = data_ov004_020ba2d4;
OamAttr *data_ov004_020b9e64 = data_ov004_020bba14;
OamAttr *data_ov004_020b9e68 = data_ov004_020babb4;
OamAttr *data_ov004_020b9e6c = data_ov004_020bb75c;
OamAttr *sMgMsgNoPairSpritesEnglish = data_ov004_020ba1f4;
OamAttr *data_ov004_020b9e74 = data_ov004_020ba3d4;
OamAttr *sMgMsgFullHouseSpritesEnglish = data_ov004_020bb554;
OamAttr *sMgMsgTimeOverSpritesEnglish = data_ov004_020bb1ac;
OamAttr *data_ov004_020b9e80 = data_ov004_020ba554;
OamAttr *data_ov004_020b9e84 = data_ov004_020ba234;
OamAttr *data_ov004_020b9e88 = data_ov004_020ba704;
OamAttr *sMgMsgQuitSpritesEnglish = data_ov004_020ba214;
OamAttr *sMgMsgYouWinSpritesEnglish = data_ov004_020ba734;
OamAttr *data_ov004_020b9e94 = data_ov004_020bb2ac;
OamAttr *sMgMsgRetrySpritesEnglish = data_ov004_020bb59c;
OamAttr *data_ov004_020b9e9c = data_ov004_020bac5c;
OamAttr *data_ov004_020b9ea0 = data_ov004_020bae54;
OamAttr *sMgMsgGoalSpritesEnglish = data_ov004_020ba134;
OamAttr *sMgMsgHateSpritesEnglish = data_ov004_020ba174;
OamAttr *data_ov004_020b9eac = data_ov004_020ba154;
OamAttr *data_ov004_020b9eb0 = data_ov004_020bb85c;
OamAttr *data_ov004_020b9eb4 = data_ov004_020bb90c;
OamAttr *sMgMsgPerfectSpritesEnglish = data_ov004_020babec;
OamAttr *sMgMsgWhereSpritesEnglish = data_ov004_020bbc44;
OamAttr *data_ov004_020b9ec0 = data_ov004_020ba274;
OamAttr *data_ov004_020b9ec4 = data_ov004_020bbb7c;
OamAttr *data_ov004_020b9ec8 = data_ov004_020bb964;
OamAttr *data_ov004_020b9ecc = data_ov004_020ba52c;
OamAttr *data_ov004_020b9ed0 = data_ov004_020baf6c;
OamAttr *sMgMsgTooBadSpritesEnglish = data_ov004_020ba824;
OamAttr *data_ov004_020b9ed8 = data_ov004_020bb0ac;
OamAttr *data_ov004_020b9edc = data_ov004_020ba7f4;
OamAttr *data_ov004_020b9ee0 = data_ov004_020bb22c;
OamAttr *sMgMsgLikeSpritesEnglish = data_ov004_020bab44;
OamAttr *sMgMsgFiveCardSpritesEnglish = data_ov004_020ba464;
OamAttr *data_ov004_020b9eec = data_ov004_020bb36c;
OamAttr *sMgMsgLoveSpritesEnglish = data_ov004_020ba1d4;
OamAttr *data_ov004_020b9ef4 = data_ov004_020bb32c;
OamAttr *data_ov004_020b9ef8 = data_ov004_020ba094;
OamAttr *sMgMsgTwoPairSpritesEnglish = data_ov004_020ba764;
OamAttr *data_ov004_020b9f00 = data_ov004_020ba9d4;
OamAttr *data_ov004_020b9f04 = data_ov004_020ba504;
OamAttr *data_ov004_020b9f08 = data_ov004_020bb47c;
OamAttr *sMgMsgEsperSpritesEnglish = data_ov004_020ba48c;
OamAttr *data_ov004_020b9f10 = data_ov004_020ba0f4;
OamAttr *data_ov004_020b9f14 = data_ov004_020baa34;
OamAttr *data_ov004_020b9f18 = data_ov004_020bafec;
OamAttr *data_ov004_020b9f1c = data_ov004_020ba644;
OamAttr *data_ov004_020b9f20 = data_ov004_020baa64;
OamAttr *data_ov004_020b9f24 = data_ov004_020bad74;
OamAttr *data_ov004_020b9f28 = data_ov004_020bbac4;
OamAttr *sMgMsgLevelClearSpritesEnglish = data_ov004_020ba4b4;
OamAttr *data_ov004_020b9f30 = data_ov004_020bbdec;
OamAttr *data_ov004_020b9f34 = data_ov004_020bad04;
OamAttr *data_ov004_020b9f38 = data_ov004_020bab0c;
OamAttr *data_ov004_020b9f3c = data_ov004_020bade4;
OamAttr *data_ov004_020b9f40 = data_ov004_020baa9c;
OamAttr *data_ov004_020b9f44 = data_ov004_020ba9a4;
OamAttr *data_ov004_020b9f48 = data_ov004_020ba114;
OamAttr *data_ov004_020b9f4c = data_ov004_020ba674;
OamAttr *data_ov004_020b9f50 = data_ov004_020bbe5c;
OamAttr *sMgMsgGiveUpSprites = data_ov004_020ba7c4;
OamAttr *data_ov004_020b9f58 = data_ov004_020bb62c;
OamAttr *sMgMsgResultHiScoreSpritesEnglish = data_ov004_020bb434;
OamAttr *sMgMsgOnePairSpritesEnglish = data_ov004_020ba43c;
OamAttr *data_ov004_020b9f64 = data_ov004_020bac24;
OamAttr *data_ov004_020b9f68 = data_ov004_020bbecc;
OamAttr *data_ov004_020b9f6c = data_ov004_020bbcac;
OamAttr *data_ov004_020b9f70 = data_ov004_020baad4;
OamAttr *data_ov004_020b9f74 = data_ov004_020bab7c;
OamAttr *sMgMsgStartSpritesEnglish = data_ov004_020ba414;
OamAttr *sMgMsgDislikeSpritesEnglish = data_ov004_020bb70c;
OamAttr *data_ov004_020b9f80 = data_ov004_020ba0d4;
OamAttr *data_ov004_020b9f84 = data_ov004_020ba6a4;
OamAttr *sMgMsgThreeCardSpritesEnglish = data_ov004_020bb02c;
OamAttr *data_ov004_020b9f8c = data_ov004_020bb12c;
OamAttr *data_ov004_020b9f90 = data_ov004_020bb1ec;
OamAttr *data_ov004_020b9f94 = data_ov004_020bb0ec;
OamAttr *data_ov004_020b9f98 = data_ov004_020bbd14;
OamAttr *data_ov004_020b9f9c = data_ov004_020ba394;
OamAttr *sMgMsgLevelSpritesEnglish = data_ov004_020ba4dc;
OamAttr *data_ov004_020b9fa4 = data_ov004_020bb2ec;
OamAttr *data_ov004_020b9fa8 = data_ov004_020bb6bc;
OamAttr *data_ov004_020b9fac = data_ov004_020bbbdc;
OamAttr *data_ov004_020b9fb0 = data_ov004_020ba194;
OamAttr *data_ov004_020b9fb4 = data_ov004_020baa04;
OamAttr *data_ov004_020b9fb8 = data_ov004_020bb8b4;
OamAttr *data_ov004_020b9fbc = data_ov004_020ba034;
OamAttr *data_ov004_020b9fc0 = data_ov004_020ba794;
OamAttr *data_ov004_020b9fc4 = data_ov004_020ba054;
OamAttr *sMgMsgFourCardSpritesEnglish = data_ov004_020bb06c;
OamAttr *data_ov004_020b9fcc = data_ov004_020ba1b4;
OamAttr *data_ov004_020b9fd0 = data_ov004_020ba61c;
OamAttr *data_ov004_020b9fd4 = data_ov004_020bbd7c;
OamAttr *sMgMsgHiScoreSpritesEnglish = data_ov004_020bb674;
OamAttr *data_ov004_020b9fdc = data_ov004_020baf34;
OamAttr *data_ov004_020b9fe0 = data_ov004_020bae1c;
OamAttr *data_ov004_020b9fe4 = data_ov004_020ba374;
OamAttr *data_ov004_020b9fe8 = data_ov004_020ba314;
OamAttr *data_ov004_020b9fec = data_ov004_020bafac;
OamAttr *data_ov004_020b9ff0 = data_ov004_020ba294;
OamAttr *data_ov004_020b9ff4 = data_ov004_020ba3f4;
OamAttr *data_ov004_020b9ff8 = data_ov004_020bb7ac;
OamAttr *sMgMsgDrawSpritesEnglish = data_ov004_020ba354;
OamAttr *data_ov004_020ba000 = data_ov004_020ba6d4;
OamAttr *data_ov004_020ba004 = data_ov004_020ba3b4;
OamAttr *data_ov004_020ba008 = data_ov004_020ba0b4;
OamAttr *data_ov004_020ba00c = data_ov004_020baefc;
OamAttr *sMgMsgGameOverSpritesEnglish = data_ov004_020bb3ac;
OamAttr *data_ov004_020ba014 = data_ov004_020baec4;
OamAttr *data_ov004_020ba018 = data_ov004_020bba6c;
OamAttr *data_ov004_020ba01c = data_ov004_020ba334;
OamAttr *data_ov004_020ba020 = data_ov004_020bb804;
OamAttr *data_ov004_020ba024 = data_ov004_020bb3ec;
OamAttr *data_ov004_020ba028 = data_ov004_020bb26c;
OamAttr *data_ov004_020ba02c = data_ov004_020bb4c4;
OamAttr *data_ov004_020ba030 = data_ov004_020ba5f4;
}
static EntryTable sMgBannerFileTemplate = {{{0, 0}, {0x140, &sMgMsgGiveUpSprites}}};
extern OamAttr *data_ov006_021346bc;
extern int sMgTopScreenYOffset;
extern int sMgBannerButtonsArmed;
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
extern OamAttr *sMgBannerSprites[];

OamAttr **sMgMsgWhereSpritesByLanguage[] = {&sMgMsgWhereSpritesEnglish, &data_ov004_020b9f68,
                                            &data_ov004_020b9f28, &data_ov004_020b9ec8,
                                            &data_ov004_020b9e64};
OamAttr **sMgMsgLikeSpritesByLanguage[] = {&sMgMsgLikeSpritesEnglish, &data_ov004_020b9e34,
                                           &data_ov004_020b9ec0, &data_ov004_020b9e28,
                                           &data_ov004_020b9e10};
OamAttr **sMgMsgLoveSpritesByLanguage[] = {&sMgMsgLoveSpritesEnglish, &data_ov004_020ba004,
                                           &data_ov004_020b9fe8, &data_ov004_020ba01c,
                                           &data_ov004_020b9fe4};
OamAttr **sMgMsgDislikeSpritesByLanguage[] = {&sMgMsgDislikeSpritesEnglish, &data_ov004_020b9e74,
                                              &data_ov004_020b9ff0, &data_ov004_020b9f9c,
                                              &data_ov004_020ba008};
OamAttr **sMgMsgEsperSpritesByLanguage[] = {&sMgMsgEsperSpritesEnglish, &data_ov004_020b9fa8,
                                            &data_ov004_020b9f14, &data_ov004_020b9ff8,
                                            &data_ov004_020ba020};
OamAttr **sMgMsgSuperEsperSpritesByLanguage[] = {&sMgMsgSuperEsperSpritesEnglish,
                                                 &data_ov004_020b9e4c, &data_ov004_020ba018,
                                                 &data_ov004_020b9fd4, &data_ov004_020b9fb8};
OamAttr **sMgMsgGoalSpritesByLanguage[] = {&sMgMsgGoalSpritesEnglish, &data_ov004_020b9f10,
                                           &data_ov004_020b9f70, &data_ov004_020b9f80,
                                           &data_ov004_020b9fc0};
int sMgMsgOnePairNcgByLanguage[] = {273, 274, 270, 271, 272};
int sMgMsgTwoPairNcgByLanguage[] = {278, 279, 275, 276, 277};
int sMgMsgFourCardNcgByLanguage[] = {288, 289, 285, 286, 287};
OamAttr **sMgMsgHateSpritesByLanguage[] = {&sMgMsgHateSpritesEnglish, &data_ov004_020b9fcc,
                                           &data_ov004_020b9f48, &data_ov004_020b9eac,
                                           &data_ov004_020b9fb0};
int sMgMsgFiveCardNcgByLanguage[] = {293, 294, 290, 291, 292};
int sMgMsgDrawNcgByLanguage[] = {303, 304, 300, 301, 302};
int sMgMsgNoPairNcgByLanguage[] = {359, 360, 356, 357, 358};
OamAttr **sMgMsgTimeOverSpritesByLanguage[] = {&sMgMsgTimeOverSpritesEnglish, &data_ov004_020ba028,
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
OamAttr **sMgMsgNoPairSpritesByLanguage[] = {&sMgMsgNoPairSpritesEnglish, &data_ov004_020b9fc4,
                                             &data_ov004_020b9ff4, &data_ov004_020b9f4c,
                                             &data_ov004_020b9eb0};
OamAttr **sMgMsgOnePairSpritesByLanguage[] = {&sMgMsgOnePairSpritesEnglish, &data_ov004_020b9e40,
                                              &data_ov004_020b9e58, &data_ov004_020b9e80,
                                              &data_ov004_020b9e3c};
OamAttr **sMgMsgTwoPairSpritesByLanguage[] = {&sMgMsgTwoPairSpritesEnglish, &data_ov004_020b9e24,
                                              &data_ov004_020b9e44, &data_ov004_020b9e08,
                                              &data_ov004_020b9e20};
OamAttr **sMgMsgThreeCardSpritesByLanguage[] = {&sMgMsgThreeCardSpritesEnglish,
                                                &data_ov004_020b9e60, &data_ov004_020b9e00,
                                                &data_ov004_020b9f90, &data_ov004_020b9e5c};
OamAttr **sMgMsgFourCardSpritesByLanguage[] = {&sMgMsgFourCardSpritesEnglish, &data_ov004_020b9e14,
                                               &data_ov004_020b9e2c, &data_ov004_020b9ee0,
                                               &data_ov004_020b9e30};
OamAttr **sMgMsgFiveCardSpritesByLanguage[] = {&sMgMsgFiveCardSpritesEnglish, &data_ov004_020ba014,
                                               &data_ov004_020ba030, &data_ov004_020b9fd0,
                                               &data_ov004_020b9ea0};
int sMgMsgHateNcgByLanguage[] = {329, 330, 326, 327, 328};
OamAttr **sMgMsgDrawSpritesByLanguage[] = {&sMgMsgDrawSpritesEnglish, &data_ov004_020b9e0c,
                                           &data_ov004_020b9fe0, &data_ov004_020b9f6c,
                                           &data_ov004_020b9f44};
OamAttr **sMgMsgFullHouseSpritesByLanguage[] = {&sMgMsgFullHouseSpritesEnglish,
                                                &data_ov004_020b9ef8, &data_ov004_020b9fbc,
                                                &data_ov004_020ba02c, &data_ov004_020b9e04};
OamAttr **sMgMsgGameOverSpritesByLanguage[] = {&sMgMsgGameOverSpritesEnglish, &data_ov004_020b9eec,
                                               &data_ov004_020b9e94, &data_ov004_020b9fa4,
                                               &data_ov004_020b9ef4};
int sMgMsgTimeOverNcgByLanguage[] = {394, 395, 391, 392, 393};
OamAttr **sMgMsgLevelClearSpritesByLanguage[] = {&sMgMsgLevelClearSpritesEnglish,
                                                 &data_ov004_020b9e54, &data_ov004_020b9e38,
                                                 &data_ov004_020b9e18, &data_ov004_020b9e1c};
OamAttr **sMgMsgHiScoreSpritesByLanguage[] = {&sMgMsgHiScoreSpritesEnglish, &data_ov004_020b9fb4,
                                              &data_ov004_020b9f98, &data_ov004_020b9f58,
                                              &data_ov004_020b9f00};
OamAttr **sMgMsgPerfectSpritesByLanguage[] = {&sMgMsgPerfectSpritesEnglish, &data_ov004_020b9f8c,
                                              &data_ov004_020b9f38, &data_ov004_020b9ed8,
                                              &data_ov004_020b9f94};
int sMgMsgStartNcgByLanguage[] = {384, 385, 381, 382, 383};
OamAttr **sMgMsgResultHiScoreSpritesByLanguage[] = {&sMgMsgResultHiScoreSpritesEnglish,
                                                    &data_ov004_020b9f20, &data_ov004_020b9fac,
                                                    &data_ov004_020ba024, &data_ov004_020b9f1c};
OamAttr **sMgMsgQuitSpritesByLanguage[] = {&sMgMsgQuitSpritesEnglish, &data_ov004_020b9e48,
                                           &data_ov004_020b9f74, &data_ov004_020b9f64,
                                           &data_ov004_020b9e84};
OamAttr **sMgMsgRetrySpritesByLanguage[] = {&sMgMsgRetrySpritesEnglish, &data_ov004_020ba00c,
                                            &data_ov004_020b9f34, &data_ov004_020b9f24,
                                            &data_ov004_020b9f3c};
OamAttr **sMgMsgStartSpritesByLanguage[] = {&sMgMsgStartSpritesEnglish, &data_ov004_020b9fec,
                                            &data_ov004_020b9f08, &data_ov004_020b9ed0,
                                            &data_ov004_020b9f84};
OamAttr **sMgMsgLevelSpritesByLanguage[] = {&sMgMsgLevelSpritesEnglish, &data_ov004_020b9ecc,
                                            &data_ov004_020b9edc, &data_ov004_020b9f04,
                                            &data_ov004_020b9e9c};
OamAttr **sMgMsgTooBadSpritesByLanguage[] = {&sMgMsgTooBadSpritesEnglish, &data_ov004_020b9e6c,
                                             &data_ov004_020b9fdc, &data_ov004_020b9e88,
                                             &data_ov004_020b9e68};
OamAttr **sMgMsgYouWinSpritesByLanguage[] = {&sMgMsgYouWinSpritesEnglish, &data_ov004_020b9f40,
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
    e->DrawPlain();
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
        e->mOamCount = Mg_CopySpriteList((int)sMgBannerSprites[e->mFileIndex], e->mOam,
                                         charNo.v[e->mGfxSlot], e->mFileIndex);
    }

    if (sBannerSetupTable[e->mStyle])
        (e->*sBannerSetupTable[e->mStyle])();

    e->mBehavior = sBannerUpdateTable[e->mStyle];
    e->mRender = sBannerDrawTable[e->mStyle];
}

void MgBanner::DrawLevelNumber() {
    int x;
    int originX = mX;
    int n;
    /* the first OAM record's 9-bit signed x, made positive */
    int oamX = ((GXOamAttr *)mOam)->x;
    int w;
    unsigned long left;
    n = Mg_GetLevel() + 1;
    if (oamX > 0x100) {
        oamX -= 0x200;
    }
    if (oamX < 0) {
        oamX = -oamX;
    }
    x = mX + oamX + 0x10;
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
    Mg_RenderSpriteOn((OamAttr *)mOam, left, mY, mScreen, mPriority);
    Mg_DrawBigNumber(x, mY, n, mScreen, mPriority, 2, 0);
}

void MgBanner::SetupNone() {}

void MgBanner::DrawSpin() {
    GXOamAttr *oam = (GXOamAttr *)mOam;
    int angle = (u16)mTimer;
    Matrix2x2 m;
    u16 flag;
    do {
        int mm = FX_MUL(FX_SinIdx(angle >> 4), 0x800);
        int cos1 = FX_CosIdx((u16)mm >> 4);
        int sin1 = FX_SinIdx((u16)mm >> 4);
        int fc = FX_MUL(cos1, 0x1000);
        int fs = FX_MUL(sin1, 0x1000);
        m.m00 = fc;
        m.m11 = fc;
        m.m01 = fs;
        m.m10 = -fs;
        Mg_RenderSpriteOnMtx(oam, mX, mY, mScreen, mPriority, &m);
        angle = (u16)(angle + 0x4000);
        flag = oam->attr3;
        oam++;
    } while (flag != 0xffff);
}

void MgBanner::UpdateSpin() {
    if (ApproachLinear(mTimer, mTimerEnd, 0x1000)) {
        mTimer = 0;
    }
}

void MgBanner::StartSpin() {
    mTimer = 0;
    mTimerEnd = 0x27000;
    mBehavior = &MgBanner::UpdateSpin;
    mRender = &MgBanner::DrawSpin;
}

void MgBanner::UpdateDrop() {
    if (mTimer < 48) {
        mTimer++;
        return;
    }
    if (ApproachLinear2(mY, mTargetY, 16)) {
        StartSpin();
    }
}

void MgBanner::SetupDrop() {
    mTimer = 0;
    mY = -(sMgTopScreenYOffset + 96);
}

void MgBanner::DrawWave() {
    GXOamAttr *oam = (GXOamAttr *)mOam;
    int x = mTimer >> 2;
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
            Matrix2x2 m = {0};
            m.m00 = v;
            m.m11 = v;
            Mg_RenderSpriteOnMtx(oam, mX, mY, mScreen, mPriority, &m);
        }
        t += 0x1000;
        flag = oam->attr3;
        oam++;
    } while (flag != 0xffff);
}

void MgBanner::UpdateWave() {
    if (ApproachLinear(mTimer, mTimerEnd, 0x1000)) {
        mTimer = 0;
    }
}

void MgBanner::StartWaveBurst() {
    int i;

    mTimer = 0;
    mTimerEnd = (mOamCount + 1) << 14;

    mBehavior = &MgBanner::UpdateWave;
    mRender = &MgBanner::DrawWave;

    Vector2 burstVel(0, -0x2000);
    Vector2 liftVel(0, -0x1800);
    Vector2 ringOffset(0, -0x4000);

    Vector2 center(mX << 12, mY << 12);
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

        Mg_SpawnFx(0, RAND_FX_SCALE(60) + 90, &spawnPos, &spawnVel, &sBannerWaveFxGravity);
    }
}

void MgBanner::DrawSqueeze() {
    int m, t, v;
    unsigned short term;
    if (mTimer == 0)
        return;
    m = FX_MUL(mTimer, 0xcc);
    {
        GXOamAttr *oam = (GXOamAttr *)mOam;
        t = FX_MUL(FX_Div(m, 0x1000), 0x1000);
        v = FX_Inv(m);
        {
            Matrix2x2 buf = {0};
            buf.m00 = v;
            buf.m11 = v;
            t = 0x1000 - t;
            do {
                s16 x = oam->x;
                if (x > 0x100)
                    x -= 0x200;
                x = x * t >> 12;
                Mg_RenderSpriteOnMtx(oam, mX - x, mY, mScreen, mPriority, &buf);
                term = oam->attr3;
                oam++;
            } while (term != 0xffff);
        }
    }
}

void MgBanner::UpdateFall() {
    ApproachLinear(mTimer, mTimerEnd, 0x1000);
    if (ApproachLinear2(mY, mTargetY, 0x10) == 0)
        return;
    StartWaveBurst();
}

void MgBanner::SetupFall() {
    mTimer = 0;
    mTimerEnd = 0x14000;
    mY = 96;
}

void MgBanner::DrawPopSpinA() {
    u16 flag;
    if (mTimer == 0)
        return;
    {
        GXOamAttr *oam = (GXOamAttr *)mOam;
        if (mTimer < 0x18000) {
            int v = 0x2000;
            if (mTimer >= 0x14000) {
                v = 0x1000;
            } else {
                int t = FX_Div(0x1000, 0x18000);
                v -= FX_MUL(mTimer, t);
                v = FX_Inv(v);
            }
            {
                Matrix2x2 m = {0};
                m.m00 = v;
                m.m11 = v;
                do {
                    Mg_RenderSpriteOnMtx(oam, mX, mY, mScreen, mPriority, &m);
                    flag = oam->attr3;
                    oam++;
                } while (flag != 0xffff);
            }
        } else {
            int angle = (u16)(mTimer - 0x18000);
            Matrix2x2 m2;
            do {
                int mm = FX_MUL(FX_SinIdx(angle >> 4), 0x800);
                int cos1 = FX_CosIdx((u16)mm >> 4);
                int sin1 = FX_SinIdx((u16)mm >> 4);
                int fc = FX_MUL(cos1, 0x1000);
                int fs = FX_MUL(sin1, 0x1000);
                m2.m00 = fc;
                m2.m11 = fc;
                m2.m01 = fs;
                m2.m10 = -fs;
                Mg_RenderSpriteOnMtx(oam, mX, mY, mScreen, mPriority, &m2);
                angle = (u16)(angle + 0x4000);
                flag = oam->attr3;
                oam++;
            } while (flag != 0xffff);
        }
    }
}

void MgBanner::DrawPopSpinB() {
    u16 flag;
    if (mTimer == 0)
        return;
    {
        GXOamAttr *oam = (GXOamAttr *)mOam;
        if (mTimer < 0x18000) {
            int v = 0x2000;
            if (mTimer >= 0x14000) {
                v = 0x1000;
            } else {
                int t = FX_Div(0x1000, 0x18000);
                v -= FX_MUL(mTimer, t);
                v = FX_Inv(v);
            }
            {
                Matrix2x2 m = {0};
                m.m00 = v;
                m.m11 = v;
                do {
                    Mg_RenderSpriteOnMtx(oam, mX, mY, mScreen, mPriority, &m);
                    flag = oam->attr3;
                    oam++;
                } while (flag != 0xffff);
            }
        } else {
            int angle = (u16)(mTimer - 0x18000);
            Matrix2x2 m2;
            do {
                int mm = FX_MUL(FX_SinIdx(angle >> 4), 0x800);
                int cos1 = FX_CosIdx((u16)mm >> 4);
                int sin1 = FX_SinIdx((u16)mm >> 4);
                int fc = FX_MUL(cos1, 0x1000);
                int fs = FX_MUL(sin1, 0x1000);
                m2.m00 = fc;
                m2.m11 = fc;
                m2.m01 = fs;
                m2.m10 = -fs;
                Mg_RenderSpriteOnMtx(oam, mX, mY, mScreen, mPriority, &m2);
                angle = (u16)(angle + 0x4000);
                flag = oam->attr3;
                oam++;
            } while (flag != 0xffff);
        }
    }
}

void MgBanner::UpdatePopSpin() {
    if (ApproachLinear(mTimer, mTimerEnd, 0x1000)) {
        mTimer = 0x18000;
    }
}

void MgBanner::SetupPopSpin() {
    mTimer = 0;
    mTimerEnd = 0x27000;
}

int Banner_IsPressFinished(MgBanner *e) {
    if (e->mBehavior == &MgBanner::UpdatePressed && e->mTimer == e->mTimerEnd)
        return 1;
    return 0;
}

void MgBanner::DrawPressed() {
    GXOamAttr *oam = (GXOamAttr *)mOam;
    int x = mTimer;
    u16 ang = (unsigned int)x >> 1;
    fx16 s = FX_SinIdx(ang >> 4);
    int v = FX_Inv(FX_MUL(s, 0x800) + 0x1000);
    Matrix2x2 m = {0};
    u16 flag;
    m.m00 = v;
    m.m11 = v;
    do {
        Mg_RenderSpriteOnMtx(oam, mX, mY, mScreen, mPriority, &m);
        flag = oam->attr3;
        oam++;
    } while (flag != 0xffff);

    x = mFileIndex;
    if (x != 0 && x != 1 && x != 2)
        return;
    Mg_RenderSpriteOn(data_ov006_021346bc, mX, mY, mScreen, mPriority);
}

void MgBanner::UpdatePressed() {
    ApproachLinear(mTimer, mTimerEnd, 0x1000);
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
    e->mBehavior = &MgBanner::UpdatePressed;
    e->mRender = &MgBanner::DrawPressed;
}

void MgBanner::DrawPlain() {
    Mg_RenderSpriteOn((OamAttr *)mOam, mX, mY, mScreen, mPriority);
    if (mFileIndex != 0 && mFileIndex != 1 && mFileIndex != 2)
        return;
    Mg_RenderSpriteOn(data_ov006_021346bc, mX, mY, mScreen, mPriority);
}

void MgBanner::SetupZoom() {
    mTimer = 0;
    mTimerEnd = ((mOamCount + 1) << 2) + 0x1e << 0xc;
}

void MgBanner::DrawZoomA() {
    u16 flag;
    int val = mTimer;
    if (val == 0) return;
    {
        GXOamAttr *oam = (GXOamAttr *)mOam;
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
                Matrix2x2 m = {0};
                m.m00 = d;
                m.m11 = d;
                scale = 0x1000 - scale;
                do {
                    s16 x = oam->x;
                    if (x > 0x100)
                        x -= 0x200;
                    x = x * scale >> 12;
                    Mg_RenderSpriteOnMtx(oam, mX - x, mY, mScreen, mPriority, &m);
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
                    Matrix2x2 m2 = {0};
                    m2.m00 = r;
                    m2.m11 = r;
                    Mg_RenderSpriteOnMtx(oam, mX, mY, mScreen, mPriority, &m2);
                }
                i += 0x1000;
                flag = oam->attr3;
                oam++;
            } while (flag != 0xffff);
        }
    }
}

void MgBanner::DrawZoomB() {
    u16 flag;
    int val = mTimer;
    if (val == 0) return;
    {
        GXOamAttr *oam = (GXOamAttr *)mOam;
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
                Matrix2x2 m = {0};
                m.m00 = d;
                m.m11 = d;
                scale = 0x1000 - scale;
                do {
                    s16 x = oam->x;
                    if (x > 0x100)
                        x -= 0x200;
                    x = x * scale >> 12;
                    Mg_RenderSpriteOnMtx(oam, mX - x, mY, mScreen, mPriority, &m);
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
                    Matrix2x2 m2 = {0};
                    m2.m00 = r;
                    m2.m11 = r;
                    Mg_RenderSpriteOnMtx(oam, mX, mY, mScreen, mPriority, &m2);
                }
                i += 0x1000;
                flag = oam->attr3;
                oam++;
            } while (flag != 0xffff);
        }
    }
}

void MgBanner::UpdateZoom() {
    if (ApproachLinear(mTimer, mTimerEnd, 0x1000)) {
        mTimer = 0x1e000;
    }
}

void MgBanner::SetupZoomBurst() {
    int i;

    mTimer = 0;
    mTimerEnd = (((mOamCount + 1) << 2) + 0x1e) << 0xc;

    Vector2 burstVel(0, -0x2000);
    Vector2 liftVel(0, -0x1800);
    Vector2 ringOffset(0, -0x4000);

    Vector2 center(mX << 12, mY << 12);
    Vector2 spawnPos, spawnVel;

    static Vector2 sBannerZoomFxGravity(0, 0xc0);

    for (i = 0; i < 32; i++) {
        func_0203d704((int *)&spawnPos, (int *)&center, (int *)&ringOffset);
        func_0203d388((int *)&burstVel, 0x800);
        func_0203d388((int *)&ringOffset, 0x800);
        func_0203d704((int *)&spawnVel, (int *)&liftVel, (int *)&burstVel);

        Mg_SpawnFx(0, RAND_FX_SCALE(60) + 90, &spawnPos, &spawnVel, &sBannerZoomFxGravity);
    }
}

void MgBanner::UpdateSlideFromRight() {
    if (ApproachLinear2(mX, mTargetX, 0x10))
        mArrived = 1;
}

void MgBanner::SetupSlideFromRight() {
    mX = 320;
}

void MgBanner::UpdateSlideFromLeft() {
    if (ApproachLinear2(mX, mTargetX, 0x10))
        mArrived = 1;
}

void MgBanner::SetupSlideFromLeft() {
    mX = -0x40;
}

void MgBanner::UpdateSlideFromBottom() {
    ApproachLinear2(mY, mTargetY, 4);
}

void MgBanner::SetupSlideFromBottom() {
    mY = 200;
}

void MgBanner::UpdateSlideFromTop() {
    ApproachLinear2(mY, mTargetY, 4);
}

void MgBanner::SetupSlideFromTop() {
    mY = -8;
}

MgBanner::MgBanner() {
    mFileIndex = MG_BANNER_FILE_NUM;
}

/* __sinit_ov004_020b955c sources: the null member pointer the three
   per-style tables copy (setup sBannerSetupTable, behavior
   sBannerUpdateTable, render sBannerDrawTable), and the words the
   29-entry file-id table copies. */
extern "C" {
extern ElemFn data_02086b58; /* __ptmf_null's address: the null member pointer */
}

namespace s20b955c {
extern "C" {
MgBanner gMgBanners[3];
}
}

extern "C" {
OamAttr *sMgBannerSprites[MG_BANNER_FILE_NUM] = {
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
                                &MgBanner::SetupSlideFromTop,
                                &MgBanner::SetupSlideFromBottom,
                                &MgBanner::SetupSlideFromLeft,
                                &MgBanner::SetupSlideFromRight,
                                &MgBanner::SetupZoomBurst,
                                &MgBanner::SetupPopSpin,
                                &MgBanner::SetupZoom,
                                &MgBanner::SetupZoomBurst,
                                &MgBanner::SetupPopSpin,
                                &MgBanner::SetupFall,
                                &MgBanner::SetupDrop,
                                &MgBanner::SetupNone};
ElemFn sBannerUpdateTable[13] = {data_02086b58,
                                 &MgBanner::UpdateSlideFromTop,
                                 &MgBanner::UpdateSlideFromBottom,
                                 &MgBanner::UpdateSlideFromLeft,
                                 &MgBanner::UpdateSlideFromRight,
                                 &MgBanner::UpdateZoom,
                                 &MgBanner::UpdatePopSpin,
                                 &MgBanner::UpdateZoom,
                                 &MgBanner::UpdateZoom,
                                 &MgBanner::UpdatePopSpin,
                                 &MgBanner::UpdateFall,
                                 &MgBanner::UpdateDrop,
                                 data_02086b58};
ElemFn sBannerDrawTable[13] = {data_02086b58,
                               data_02086b58,
                               data_02086b58,
                               data_02086b58,
                               data_02086b58,
                               &MgBanner::DrawZoomB,
                               &MgBanner::DrawPopSpinB,
                               &MgBanner::DrawZoomB,
                               &MgBanner::DrawZoomA,
                               &MgBanner::DrawPopSpinA,
                               &MgBanner::DrawSqueeze,
                               data_02086b58,
                               &MgBanner::DrawLevelNumber};
void *sMgBannerFiles[MG_BANNER_FILE_NUM];
}
