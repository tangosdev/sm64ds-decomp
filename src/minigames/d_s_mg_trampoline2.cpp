//cpp
#include "dFdBrightness_c.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_light.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_load.h"
#include "math/Approach.h"
#include "nitro/fx/fx.h"
#include "nitro/gx/g3x.h"
#include "nitro/fx/fx_cp.h"
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_trampoline2_AssetId {
    ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NCG_BIN = 0x00a2,
    ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NCL_BIN = 0x00a3,
    ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NSC_BIN = 0x00a4,
    ASSET_D_2D_MG_TRAMPOLINE_NCL_BIN = 0x0108,
    ASSET_MINI_STAR_BMD = 0x0219
};

#pragma opt_loop_invariants off
/* dScMgTrampoline2_c -- Trampoline Terror. Draw lines on the touch screen
 * so Mario can bounce through the shapes that drop in.
 *
 * Functions are in reverse ROM order; do not reorder.
 * opt_loop_invariants is off for the whole file. Measured 2026-11-06:
 * PaintSquare is the only function in the TU whose bytes move when
 * invariants are hoisted, and the setting still cannot be narrowed to it --
 * this TU leaves codegen deferred, so mwccarm reads the state at the end of
 * the file and an `on` placed after PaintSquare (or after the function
 * following it) puts hoisting back for PaintSquare as well. TUBUILD CONFLICT
 * lines stay; the manifest pairs them.
 *
 * leftovers:
 * - 0x7acc has no matched read.
 * - NewCleanParticle takes Fix12 by value, which costs the caller bytes
 *   (notes/mwccarm-codegen.md 6az).
 * - The 0x78-element pool helpers stay free functions over char *; they
 *   read the record through Trmp2Spinner.h's view. Their
 *   pointer locals (Init's p1..p3, Update's p), StatePlay's level pointer
 *   and its unread raw hold the pinned @1110: folding any of them into a
 *   direct member expression renumbers it (measured).
 * - TrmpItem's +0x1a8 array (five Vector2s, mUnusedVec) has no known role.
 */

#include "common.h"
#include "types.h"
#include "dScMgTrampoline2_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "TrmpItem.h"
#include "TrmpLine.h"
#include "Trmp2Spinner.h"
#include "decl_common.h"
#include "dMgTrmplnShared.h"
#include "PlayerInput.h"
#include "Sound.h"
#include "math/MtxStack.h"
#include "LevelRng.h"
#include "nitro/mi/memory.h"
#include "math/CalcLib.h"
using ns_020caadc::Trmp2Mario_DrawAll;
using ns_020cac9c::Trmp2Mario_ResetAll;

/* local: the ROM re-reads the slot byte at each use here, so keep the read volatile. */
#define ACTIVE_SLOT (*(volatile unsigned char *)&gActivePlayerSlot)
/* local: these readers only reproduce through a 4-byte-record view of the lane. */
#define TOUCH_REC(lane) ((unsigned char (*)[4])(lane))

/* The forward declarations below are local stand-ins for types that have
 * no header yet (issue #2497). */

/* The cartridge's ctor/dtor imports are the functions the wrapper's aliases
 * name. */
struct Trampoline2ModelFilePtr : SharedFilePtr {
    Trampoline2ModelFilePtr(u32 fileID);
    ~Trampoline2ModelFilePtr();
};

struct BMD_File;

struct Vector3_16f;


struct Tbl {
    int a, b, c;
};

typedef unsigned char u8;

typedef unsigned short u16;

/* A scripted wave supplies one position and three parameters per item.
 * Positions are Q12; angles rotate the item's forward vector in the XY plane. */
struct Trampoline2WaveDef {
    int count;
    const VecFx32 *positions;
    const short *angles;
    const short *itemTypes;
    const short *motionTypes;
};

/* TUBUILD CONFLICT -- alternate body of struct 'dExtSimpleModel_c', from the legacy file for Trmp2Spinner_Destruct, NOT applied:
struct dExtSimpleModel_c { ~dExtSimpleModel_c(); };
*/

void Trmp2Spinner_UpdateMatrix(char *raw);

extern Trampoline2ModelFilePtr sTrmp2StarModelFile;

extern int sTrmp2SpinnerCount;

extern unsigned char *sTrmp2Spinners;
void Trmp2Spinner_Update(char *raw);
void Trmp2Spinner_Init(void *r0, int value);
void Trmp2Spinner_SetModel(void *raw);

extern int sTrmp2BrushWord[];

extern const u16 sTrmp2BrushPltt[];

extern dFdBrightness_c *gSceneFader;

extern int sTrmp2TouchBlocked;
namespace s20ad90c {
void Mg_CommitRecords(void);
}
using s20ad90c::Mg_CommitRecords;
namespace s20ad674 {
int GetGameLanguage(void);
}
using s20ad674::GetGameLanguage;
extern "C" {

void func_0203cd80(int *m, short angle);
}

struct OamAttr;

struct Matrix2x2;
namespace s20afcf8 {
void DrawOamSprite(OamAttr *, int, int, Matrix2x2 *);
}
using s20afcf8::DrawOamSprite;
namespace s20afa20 {
void RenderOamMainScreen(OamAttr *, int, int, int, int);
}
using s20afa20::RenderOamMainScreen;
extern "C" {

unsigned int d_s_mg_trampoline2NewCleanParticleHold(unsigned int a, unsigned int b, int c, int d,
                                                    int e, const Vector3_16f *f);
}
void Jump2Popup_UpdateAll(void);
/* TUBUILD CONFLICT -- alternate declaration of Trmp2Spinner_UpdateMatrix, from the legacy file for Trmp2Spinner_UpdateMatricesAll, NOT applied: extern void Trmp2Spinner_UpdateMatrix(char *obj); */
/* TUBUILD CONFLICT -- alternate declaration of sTrmp2Spinners, from the legacy file for Trmp2Spinner_HideAll, NOT applied: extern int sTrmp2Spinners; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmp2Spinners, from the legacy file for Trmp2Spinner_InitAll, NOT applied: extern unsigned char *sTrmp2Spinners; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmp2Spinners, from the legacy file for Trmp2Spinner_LoadAll, NOT applied: extern int sTrmp2Spinners; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmp2StarModelFile, from the legacy file for Trmp2Spinner_Destruct, NOT applied: extern SharedFilePtr sTrmp2StarModelFile; */
/* TUBUILD CONFLICT -- alternate declaration of MIi_CpuClear16, from the legacy file for _ZN18dScMgTrampoline2_c16OnGraphCallback0Ev, NOT applied: void MI_CpuFill16(char* dst, u16 val, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for Trmp2_PickFaceA, NOT applied: extern int RandomIntInternal(void* seed); */
/* TUBUILD CONFLICT -- alternate declaration of MIi_CpuClear16, from the legacy file for func_ov006_02123b24, NOT applied: extern void MI_CpuFill16(int b, unsigned short a, int c); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov006_02123bf4, NOT applied: extern int _Z14ApproachLinearRiii(int &v, int a, int b); */
/* Both callers use the existing ov006 helpers' void(void) contracts. The
 * unrelated daPly_c::St_Null_Init at the same RAM address belongs to ov002. */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov006_02123cb4, NOT applied: extern int _Z14ApproachLinearRiii(int *v, int step, int rate); */
/* TUBUILD CONFLICT -- alternate declaration of func_0203cc28, from the legacy file for func_ov006_02123cb4, NOT applied: extern void func_0203cc28(int *p, int angle); */
/* TUBUILD CONFLICT -- alternate declaration of TrmpItem_Spawn, from the legacy file for func_ov006_02123cb4, NOT applied: extern char *TrmpItem_Spawn(int a0, int *a1, int a2, int a3); */
/* TUBUILD CONFLICT -- alternate declaration of MIi_CpuClear16, from the legacy file for func_ov006_02124088, NOT applied: extern void MI_CpuFill16(char *dst, u16 val, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012718, from the legacy file for func_ov006_02124088, NOT applied: extern void func_02012718(void *a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012468, from the legacy file for func_ov006_02124088, NOT applied: extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h); */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpStartTouchY, from the legacy file for func_ov006_02124228, NOT applied: extern short sTrmpStartTouchY; */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN18dScMgTrampoline2_c11CanEndStateEi, NOT applied: extern int GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN18dScMgTrampoline2_c10StartRoundEi, NOT applied: extern "C" s32 GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of MIi_CpuClear16, from the legacy file for _ZN18dScMgTrampoline2_c10StartRoundEi, NOT applied: extern "C" void MI_CpuFill16(char *dst, u16 val, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for func_ov006_021245a8, NOT applied: extern s32 GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of MIi_CpuClear16, from the legacy file for func_ov006_021245a8, NOT applied: extern void MI_CpuFill16(char *dst, u16 val, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2GX10LoadBGPlttEPKvjj, from the legacy file for func_ov006_021245a8, NOT applied: extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b); */

