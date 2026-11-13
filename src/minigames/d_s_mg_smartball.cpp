//cpp
/* Option tilemap, loaded by id through LoadCompressedFileAt. */
enum dScMgSmartball_c_AssetId { ASSET_D_2D_MG_BG_OPTION_NSC_BIN = 0x005b };

#pragma defer_codegen off
/* Smartball: the minigame scene and the pinball-board objects it spawns --
 * balls, mushrooms, holes, propellers, pipes, piranhas, push switches, the
 * spring, the slot machine, the scoreboard and the wing.
 *
 * ROM evidence: _ZTS 0x0213ec7c, _ZTI 0x0213ec28, _ZTV 0x0213eefc; the
 * destructor is the key function, D1 at 0x0210d740 and D0 at 0x0210d7e0.
 * defer_codegen stays off so the destructor is emitted at its definition.
 *
 * SaveSnapshot and RestoreInitial are coined names for slots 0 and 2.
 * Comments marked "Leftover" name a spelling the cartridge's bytes require. */

// Leftovers kept for byte-exactness; each has a note at its site:
// - several slot-0 overrides repeat the base's SaveSnapshot body inline;
// - the object tables, popups, dust and score arrays are walked by stepping
//   `this` and reading element 0, because indexing changes the addressing.

#include "dScMgSmartball_c.h"
#include "nitro/hw/registers.h"
#include "nitro/os/cache.h"
#include "cMgSmartball_wing_c.h"
#include "OamAttr.h"
#include "OAM.h"
#include "cMgSmartball_pushswitch_c.h"
#include "cMgSmartball_board_c.h"
#include "types.h"
#include "cMgSmartball_slot_c.h"
#include "nitro/mi/memory.h"
#include "cMgSmartball_pakkun_c.h"
#include "cMgSmartball_dokan_c.h"
#include "cMgSmartball_propeller_c.h"
#include "cMgSmartball_spring_c.h"
#include "cMgSmartball_ana_c.h"
#include "cMgSmartball_kinoko_c.h"
#include "cMgSmartball_ball_c.h"
#include "cMgSmartball_object_c.h"
namespace s20b1ea4 { void Mg_DrawNumber(int, int, int, int, int, int, int); }
using s20b1ea4::Mg_DrawNumber;
namespace s20b1a5c { void Mg_DrawTopRightCounter(int, int); }
using s20b1a5c::Mg_DrawTopRightCounter;
namespace s20b19f0 { void Mg_DrawTopLeftCounter(int); }
using s20b19f0::Mg_DrawTopLeftCounter;
namespace s20b0d8c { void Mg_DrawBlinkSprite(void *c, s32, s32); }
using s20b0d8c::Mg_DrawBlinkSprite;
namespace s20b0cac { void Mg_ShowBanner(int, int, int, int, int, short); }
using s20b0cac::Mg_ShowBanner;
namespace s20b0a54 { void Mg_SetSharedState(s32); }
using s20b0a54::Mg_SetSharedState;
namespace s20b0aa0 { void FreeGfxSlotsById(int); }
using s20b0aa0::FreeGfxSlotsById;
namespace s20b04d0 { void Mg_SetTopScreenYOffset(int); }
using s20b04d0::Mg_SetTopScreenYOffset;
namespace s20b0104 { void RenderOamBothScreens(void *attr, int, int, int, int, void *mtx); }
using s20b0104::RenderOamBothScreens;
namespace s20afdd0 { void Mg_RenderSpriteBoth(void *a0, int, int, int, int); }
using s20afdd0::Mg_RenderSpriteBoth;
namespace s20ae274 { void Mg_PlayJingle(void*); }
using s20ae274::Mg_PlayJingle;
namespace s20ae20c { void Mg_RestoreMusic(void); }
using s20ae20c::Mg_RestoreMusic;
namespace s20adc74 { void *Ov004_LoadFileByPath(const char *path); }
using s20adc74::Ov004_LoadFileByPath;
namespace s20adc5c { void Ov004_Deallocate(void *ptr); }
using s20adc5c::Ov004_Deallocate;
namespace s20adb1c { void Mg_SubmitScore(int); }
using s20adb1c::Mg_SubmitScore;
namespace s20adc1c { int Mg_GetTopRecord(void); }
using s20adc1c::Mg_GetTopRecord;
namespace s20adbc0 { int Mg_GetSubmittedScore(void); }
using s20adbc0::Mg_GetSubmittedScore;
namespace s20ad674 { s32 GetGameLanguage(void); }
using s20ad674::GetGameLanguage;
#include "dScMgBase_c.h"
#include "private/disp_obj_vtbl.h"
#include "private/ov006_smartball_pair.h"
#include "PlayerInput.h"
#include "Sound.h"
#include "Memory.h"
#include "nitro/fx/fx.h"
#include "nitro/gx/gx_load.h"
#include "nitro/os/printf.h"
#include "cstd.h"
#include "Vector2Math.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"
#include "math/CalcLib.h"
#include "LevelRng.h"

/* Two-word pair that Vec2_Len also accepts as a Vector2. */
typedef struct {
    int x, y;
} Vec2;
#include "nitro/gx/gx_bgcnt.h"

/* Value types the board objects pass around; no shared header declares them. */

/* (x, y) pair: the unit the board's vector helpers work on. */
typedef struct {
    int x, y;
} V2;

/* The same two words, named x/z by the ball physics. */
typedef struct V2xz {
    int x;
    int z;
} V2xz;

/* Point and triangle corners passed to PointInTriangle. */
struct S {
    int a, b;
};

/* Nine-word copy of sSmartballCellStartOrder. */
struct W9 {
    int w[9];
};

/* OAM::Render's scale arguments. */
typedef int Fix12i;

/* The pipe's scratch block: three pairs. */
typedef struct {
    V2 d;
    V2 v;
    V2 e;
} W;

/* Two-word pair with an empty destructor; the destructor keeps the value in
 * memory. */
struct TVec2 {
    int x;
    int y;
    ~TVec2() {}
};

/* First virtual slot, as Behavior dispatches it. */
typedef void (*VFunc)(void *);

/* cMgSmartball_ball_c::UpdatePhysics's search state. */
typedef struct State2 {
    int flags[7];
    int iteration;
    int depth;
    int zeroAngle;
    int zero;
    int entityIndex;
    void *null34;
    void *null38;
    int null3c;
    int angle40;
    int index44;
    int zero48;
    int zero4c;
    int baseZ;
} State2;

extern "C" {

extern void func_0203d388(V2 *p, int angle);

extern long long func_0203d5bc(int *p);

}

extern struct OamAttr *sSmartballWingSprites[];
extern "C" {

extern void OamRender10_d_s_mg_smartball(int sub, struct OamAttr *data, s32 x, s32 y, s32 palette,
                                         s32 priority, Fix12i scaleX, Fix12i scaleY, s32 rotation,
                                         s32 mode);

}

extern int GetOwnerLanguage(void);

extern OamAttr *sSmartballLineSpritesItalian[];

extern OamAttr *sSmartballLineSpritesSpanish[];

extern OamAttr *sSmartballLineSpritesGerman[];

extern OamAttr *sSmartballLineSpritesFrench[];

extern OamAttr *sSmartballLineSprites[];

extern OamAttr *sSmartballCellSprites[];

extern s32 sSmartballCellOrder[];

extern OamAttr *sSmartballCellRowSprites[];



extern void func_02012718(int a, int b);

extern struct W9 sSmartballCellStartOrder;


extern int Sound_PlayIfNotActive(int a, int b, int c, short d);

extern void *sSmartballSlotFrameSprite;

extern void *sSmartballSlotReelEndSprite;

extern void *sSmartballSlotReelLowerSprite;

extern void *sSmartballSlotReelUpperSprite;
extern "C" {

extern void Vec2_Sub(int *o, int *a, int *b);}

extern OamAttr *sSmartballDokanSprites[];

extern OamAttr *sSmartballAnaSprites[];


extern int func_020126e8(int a);

extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);extern "C" {

extern int func_0203d570(V2 *a, V2 *b);

extern int func_0203d434(V2 *p);

extern void func_0203d630(V2 *p, int scale);}

extern void func_0203d480(Fix12i *out, Fix12i *in);extern "C" {

extern int func_0203d524(V2 *a, V2 *b);

extern int func_0203d4d0(void *p, int *a, int *b);}

extern OamAttr *sSmartballSparkSpritesAlt[];

extern OamAttr *sSmartballSparkSprites[];
extern "C" {





extern s16 atan2_hold_d_s_mg_smartball_1(int y, int x);



extern void DecompressLZ16(void *src, void *dst);



extern void __cxa_vec_ctor(void *, unsigned int, unsigned int, ctor_t, dtor_t);

extern void SetSubBg0Offset(int x, int y);

extern void func_0203d738(void);}

extern char sSmartballPathBuf[];

extern const char sSmartballLayoutNcgFmtSpanish[];

extern const char sSmartballLayoutNcgFmtItalian[];

extern const char sSmartballLayoutNcgFmtGerman[];

extern const char sSmartballLayoutNcgFmtFrench[];

extern const char sSmartballLayoutNcgFmtE[];

extern const char sSmartballLayoutNcgFmt[];

extern const char sSmartballBg1NscFmt[];

extern const char sSmartballBg2NscFmt[];

extern int sSmartballLay0Bg1FrameNscPath[];

extern int sSmartballLay0Bg2FrameNscPath[];

extern int sSmartballLay0Bg1NscPath[];

extern int sSmartballLay0Bg2NscPath[];

extern int sSmartballLay1Bg1FrameNscPath[];

extern int sSmartballLay1Bg2FrameNscPath[];

extern int sSmartballLay1Bg1NscPath[];

extern int sSmartballLay1Bg2NscPath[];
extern "C" {






}

extern OamAttr *data_ov006_02138ae0[];

extern char *sSmartballTextSpritesByLanguage[];

extern int sSmartballCellScores[];

extern OamAttr *data_ov006_02137a78[];

extern OamAttr *data_ov006_02137a6c[];

extern OamAttr *sSmartballTimerSprite[];

extern OamAttr *sSmartballTimerSpriteAlt[];

extern OamAttr *sSmartballMarkerSprites[];

extern OamAttr *data_ov006_02137a60[];

extern OamAttr *sSmartballSideSprites[];

extern OamAttr *sSmartballSideSpritesAlt[];
extern "C" {



extern unsigned int LoadCompressedFileAt(int fileID, void *target);}


extern unsigned char gSubScreenLayers[];

extern u8 gMainScreenLayers;

extern s32 data_0208ee44;


extern void *sSmartballResultBgNcgPath;

extern void *sSmartballResultBgNscPath;

extern void *sSmartballResultNclPath;

extern void *sSmartballLay0Bg1FrameNcgPath;

extern void *sSmartballLay0Bg2FrameNcgPath;

extern void *sSmartballLay0Bg1NcgPath;

extern void *sSmartballLay0Bg2NcgPath;

extern void *sSmartballLay1Bg1FrameNcgPath;

extern void *sSmartballLay1Bg2FrameNcgPath;

extern void *sSmartballLay1Bg1NcgPath;

extern void *sSmartballLay1Bg2NcgPath;

extern void *sSmartballLay0Bg1NclPath;

extern void *sSmartballLay0Bg2NclPath;

extern void *sSmartballLay1Bg1NclPath;

extern void *sSmartballLay1Bg2NclPath;

extern void *sSmartballNclPath;

extern OamAttr *sSmartballBallSprites[];

extern OamAttr *sSmartballBallSpritesAlt[];

extern OamAttr *sSmartballPushSwitchSprites[];

extern OamAttr *sSmartballPropellerSprites[];

extern OamAttr *sSmartballKinokoSprites[];

extern OamAttr *sSmartballKinokoTurnSprites[];

extern OamAttr *sSmartballSpringSprites[];

extern OamAttr *sSmartballSpringSpritesAlt[];

extern OamAttr *sSmartballPakkunSprites[];

extern OamAttr *sSmartballWingBodySprites[];


/* local extern: no header declares these vtable symbols. The symbol is the
   slot array itself, so a vptr store is [0], not the compiler's _ZTV+8. */
extern VFunc cMgSmartball_pushswitch_c_vtbl[];
extern VFunc cMgSmartball_board_c_vtbl[];
extern VFunc cMgSmartball_pakkun_c_vtbl[];
extern VFunc cMgSmartball_dokan_c_vtbl[];
extern VFunc cMgSmartball_propeller_c_vtbl[];
extern VFunc cMgSmartball_spring_c_vtbl[];
extern VFunc cMgSmartball_ana_c_vtbl[];
extern VFunc cMgSmartball_ball_c_vtbl[];
extern VFunc cMgSmartball_object_c_vtbl[];
extern VFunc cMgSmartball_kinoko_c_vtbl[];
extern VFunc cMgSmartball_slot_c_vtbl[];
extern VFunc cMgSmartball_wing_c_vtbl[];
/* Helpers, some called before their definition. */
cMgSmartball_wing_c *cMgSmartball_wing_c_Construct(cMgSmartball_wing_c *self, dScMgSmartball_c *x, int y, V2 *src);
cMgSmartball_kinoko_c *cMgSmartball_kinoko_c_Construct(cMgSmartball_kinoko_c *self, dScMgSmartball_c *x, int y, V2 *src);
cMgSmartball_pushswitch_c *cMgSmartball_pushswitch_c_Construct(cMgSmartball_pushswitch_c *c, dScMgSmartball_c *b, int d, V2 *s);
cMgSmartball_board_c *cMgSmartball_board_c_Construct(cMgSmartball_board_c *c, dScMgSmartball_c *b, int d, V2 *s);
cMgSmartball_pakkun_c *cMgSmartball_pakkun_c_Construct(cMgSmartball_pakkun_c *c, dScMgSmartball_c *b, int d, V2 *s);
cMgSmartball_dokan_c *cMgSmartball_dokan_c_Construct(cMgSmartball_dokan_c *c, dScMgSmartball_c *b, int d, V2 *s);
cMgSmartball_propeller_c *cMgSmartball_propeller_c_Construct(cMgSmartball_propeller_c *c, dScMgSmartball_c *b, int d, V2 *s);
cMgSmartball_spring_c *cMgSmartball_spring_c_Construct(cMgSmartball_spring_c *c, dScMgSmartball_c *b, int d, V2 *s);
cMgSmartball_ana_c *cMgSmartball_ana_c_Construct(cMgSmartball_ana_c *c, dScMgSmartball_c *b, int d, V2 *s);
cMgSmartball_ball_c *cMgSmartball_ball_c_Construct(cMgSmartball_ball_c *c, dScMgSmartball_c *b, int d, V2 *s);
void CopyObjectVel(void *a, void *b);
void CopyObjectPos(void *a, void *b);
cMgSmartball_slot_c *cMgSmartball_slot_c_Construct(cMgSmartball_slot_c *c, dScMgSmartball_c *a1, int a2, V2 *src);


/* Declarations kept out of the common header: three of its parameter views
   clash with definitions here, which makes an illegal overload if included. */


extern int sMgResultsBannerY;

extern int sMgResultsBannerX;

extern int sSmartballOptionNcgByLanguage[];
extern "C" {

extern void SetSubBg1Offset(int, int);


}


void GetObjectDelta(void *a, cMgSmartball_object_c *b);
cMgSmartball_object_c *cMgSmartball_object_c_Construct(cMgSmartball_object_c *o, dScMgSmartball_c *a1, int a2, int *a3);

static inline void CallObjectConstruct(cMgSmartball_object_c *c, dScMgSmartball_c *a1, int a2,
                                       int *a3) {
    cMgSmartball_object_c_Construct(c, a1, a2, a3);
}

#define LP(e) ((int *)(e))

/* Byte offset of a member, as a number. mwccarm has no offsetof here, and the
   cartridge needs these offsets as values rather than as member accesses: see
   SlotAddress below. */
#define OFFSET_OF(type, member) ((size_t)&((type *)0)->member)

/* The shared draw routine takes the sprite-handle word at +0x54 of each
   language's text-sprite record. */
static inline void *TextSpriteGfx(char *sprite) {
    return *(void **)(sprite + 0x54);
}

/* Address of one entry of one of the manager's per-slot arrays. The cartridge
   forms the address as an integer -- object + index*stride, then the member
   offset through a register -- so naming the member on the object instead
   folds the offset into the load and moves bytes. */
static inline void *SlotAddress(dScMgSmartball_c *object, size_t offset, int index, int stride) {
    return (void *)((intptr_t)object + index * stride + (intptr_t)offset);
}

/* One entry of a pointer table: mpBalls and its siblings. */
static inline void *TableEntry(dScMgSmartball_c *object, size_t offset, int index) {
    return *(void **)SlotAddress(object, offset, index, 4);
}

/* Fixed-point multiplies, signed/unsigned pair. Leftover: FIX_MUL_SU's inner
   (s64) is required. */
#define FIX_MUL_SU(a, b) \
    ((int)(((s64)((u64)(s64)(a) * (b)) + 0x800) >> 12))
#define FIX_MUL_US(a, b) \
    ((int)(((s64)((a) * (u64)(b)) + 0x800) >> 12))

/* Both destructor variants come from this one out-of-line body, which tears
   the four arrays down in reverse construction order. */
dScMgSmartball_c::~dScMgSmartball_c() {
    __cxa_vec_cleanup(mSparks, 0x40, 0x24, (dtor_t)SmartballObj_ElemDtor);
    __cxa_vec_cleanup(mDustVel, 0x10, 8, (dtor_t)NullDestructor_0203d47c);
    __cxa_vec_cleanup(mDustPos, 0x10, 8, (dtor_t)NullDestructor_0203d47c);
    __cxa_vec_cleanup(mPopupPos, 5, 8, (dtor_t)NullDestructor_0203d47c);
}
 /* C linkage, as in the original .c file */
void SmartballObj_ElemDtor(void) {}


int cMgSmartball_wing_c::IsSettled() {
    if (mTriggerCount == 0)
        return 1;
    return mAngle == 0x3000;
}

void cMgSmartball_wing_c::Trigger() {
    mTriggerCount += 1;
    if (mTriggerCount == 1) {
        mTriggerTime = 0x14;
        mTriggerFlag = 1;
        mTriggerPhase = 0;
        Sound::PlayBank2_2D(0x1a1);
        return;
    }
    mTriggerTime = 0;
    mTriggerFlag = 1;
    mTriggerPhase = 0;
    Sound::PlayBank2_2D(0x1a2);
}

void cMgSmartball_wing_c::RebuildVerts() {
    mVertices[0].x = -0x4000;
    mVertices[0].y = 0;
    mVertices[1].x = 0x4000;
    mVertices[1].y = 0;
    mVertices[2].x = -0x4000;
    mVertices[2].y = -0x18000;
    mVertices[3].x = 0x4000;
    mVertices[3].y = -0x18000;
    mVertices[4].x = -0x4000;
    mVertices[4].y = 0;
    mVertices[5].x = 0x4000;
    mVertices[5].y = 0;
    mVertices[6].x = -0x4000;
    mVertices[6].y = -0x18000;
    mVertices[7].x = 0x4000;
    mVertices[7].y = -0x18000;

    func_0203d388((V2 *)&mVertices[0], (s16)-mAngle);
    func_0203d388((V2 *)&mVertices[1], (s16)-mAngle);
    func_0203d388((V2 *)&mVertices[2], (s16)-mAngle);
    func_0203d388((V2 *)&mVertices[3], (s16)-mAngle);
    func_0203d388((V2 *)&mVertices[4], mAngle);
    func_0203d388((V2 *)&mVertices[5], mAngle);
    func_0203d388((V2 *)&mVertices[6], mAngle);
    func_0203d388((V2 *)&mVertices[7], mAngle);

    mVertices[0].x += mCurrent0 - 0x10000;
    mVertices[0].y += mCurrent1 + 0x28000;
    mVertices[1].x += mCurrent0 - 0x10000;
    mVertices[1].y += mCurrent1 + 0x28000;
    mVertices[2].x += mCurrent0 - 0x10000;
    mVertices[2].y += mCurrent1 + 0x28000;
    mVertices[3].x += mCurrent0 - 0x10000;
    mVertices[3].y += mCurrent1 + 0x28000;
    mVertices[4].x += mCurrent0 + 0x10000;
    mVertices[4].y += mCurrent1 + 0x28000;
    mVertices[5].x += mCurrent0 + 0x10000;
    mVertices[5].y += mCurrent1 + 0x28000;
    mVertices[6].x += mCurrent0 + 0x10000;
    mVertices[6].y += mCurrent1 + 0x28000;
    mVertices[7].x += mCurrent0 + 0x10000;
    mVertices[7].y += mCurrent1 + 0x28000;
}

int cMgSmartball_wing_c::HitTest(int *point) {
    int bodyOffset[2];
    int triangle0Point[2], triangle0A[2], triangle0B[2], triangle0C[2];
    int triangle1Point[2], triangle1A[2], triangle1B[2], triangle1C[2];
    int triangle2Point[2], triangle2A[2], triangle2B[2], triangle2C[2];
    int triangle3Point[2], triangle3A[2], triangle3B[2], triangle3C[2];

    if (mTriggerCount <= 1 && point[1] < mCurrent1 + 0x8000) {
        bodyOffset[0] = point[0];
        bodyOffset[1] = point[1];
        bodyOffset[0] -= mCurrent0;
        bodyOffset[1] -= mCurrent1;
        bodyOffset[1] -= 0x8000;
        if (func_0203d5bc(bodyOffset) < 0x100000)
            return 1;
    }

    triangle0Point[0] = point[0];
    triangle0Point[1] = point[1];
    triangle0A[0] = mVertices[0].x;
    triangle0A[1] = mVertices[0].y;
    triangle0B[0] = mVertices[1].x;
    triangle0B[1] = mVertices[1].y;
    triangle0C[0] = mVertices[2].x;
    triangle0C[1] = mVertices[2].y;
    if (PointInTriangle(triangle0Point, triangle0A, triangle0B, triangle0C))
        return 1;

    triangle1Point[0] = point[0];
    triangle1Point[1] = point[1];
    triangle1A[0] = mVertices[1].x;
    triangle1A[1] = mVertices[1].y;
    triangle1B[0] = mVertices[2].x;
    triangle1B[1] = mVertices[2].y;
    triangle1C[0] = mVertices[3].x;
    triangle1C[1] = mVertices[3].y;
    if (PointInTriangle(triangle1Point, triangle1A, triangle1B, triangle1C))
        return 1;

    triangle2Point[0] = point[0];
    triangle2Point[1] = point[1];
    triangle2A[0] = mVertices[4].x;
    triangle2A[1] = mVertices[4].y;
    triangle2B[0] = mVertices[5].x;
    triangle2B[1] = mVertices[5].y;
    triangle2C[0] = mVertices[6].x;
    triangle2C[1] = mVertices[6].y;
    if (PointInTriangle(triangle2Point, triangle2A, triangle2B, triangle2C))
        return 1;

    triangle3Point[0] = point[0];
    triangle3Point[1] = point[1];
    triangle3A[0] = mVertices[5].x;
    triangle3A[1] = mVertices[5].y;
    triangle3B[0] = mVertices[6].x;
    triangle3B[1] = mVertices[6].y;
    triangle3C[0] = mVertices[7].x;
    triangle3C[1] = mVertices[7].y;
    return PointInTriangle(triangle3Point, triangle3A, triangle3B, triangle3C);
}

/* Slot 1: draws the wing's base sprites and its two mirrored blades. */
void cMgSmartball_wing_c::Update() {
    if (mTriggerCount <= 1) {
        Mg_RenderSpriteBoth(sSmartballWingBodySprites[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        Mg_RenderSpriteBoth(sSmartballWingBodySprites[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5,
                            -1, 2);
    }
    OAM::Render(0, sSmartballWingSprites[0], (mCurrent0 >> 12) - 0x10, (mCurrent1 >> 12) + 0x118,
                -1, 1, 0x1000, 0x1000, (unsigned short)(short)(-0x4000 - mAngle), -1);
    OAM::Render(0, sSmartballWingSprites[0], (mCurrent0 >> 12) + 0x10, (mCurrent1 >> 12) + 0x118,
                -1, 1, 0x1000, 0x1000, (unsigned short)(short)(mAngle - 0x4000), -1);
    OAM::Render(0, sSmartballWingSprites[1], (mCurrent0 >> 12) - 0x10, (mCurrent1 >> 12) + 0x11d,
                -1, 2, 0x1000, 0x1000, (unsigned short)(short)(-0x4000 - mAngle), -1);
    OAM::Render(0, sSmartballWingSprites[1], (mCurrent0 >> 12) + 0x10, (mCurrent1 >> 12) + 0x11d,
                -1, 2, 0x1000, 0x1000, (unsigned short)(short)(mAngle - 0x4000), -1);
}

/* Slot 0: eases the flap angle toward its 0x3000 stop and rebuilds the
 * collision vertices. */
void cMgSmartball_wing_c::SaveSnapshot() {
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mAngleSettled != 1 && mTriggerCount > 0) {
        if (mAngle < 0x3000) {
            mAngle += 0x200;
            if (mAngle >= 0x3000)
                mAngle = 0x3000;
        } else {
            mAngle -= 0x200;
            if (mAngle < 0x3000)
                mAngle = 0x3000;
        }
    }
    RebuildVerts();
}

/* Slot 2: clears the wing's trigger state and rebuilds the vertices. */
void cMgSmartball_wing_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mAngle = 0;
    mTriggerTime = 0;
    mTriggerFlag = 0;
    mTriggerPhase = 0;
    mTriggerCount = 0;
    mAngleSettled = 0;
    RebuildVerts();
}

 /* C linkage, as in the original .c file */
cMgSmartball_wing_c *cMgSmartball_wing_c_Construct(cMgSmartball_wing_c *self, dScMgSmartball_c *x,
                                                   int y, V2 *src) {
    int v[2];
    v[0] = src->x;
    v[1] = src->y;
    CallObjectConstruct(self, x, y, v);
    self->~cMgSmartball_wing_c();
    self->mRadius = 0x18000;
    char *angle = (char *)self;
    angle += 0x32;
    *(short *)angle = 0;
    return self;
}


int cMgSmartball_pushswitch_c::BallsClear() {
    int i;
    Vec2 delta, pos;
    for (i = 0; i < mpManager->mBallCount; i++) {
        int *o = (i >= 13) ? 0 : (int *)mpManager->mpBalls[i];
        if (((cMgSmartball_object_c *)o)->mIsActive != 0) {
            int *o2 = (i >= 13) ? 0 : (int *)mpManager->mpBalls[i];
            CopyObjectPos(&pos, o2);
            delta = pos;
            delta.x -= mCurrent0;
            delta.y -= mCurrent1;
            if (Vec2_Len((const Vector2 *)&delta) < 0x12000)
                return 0;
        }
    }
    return 1;
}

int cMgSmartball_pushswitch_c::CheckBall(int *p) {
    int d[2];
    d[0] = p[0];
    d[1] = p[1];
    d[0] -= mCurrent0;
    d[1] -= mCurrent1;
    if (mTriggered) {
        if (p[1] >= mCurrent1 + 0x8000)
            return 0;
        d[1] -= 0x8000;
        return func_0203d5bc(d) < 0x40000;
    }
    if (p[1] > mCurrent1) {
        if (p[0] >= mCurrent0 - 0x8000 && p[0] < mCurrent0 + 0x8000 && p[1] < mCurrent1 + 0xa000)
            return 1;
        return 0;
    }
    if (func_0203d5bc(d) >= 0x40000)
        return 0;
    if (p[0] >= mCurrent0 - 0x4000 && p[0] < mCurrent0 + 0x4000) {
        mpManager->mpSlot->StartSpin();
        mTriggered = 1;
        mReleaseDelay = 0xa;
        Sound::PlayBank2_2D(0x1a4);
    }
    return 1;
}

/* Slot 1: draws the switch and the plate under it. */
void cMgSmartball_pushswitch_c::Update() {
    RenderOamBothScreens(sSmartballPushSwitchSprites[mTriggered], mCurrent0 >> 12, mCurrent1 >> 12,
                         -1, 1, 0);
    RenderOamBothScreens(sSmartballPushSwitchSprites[2], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1,
                         2, 0);
}

/* Slot 0: counts down and releases the switch once no ball is on it. */
void cMgSmartball_pushswitch_c::SaveSnapshot() {
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mReleaseDelay > 0) {
        mReleaseDelay -= 1;
    }
    if (mTriggered != 1)
        return;
    if (BallsClear()) {
        if (mReleaseDelay == 0)
            mTriggered = 0;
    } else {
        mReleaseDelay = 0xa;
    }
}

/* Slot 2: clears the switch's trigger state. */
void cMgSmartball_pushswitch_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mTriggered = 0;
    mReleaseDelay = 0;
}

 /* C linkage, as in the original .c file */
cMgSmartball_pushswitch_c *cMgSmartball_pushswitch_c_Construct(cMgSmartball_pushswitch_c *c,
                                                               dScMgSmartball_c *b, int d, V2 *s) {
    int tmp[2];
    tmp[0] = s->x;
    tmp[1] = s->y;
    CallObjectConstruct(c, b, d, tmp);
    c->~cMgSmartball_pushswitch_c();
    c->mRadius = 0x8000;
    return c;
}


int cMgSmartball_board_c::AnyLineActive() {
    int i;
    for (i = 0; i < 8; i++) {
        if (mLineTimer[i] > 0x168)
            return 1;
    }
    return 0;
}

/* Slot 1: draws the eight scored lines with their lit cells, the nine cells
 * (a flipping one is sheared from the shared sine table) and the nine slot
 * markers. */
void cMgSmartball_board_c::Update() {
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < 8; i++) {
        if (mLineTimer[i] > 0 || (mpManager->mPhase != 0 && mLineScored[i] == 1)) {
            switch (i) {
            case 0:
            case 1:
            case 2:
                if (GetOwnerLanguage() == 5)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesSpanish[0],
                                        (i - 1) * 0x18 + (mCurrent0 >> 12), mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 4)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesItalian[0],
                                        (i - 1) * 0x18 + (mCurrent0 >> 12), mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 3)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesGerman[0],
                                        (i - 1) * 0x18 + (mCurrent0 >> 12), mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 2)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesFrench[0],
                                        (i - 1) * 0x18 + (mCurrent0 >> 12), mCurrent1 >> 12, -1, 1);
                else
                    Mg_RenderSpriteBoth(sSmartballLineSprites[0],
                                        (i - 1) * 0x18 + (mCurrent0 >> 12), mCurrent1 >> 12, -1, 1);
                mCellHighlight[i] = 1;
                mCellHighlight[i + 3] = 1;
                mCellHighlight[i + 6] = 1;
                break;
            case 3:
            case 4:
            case 5:
                if (GetOwnerLanguage() == 5)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesSpanish[1], mCurrent0 >> 12,
                                        (i - 4) * 0x18 + (mCurrent1 >> 12), -1, 1);
                else if (GetOwnerLanguage() == 4)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesItalian[1], mCurrent0 >> 12,
                                        (i - 4) * 0x18 + (mCurrent1 >> 12), -1, 1);
                else if (GetOwnerLanguage() == 3)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesGerman[1], mCurrent0 >> 12,
                                        (i - 4) * 0x18 + (mCurrent1 >> 12), -1, 1);
                else if (GetOwnerLanguage() == 2)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesFrench[1], mCurrent0 >> 12,
                                        (i - 4) * 0x18 + (mCurrent1 >> 12), -1, 1);
                else
                    Mg_RenderSpriteBoth(sSmartballLineSprites[1], mCurrent0 >> 12,
                                        (i - 4) * 0x18 + (mCurrent1 >> 12), -1, 1);
                mCellHighlight[(i - 3) * 3] = 1;
                mCellHighlight[(i - 3) * 3 + 1] = 1;
                mCellHighlight[(i - 3) * 3 + 2] = 1;
                break;
            case 6:
                if (GetOwnerLanguage() == 5)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesSpanish[2], mCurrent0 >> 12,
                                        mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 4)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesItalian[2], mCurrent0 >> 12,
                                        mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 3)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesGerman[2], mCurrent0 >> 12,
                                        mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 2)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesFrench[2], mCurrent0 >> 12,
                                        mCurrent1 >> 12, -1, 1);
                else
                    Mg_RenderSpriteBoth(sSmartballLineSprites[2], mCurrent0 >> 12, mCurrent1 >> 12,
                                        -1, 1);
                mCellHighlight[0] = 1;
                mCellHighlight[4] = 1;
                mCellHighlight[8] = 1;
                break;
            case 7:
                if (GetOwnerLanguage() == 5)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesSpanish[3], mCurrent0 >> 12,
                                        mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 4)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesItalian[3], mCurrent0 >> 12,
                                        mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 3)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesGerman[3], mCurrent0 >> 12,
                                        mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 2)
                    Mg_RenderSpriteBoth(sSmartballLineSpritesFrench[3], mCurrent0 >> 12,
                                        mCurrent1 >> 12, -1, 1);
                else
                    Mg_RenderSpriteBoth(sSmartballLineSprites[3], mCurrent0 >> 12, mCurrent1 >> 12,
                                        -1, 1);
                mCellHighlight[2] = 1;
                mCellHighlight[4] = 1;
                mCellHighlight[6] = 1;
                break;
            }
        }
    }

    for (j = 0; j < 9; j++) {
        s32 ang = mCellFlipAngle[j];
        s32 idx;

        if (ang < 0x4000)
            idx = j + 0xb;
        else if (mCellHighlight[j] == 1 && (mFanfareTimer & 0x3c) >= 0x1e)
            idx = j + 0x16;
        else
            idx = j + 2;

        if (ang == 0 || ang == 0x8000) {
            Mg_RenderSpriteBoth(sSmartballCellSprites[idx], (j % 3 - 1) * 0x18 + (mCurrent0 >> 12),
                                (j / 3 - 1) * 0x18 + (mCurrent1 >> 12), -1, 1);
        } else if ((ang >= 0x3c00 && ang <= 0x4400) || (ang >= -0x3c00 && ang <= -0x4400)) {
            Mg_RenderSpriteBoth(sSmartballCellSprites[1], (j % 3 - 1) * 0x18 + (mCurrent0 >> 12),
                                (j / 3 - 1) * 0x18 + (mCurrent1 >> 12), -1, 1);
        } else {
            s32 vec[4];

            if (ang < 0x4000)
                vec[0] = -FX_Div(0x1000, FX_SinCosTable_[((u16)(ang + 0x8000) >> 4) * 2 + 1]);
            else
                vec[0] = -FX_Div(0x1000, FX_SinCosTable_[((u16)ang >> 4) * 2 + 1]);
            vec[1] = 0;
            vec[2] = 0;
            vec[3] = 0x1000;
            RenderOamBothScreens(sSmartballCellSprites[idx], (j % 3 - 1) * 0x18 + (mCurrent0 >> 12),
                                 (j / 3 - 1) * 0x18 + (mCurrent1 >> 12), -1, 1, vec);
            RenderOamBothScreens(sSmartballCellSprites[0], (j % 3 - 1) * 0x18 + (mCurrent0 >> 12),
                                 (j / 3 - 1) * 0x18 + (mCurrent1 >> 12), -1, 1, vec);
        }
    }

    for (k = 0; k < 9; k++) {
        Mg_RenderSpriteBoth(sSmartballCellRowSprites[sSmartballCellOrder[k]], k * 0x18 + 0x10, 0x78,
                            -1, 1);
    }
}

void cMgSmartball_board_c::PaintCell(int idx, int *pos) {
    int cx;
    int cy;
    int base;
    int i;
    int j;
    int k;
    int layer;
    char *p;
    u16 tile;
    int jj;
    int b0;
    int b1;
    int kk;
    /* volatile: the ROM narrows each fill value through its own stack halfword. */
    volatile u16 t1;
    volatile u16 t2;
    volatile u16 t3;
    volatile u16 t4;
    volatile u16 t5;

    cx = (pos[0] >> 12) / 8;
    cy = (pos[1] >> 12) / 8;

    j = -1;
    k = -2;
    layer = 2;
    b1 = 0x31a;
    b0 = 0x314;

    for (i = -1; i <= 1; i++) {
        int row = (cy + i) << 6;
        jj = j;
        kk = k;
        do {
            if (idx == 0) {
                tile = (u16)(jj + b0);
            } else {
                tile = (u16)(jj + b1);
            }
            p = (char *)G2S_GetBG3ScrPtr();
            t1 = tile | 0xf000;
            MI_CpuFill16(p + row + cx * 2 + kk, t1, layer);
            kk += 2;
            jj++;
        } while (jj <= 0);
        b0 += 0x20;
        b1 += 0x20;
    }

    if (idx == 0 || mCells[sSmartballCellOrder[idx - 1]] == 1) {
        base = 0x33f;
    } else {
        base = 0x33d;
    }

    {
        int lay2 = 2;
        int i2;
        int bas2 = base;
        for (i2 = -1; i2 <= 1; i2++) {
            p = (char *)G2S_GetBG3ScrPtr() + ((cy + i2) << 6);
            t2 = (bas2 + 0x20 * i2) | 0xf000;
            MI_CpuFill16(p + cx * 2 - 4, t2, lay2);
        }
    }

    if (idx == 8) {
        int lay3 = 2;
        int i3;
        int bas3 = 0x336;
        for (i3 = -1; i3 <= 0; i3++) {
            p = (char *)G2S_GetBG3ScrPtr() + ((cy + i3) << 6);
            t3 = (bas3 + 0x20 * i3) | 0xf000;
            MI_CpuFill16(p + cx * 2 + 2, t3, lay3);
        }
        return;
    }

    if (mCells[sSmartballCellOrder[idx + 1]] == 1) {
        int lay4 = 2;
        int i4;
        int bas4 = 0x33f;
        for (i4 = -1; i4 <= 1; i4++) {
            p = (char *)G2S_GetBG3ScrPtr() + ((cy + i4) << 6);
            t4 = (bas4 + 0x20 * i4) | 0xf000;
            MI_CpuFill16(p + cx * 2 + 2, t4, lay4);
        }
        return;
    }

    {
        int lay5 = 2;
        int i5;
        int bas5 = 0x33e;
        for (i5 = -1; i5 <= 1; i5++) {
            p = (char *)G2S_GetBG3ScrPtr() + ((cy + i5) << 6);
            t5 = (bas5 + 0x20 * i5) | 0xf000;
            MI_CpuFill16(p + cx * 2 + 2, t5, lay5);
        }
    }
}

void cMgSmartball_board_c::ClaimCell(int i) {
    int count;
    int bingo;
    int m;
    int tmp;
    int slot;
    int v;
    int a[2];
    int b[2];
    int d[2];
    int e[2];

    slot = sSmartballCellOrder[i];
    if (mCells[slot] == 1)
        return;
    mCells[slot] = 1;
    v = i * 0x18000 + 0x10000;
    a[0] = v;
    a[1] = 0x78000;
    mpManager->EmitScore((int *)a, 0x64, 0, 1);
    mpManager->CountCell();
    b[0] = v;
    b[1] = 0x78000;
    PaintCell(i, (int *)b);
    Sound::PlayBank2_2D(0x173);
    count = 0;
    if (slot < 3) {
        if (mCells[slot + 3] == 1 && mCells[slot + 6] == 1) {
            mLineTimer[slot] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot] = 1;
            count += 1;
            mLinesScored++;
        }
    } else if (slot < 6) {
        if (mCells[slot - 3] == 1 && mCells[slot + 3] == 1) {
            mLineTimer[slot - 3] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot - 3] = 1;
            count += 1;
            mLinesScored++;
        }
    } else {
        if (mCells[slot - 6] == 1 && mCells[slot - 3] == 1) {
            mLineTimer[slot - 6] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot - 6] = 1;
            count += 1;
            mLinesScored++;
        }
    }
    i = slot % 3;
    if (i == 0) {
        if (mCells[slot + 1] == 1 && mCells[slot + 2] == 1) {
            mLineTimer[slot / 3 + 3] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot / 3 + 3] = 1;
            count += 1;
            mLinesScored++;
        }
    } else if (i == 1) {
        if (mCells[slot - 1] == 1 && mCells[slot + 1] == 1) {
            mLineTimer[slot / 3 + 3] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot / 3 + 3] = 1;
            count += 1;
            mLinesScored++;
        }
    } else {
        if (mCells[slot - 2] == 1 && mCells[slot - 1] == 1) {
            mLineTimer[slot / 3 + 3] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot / 3 + 3] = 1;
            count += 1;
            mLinesScored++;
        }
    }
    switch (slot) {
    case 0:
        if (mCells[4] == 1 && mCells[8] == 1) {
            mLineTimer[6] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[6] = 1;
            count += 1;
            mLinesScored++;
        }
        break;
    case 2:
        if (mCells[4] == 1 && mCells[6] == 1) {
            mLineTimer[7] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[7] = 1;
            count += 1;
            mLinesScored++;
        }
        break;
    case 4:
        if (mCells[0] == 1 && mCells[8] == 1) {
            mLineTimer[6] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[6] = 1;
            count += 1;
            mLinesScored++;
        }
        if (mCells[2] == 1 && mCells[6] == 1) {
            mLineTimer[7] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[7] = 1;
            count += 1;
            mLinesScored++;
        }
        break;
    case 6:
        if (mCells[2] == 1 && mCells[4] == 1) {
            mLineTimer[7] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[7] = 1;
            count += 1;
            mLinesScored++;
        }
        break;
    case 8:
        if (mCells[0] == 1 && mCells[4] == 1) {
            mLineTimer[6] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[6] = 1;
            count += 1;
            mLinesScored++;
        }
        break;
    }
    bingo = 1;
    {
        int j;
        for (j = 0; j < 9; j++) {
            if (j == slot)
                continue;
            if (mCells[j] == 0) {
                bingo = 0;
                break;
            }
        }
    }
    if (count <= 0)
        return;
    m = 0;
    if (count > 0) {
        tmp = sSmartballCellScores[0];
        do {
            d[0] = mCurrent0;
            d[1] = mCurrent1;
            mpManager->EmitScore((int *)d, (int)tmp, (int)0, (int)1);
            mpManager->CountLine(1);
            m++;
        } while (m < count);
    }
    if (bingo != 1)
        return;
    e[0] = mCurrent0;
    e[1] = mCurrent1;
    mpManager->EmitScore((int *)e, 0x2710, 0x3c, 1);
    mpManager->CountBingo();
    mpManager->EndRound();
    Sound::PlayBank2_2D(0x174);
}

/* Slot 0: spends the fanfare timer and replays its jingle, ages the eight
 * line timers, claims the cell the next ball in the drop strip lands on, and
 * spins claimed cells up to their 0x8000 stop. */
static inline cMgSmartball_ball_c *GetObjBoard(dScMgSmartball_c *g, int i) {
    return i >= 13 ? 0 : g->mpBalls[i];
}

static inline void SetV2(V2 *p, int x, int y) {
    p->x = x;
    p->y = y;
}

/* Leftover: this form reproduces the body; other spellings come out longer. */
void cMgSmartball_board_c::SaveSnapshot() {
    int i;
    int n;
    int j;
    int lo;
    int hi;
    V2 v;
    V2 t;
    V2 a;
    V2 b;
    V2 d;

    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mFanfareTimer > 0) {
        mFanfareTimer--;
        if (mFanfareTimer > 0 && mFanfareTimer % 60 == 0) {
            switch (mLinesScored) {
            case 0:
            case 1:
                Sound::PlayBank2_2D(0x176);
                break;
            case 2:
                Sound::PlayBank2_2D(0x177);
                break;
            case 3:
                Sound::PlayBank2_2D(0x178);
                break;
            case 4:
            default:
                Sound::PlayBank2_2D(0x179);
                break;
            }
        }
    }
    for (i = 0; i < 8; i++) {
        if (mLineTimer[i] > 0)
            mLineTimer[i]--;
    }
    for (n = 0; n < mpManager->mBallCount; n++) {
        if (mpManager->mDraining != 0)
            break;
        if (GetObjBoard(mpManager, n)->mIsActive == 0)
            continue;
        CopyObjectPos(&t, GetObjBoard(mpManager, n));
        /* Leftover: casting the destination keeps this a memory-to-memory copy. */
        *(V2 *)&v = t;
        if (v.y < 0x74000)
            continue;
        if (v.y >= 0x7c000)
            continue;
        for (j = 0, lo = 0xc000, hi = 0x14000; j < 9; j++) {
            int slot = sSmartballCellOrder[j];
            if (v.x >= lo && v.x < hi) {
                if (GetObjBoard(mpManager, n)->ArmExpire(0x14)) {
                    if (mCells[slot] == 0) {
                        SetV2(&a, mCurrent0 + ((sSmartballCellOrder[j] % 3 - 1) * 0x18 << 12),
                              mCurrent1 + ((sSmartballCellOrder[j] / 3 - 1) * 0x18 << 12));
                        mpManager->EmitSparks(&a.x, 0);
                        SetV2(&b, (j * 0x18 + 0x10) << 12, 0x78000);
                        mpManager->EmitSparks(&b.x, 0);
                        func_02012718(0x1be, j * 0x18000 + 0x10000);
                    } else {
                        SetV2(&d, (j * 0x18 + 0x10) << 12, 0x78000);
                        mpManager->EmitSparks(&d.x, 1);
                        func_02012718(0x17a, j * 0x18000 + 0x10000);
                    }
                    ClaimCell(j);
                    break;
                }
            }
            lo += 0x18000;
            hi += 0x18000;
        }
    }
    for (i = 0; i < 9; i++) {
        if (mCells[i] == 1) {
            mCellFlipAngle[i] += 0x400;
            if (mCellFlipAngle[i] >= 0x8000)
                mCellFlipAngle[i] = 0x8000;
        }
    }
}

/* Slot 2: resets the board, reshuffles the cell order and clears all state. */
void cMgSmartball_board_c::RestoreInitial() {
    int i;
    cMgSmartball_object_c::RestoreInitial();
    for (i = 0; i < 9; i++)
        sSmartballCellOrder[i] = i;
    ShuffleCells();
    for (i = 0; i < 9; i++) {
        mCells[i] = 0;
        mCellFlipAngle[i] = 0;
        mCellHighlight[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        mLineTimer[i] = 0;
        mLineScored[i] = 0;
    }
    mFanfareTimer = 0;
    mLinesScored = 0;
}

void cMgSmartball_board_c::ShuffleCells() {
    struct W9 tmp;
    int i;
    tmp = sSmartballCellStartOrder;
    for (i = 0; i < 9; i++) {
        sSmartballCellOrder[i] = tmp.w[i] - 1;
    }
}

 /* C linkage, as in the original .c file */
cMgSmartball_board_c *cMgSmartball_board_c_Construct(cMgSmartball_board_c *c, dScMgSmartball_c *b,
                                                     int d, V2 *s) {
    int tmp[2];
    tmp[0] = s->x;
    tmp[1] = s->y;
    CallObjectConstruct(c, b, d, tmp);
    c->~cMgSmartball_board_c();
    c->mRadius = 0x18000;
    return c;
}


int cMgSmartball_slot_c::ReelsStopped() {
    int i;
    for (i = 0; i < 3; i++) {
        if (mReelVel[i].y > 0)
            return 0;
    }
    return 1;
}

void cMgSmartball_slot_c::BeginStop() {
    /* Leftover: the stop counts are stored through a copy of `this`. */
    cMgSmartball_slot_c *c = this;
    int i;
    unsigned char z;
    int *seed;
    if ((mIsSpinning) != 1) {
        return;
    }
    if ((mIsSpeedingUp) != 0) {
        return;
    }
    mIsSlowingDown = 1;
    z = 0;
    for (i = 0; i < 3; i++) {
        mReelStopping[i] = z;
        c->mReelStopCount[i] =
            ((((((unsigned)RandomIntInternal(&data_0209d4b8.value)) >> 16) & 0x7fff) * 3) >> 15) +
            3;
    }
}

void cMgSmartball_slot_c::StartSpin() {
    if (mIsSpinning != 0) {
        ++mSpinsQueued;
        return;
    }
    mIsSpeedingUp = 1;
    mIsSpinning = 1;
    mSpinTimer = 0xb4;
    Sound::PlayBank2_2D(0x160);
}

void cMgSmartball_slot_c::UpdateSpin() {
    int ib, ic;

    if (mIsSpinning == 1) {
        if (mIsSpeedingUp == 1) {
            int flag = 1;
            int ia;
            for (ia = 0; ia < 3; ia++) {
                int r = RandomIntInternal(&data_0209d4b8.value);
                mReelVel[ia].y += ((((u32)r >> 16) & 0x7fff) * 0x20 >> 15) * 0x10;
                if (mReelVel[ia].y >= 0x3000)
                    mReelVel[ia].y = 0x3000;
                else
                    flag = 0;
            }
            if (flag == 1)
                mIsSpeedingUp = 0;
        }
        if (mIsSlowingDown == 1) {
            for (ib = 0; ib < 3; ib++) {
                int v = mReelStopCount[ib];
                if (v > 0) {
                    if (v <= 2) {
                        mReelVel[ib].y -= 0x10;
                        if (mReelVel[ib].y < 0x200)
                            mReelVel[ib].y = 0x200;
                    } else {
                        int r = RandomIntInternal(&data_0209d4b8.value);
                        mReelVel[ib].y -= ((((u32)r >> 16) & 0x7fff) * 0x40 >> 15) + 0x32;
                        if (mReelVel[ib].y <= 0x800) {
                            mReelStopping[ib] = 1;
                            mReelVel[ib].y = 0x800;
                        }
                    }
                }
            }
        }
    }
    for (ic = 0; ic < 3; ic++) {
        int old = mReelOffset[ic].y;
        mReelOffset[ic].y += mReelVel[ic].y;
        if (mReelStopping[ic] == 1 && mIsSlowingDown == 1) {
            if (old / 0x10000 != mReelOffset[ic].y / 0x10000) {
                if (mReelStopCount[ic] > 0) {
                    mReelStopCount[ic] -= 1;
                    if (mReelStopCount[ic] == 0) {
                        mReelVel[ic].y = 0;
                        mReelOffset[ic].y = (mReelOffset[ic].y / 0x10000) << 16;
                        Sound::PlayBank2_2D(0x19e);
                    }
                }
            }
        }
        if (mReelOffset[ic].y >= 0x30000)
            mReelOffset[ic].y -= 0x30000;
    }
    if (ReelsStopped() == 0) {
        mSoundHandle = Sound_PlayIfNotActive(mSoundHandle, 2, 0x1a6, 0);
    } else {
        mSoundHandle = 0;
    }
    if (mIsSlowingDown != 1)
        return;
    if (mIsSpinning != 1)
        return;
    if (ReelsStopped() != 1)
        return;
    mIsSlowingDown = 0;
    mIsSpinning = 0;
    {
        int first = mReelOffset[0].y;
        int eq = 1;
        int j;
        for (j = 1; j < 3; j++) {
            if (first != mReelOffset[j].y) {
                eq = 0;
                break;
            }
        }
        if (eq == 1) {
            switch (first) {
            case 0: {
                V2 a;
                a.x = mCurrent0;
                a.y = mCurrent1;
                mpManager->EmitScore(&a.x, 0x1388, 0, 1);

                mpManager->CountAward(0);

                break;
            }
            case 0x10000: {
                V2 b;
                b.x = mCurrent0;
                b.y = mCurrent1;
                mpManager->EmitScore(&b.x, 0x1f40, 0, 1);

                mpManager->CountAward(1);

                break;
            }
            case 0x20000: {
                V2 d;
                d.x = mCurrent0;
                d.y = mCurrent1;
                mpManager->EmitScore(&d.x, 0x2710, 0, 1);

                mpManager->CountAward(2);

                break;
            }
            }
            Sound::PlayBank2_2D(0x1a7);
        }
    }
    mSpinCooldown = 0x3c;
}

/* Slot 1: draws the reel window and the three reels.
 * Leftover: the empty `if (1)`, the x mask and the goto loop are required. */
inline int inline_fn(int arg0, int arg1) {
    return arg0 + arg1;
}

void cMgSmartball_slot_c::Update() {
    int i;
    int x;
    int xo;
    for (i = 0, x = 0; i < 3; i++, x += 0x18) {
        if (1) {
        }
        Mg_RenderSpriteBoth(sSmartballSlotFrameSprite, inline_fn(mCurrent0 >> 12, x - 0x18),
                            mCurrent1 >> 12, -1, 2);
    }

    i = 0;
    x = 0;
second_loop: {
    xo = (x - 0x18) & 0xFFFFFFFFFFFFFFFF;
    Mg_RenderSpriteBoth(sSmartballSlotReelEndSprite,
                        inline_fn(inline_fn(mCurrent0 >> 12, mReelOffset[i].x >> 12), xo),
                        inline_fn(mCurrent1 >> 12, mReelOffset[i].y >> 12), -1, 3);
    Mg_RenderSpriteBoth(sSmartballSlotReelLowerSprite,
                        inline_fn(inline_fn(mCurrent0 >> 12, mReelOffset[i].x >> 12), xo),
                        inline_fn(mCurrent1 >> 12, mReelOffset[i].y >> 12) - 0x10, -1, 3);
    Mg_RenderSpriteBoth(sSmartballSlotReelUpperSprite,
                        inline_fn(inline_fn(mCurrent0 >> 12, mReelOffset[i].x >> 12), xo),
                        inline_fn(mCurrent1 >> 12, mReelOffset[i].y >> 12) - 0x20, -1, 3);
    Mg_RenderSpriteBoth(sSmartballSlotReelEndSprite,
                        inline_fn(inline_fn(mCurrent0 >> 12, mReelOffset[i].x >> 12), xo),
                        inline_fn(mCurrent1 >> 12, mReelOffset[i].y >> 12) - 0x30, -1, 3);
    x += 0x18;
    i++;
    if (i < 3)
        goto second_loop;
}
}

/* Slot 0: runs the spin cooldown, the queued spins and the stop timer, then
 * steps the reels. */
void cMgSmartball_slot_c::SaveSnapshot() {
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mSpinCooldown > 0) {
        mSpinCooldown -= 1;
    }
    if (mIsSpinning == 0 && mSpinCooldown <= 0 && mSpinsQueued > 0) {
        mSpinsQueued -= 1;
        StartSpin();
    }
    if (mSpinTimer > 0) {
        mSpinTimer -= 1;
        if (mSpinTimer == 0)
            BeginStop();
    }
    UpdateSpin();
}

/* Slot 2: clears the reels, timers and sound handle. */
void cMgSmartball_slot_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    for (int i = 0; i < 3; i++) {
        mReelOffset[i].x = 0;
        mReelOffset[i].y = 0;
        mReelVel[i].x = 0;
        mReelVel[i].y = 0;
        mReelStopCount[i] = 0;
        mReelStopping[i] = 0;
    }
    mIsSpinning = 0;
    mIsSpeedingUp = 0;
    mIsSlowingDown = 0;
    mSpinsQueued = 0;
    mSpinCooldown = 0;
    mSpinTimer = 0;
    mSoundHandle = 0;
}


cMgSmartball_slot_c *cMgSmartball_slot_c_Construct(cMgSmartball_slot_c *c, dScMgSmartball_c *a1,
                                                   int a2, V2 *src) {
    int tmp[2];
    tmp[0] = src->x;
    tmp[1] = src->y;
    CallObjectConstruct(c, a1, a2, tmp);
    c->~cMgSmartball_slot_c();
    __cxa_vec_ctor(c->mReelOffset, 3, 8, (ctor_t)func_0203d738, (dtor_t)NullDestructor_0203d47c);
    __cxa_vec_ctor(c->mReelVel, 3, 8, (ctor_t)func_0203d738, (dtor_t)NullDestructor_0203d47c);
    c->mRadius = 0x28000;
    return c;
}


/* Ball-vs-piranha test: the closed jaws, or the ring around the head while
 * biting. */
int cMgSmartball_pakkun_c::CheckBall(int *pos) {
    struct S *v = (struct S *)pos;
    struct S d;
    struct S p, t1, t2, t3;
    struct S p2, t4, t5, t6;
    int t;
    int r;
    int yv, xv;
    int yv2, xv2;

    if (mIsActive == 0)
        return 0;
    t = v->a;
    d.a = t;
    d.b = v->b;
    d.a -= mCurrent0;
    d.b -= mCurrent1;
    if (mActionTimer <= 0) {
        t = mCurrent1;
        if (v->b >= t) {
            if (func_0203d5bc(&d.a) >= 0x190000)
                return 0;
            return func_0203d5bc(&d.a) >= 0xc4000;
        }
        if (v->a >= mCurrent0 - 0x14000 && v->a < mCurrent0 + 0x14000 && v->b >= t - 0xc000) {
            if (v->a >= mCurrent0 - 0x14000 && v->a < mCurrent0 - 0xa000) {
                p.a = v->a;
                p.b = v->b;
                yv = mCurrent1 - 0xc000;
                xv = mCurrent0 - 0x14000;
                t1.a = xv;
                t1.b = yv;
                yv = mCurrent1 - 0xc000;
                xv = mCurrent0 - 0xa000;
                t2.a = xv;
                t2.b = yv;
                yv = mCurrent1 - 0x1000;
                xv = mCurrent0 - 0xa000;
                t3.a = xv;
                t3.b = yv;
                r = PointInTriangle(&p, &t1, &t2, &t3);
                if (r == 0)
                    goto in1;
                return 0;
            in1:
                return 1;
            }
            if (v->a >= mCurrent0 + 0xa000 && v->a < mCurrent0 + 0x16000) {
                p2.a = v->a;
                p2.b = v->b;
                yv2 = mCurrent1 - 0xc000;
                xv2 = mCurrent0 + 0x14000;
                t4.a = xv2;
                t4.b = yv2;
                yv2 = mCurrent1 - 0xc000;
                xv2 = mCurrent0 + 0xa000;
                t5.a = xv2;
                t5.b = yv2;
                yv2 = mCurrent1 - 0x1000;
                xv2 = mCurrent0 + 0xa000;
                t6.a = xv2;
                t6.b = yv2;
                r = PointInTriangle(&p2, &t4, &t5, &t6);
                if (r == 0)
                    goto in2;
                return 0;
            in2:
                return 1;
            }
            return 0;
        }
        return 0;
    }
    if (func_0203d5bc(&d.a) < 0x100000)
        return 0;
    return func_0203d5bc(&d.a) < 0x190000;
}

/* Slot 1: draws the chewing frames while biting, otherwise the idle pair. */
void cMgSmartball_pakkun_c::Update() {
    if (mIsActive == 0)
        return;
    {
        int f38 = mActionTimer;
        if (f38 > 0) {
            int scale;
            int idx = 1;
            int b, a;
            struct {
                int _00, _01, _10, _11;
            } vec;
            if (f38 < 0x20) {
                scale = FX_Div(0x20000, f38 << 12);
            } else {
                scale = 0x1000;
                if ((f38 - 0x20) % 16 < 4)
                    idx = 0;
            }
            b = (int)((((long long)FX_SinCosTable_[1] * scale) + 0x800) >> 12);
            a = (int)((((long long)FX_SinCosTable_[0] * scale) + 0x800) >> 12);
            vec._00 = b;
            vec._01 = a;
            vec._10 = -a;
            vec._11 = b;
            RenderOamBothScreens(sSmartballPakkunSprites[idx], mCurrent0 >> 12, mCurrent1 >> 12, -1,
                                 0, &vec);
            RenderOamBothScreens(sSmartballPakkunSprites[2], mCurrent0 >> 12, (mCurrent1 >> 12) + 5,
                                 -1, 2, &vec);
        } else {
            if (mTriggered != 0) {
                Mg_RenderSpriteBoth(sSmartballPakkunSprites[0], mCurrent0 >> 12, mCurrent1 >> 12,
                                    -1, 0);
                Mg_RenderSpriteBoth(sSmartballPakkunSprites[2], mCurrent0 >> 12,
                                    (mCurrent1 >> 12) + 5, -1, 2);
            } else {
                Mg_RenderSpriteBoth(sSmartballPakkunSprites[1], mCurrent0 >> 12, mCurrent1 >> 12,
                                    -1, 0);
                Mg_RenderSpriteBoth(sSmartballPakkunSprites[2], mCurrent0 >> 12,
                                    (mCurrent1 >> 12) + 5, -1, 2);
            }
        }
    }
}

/* Slot 0: bites the first ball that comes close, otherwise blinks between
 * the idle frames. */
inline cMgSmartball_ball_c *GetObjPakkun(dScMgSmartball_c *g, int i) {
    return i >= 13 ? 0 : g->mpBalls[i];
}

void cMgSmartball_pakkun_c::SaveSnapshot() {
    int s[5];
    int i;
    int b;

    if (mIsActive == 0)
        return;
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mActionTimer > 0) {
        mActionTimer -= 1;
        if (mActionTimer == 0)
            mIsActive = 0;
        return;
    }
    for (i = 0; i < mpManager->mBallCount; i++) {
        CopyObjectPos(&s[0], GetObjPakkun(mpManager, i));
        Vec2_Sub(&s[2], &mCurrent0, &s[0]);
        b = Vec2_Len((const Vector2 *)&s[2]) < 0x4000;
        if (b) {
            mActionTimer = 0x61;
            mTriggered = 0;
            mpManager->mpWing->Trigger();
            GetObjPakkun(mpManager, i)->mIsActive = 0;
            func_02012718(0x1a0, mCurrent0);
            return;
        }
    }
    mIdleTimer += 1;
    if (mIdleTimer < 0x3c)
        return;
    mIdleTimer = 0;
    if (mTriggered == 0)
        mTriggered = 1;
    else
        mTriggered = 0;
}

/* Slot 2: clears the piranha's bite state. */
void cMgSmartball_pakkun_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mTriggered = 0;
    mIdleTimer = 0;
    mActionTimer = 0;
}

 /* C linkage, as in the original .c file */
cMgSmartball_pakkun_c *cMgSmartball_pakkun_c_Construct(cMgSmartball_pakkun_c *c,
                                                       dScMgSmartball_c *b, int d, V2 *s) {
    int tmp[2];
    tmp[0] = s->x;
    tmp[1] = s->y;
    CallObjectConstruct(c, b, d, tmp);
    c->~cMgSmartball_pakkun_c();
    c->mRadius = 0x14000;
    return c;
}


int cMgSmartball_dokan_c::CheckBall(int *b) {
    int ax = mCurrent0;
    int bx = b[0];
    if (bx >= ax - 0xe000 && bx < ax + 0xe000) {
        int ay = mCurrent1;
        int by = b[1];
        if (by >= ay - 0x48000 && by < ay + 0x28000) {
            if (bx < ax - 0xa000)
                return 1;
            return (bx >= ax + 0xa000);
        }
    }
    return 0;
}

/* Slot 1: draws the pipe as five stacked sprites. */
void cMgSmartball_dokan_c::Update() {
    int flag = 0;
    if (mpManager->mPhase)
        flag = 1;
    Mg_RenderSpriteBoth(sSmartballDokanSprites[0], mCurrent0 >> 12, (mCurrent1 >> 12) - 0x40, -1,
                        flag);
    Mg_RenderSpriteBoth(sSmartballDokanSprites[1], mCurrent0 >> 12, (mCurrent1 >> 12) - 0x30, -1,
                        flag);
    Mg_RenderSpriteBoth(sSmartballDokanSprites[1], mCurrent0 >> 12, mCurrent1 >> 12, -1, flag);
    Mg_RenderSpriteBoth(sSmartballDokanSprites[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 0x10, -1,
                        flag);
    Mg_RenderSpriteBoth(sSmartballDokanSprites[2], mCurrent0 >> 12, (mCurrent1 >> 12) + 0x20, -1,
                        flag);
}

/* Slot 0: scores a ball that falls into the pipe mouth. */
inline cMgSmartball_ball_c *GetObjDokan(dScMgSmartball_c *g, int i) {
    return i >= 13 ? 0 : g->mpBalls[i];
}

void cMgSmartball_dokan_c::SaveSnapshot() {
    int i;
    W s;

    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    for (i = 0; i < mpManager->mBallCount; i++) {
        if (GetObjDokan(mpManager, i)->mIsActive == 0)
            continue;
        if (GetObjDokan(mpManager, i)->mPipeUsed == 1)
            continue;
        CopyObjectPos(&s.v, GetObjDokan(mpManager, i));
        s.d = s.v;
        s.d.x = s.d.x - mCurrent0;
        s.d.y = s.d.y - mCurrent1;
        if (s.d.x < -0x8000)
            continue;
        if (s.d.x >= 0x8000)
            continue;
        if (s.d.y < -0x40000)
            continue;
        if (s.d.y >= -0x38000)
            continue;
        s.e.x = mCurrent0;
        s.e.y = -0x40000;
        mpManager->EmitScore(&s.e.x, 0xbb8, 0, 1);
        mpManager->CountDokan();
        GetObjDokan(mpManager, i)->mPipeUsed = 1;
        Sound::PlayBank2_2D(0x16);
    }
}

/* Slot 2: forwards to the base reset. */
void cMgSmartball_dokan_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
}

 /* C linkage, as in the original .c file */
cMgSmartball_dokan_c *cMgSmartball_dokan_c_Construct(cMgSmartball_dokan_c *c, dScMgSmartball_c *b,
                                                     int d, V2 *s) {
    int tmp[2];
    tmp[0] = s->x;
    tmp[1] = s->y;
    CallObjectConstruct(c, b, d, tmp);
    c->~cMgSmartball_dokan_c();
    c->mRadius = 0x20000;
    return c;
}


int cMgSmartball_propeller_c::HitTest(int *pos) {
    struct S *p = (struct S *)pos;
    struct S d;
    struct S s0, s1, s2, s3;
    struct S t0, t1, t2, t3;
    struct S u0, u1, u2, u3;
    struct S v0, v1, v2, v3;

    d.a = p->a;
    d.b = p->b;
    d.a -= mCurrent0;
    d.b -= mCurrent1;
    if (func_0203d5bc(&d.a) >= 0x400000)
        return 0;

    s0.a = p->a;
    s0.b = p->b;
    s1.a = mVertices[0].x;
    s1.b = mVertices[0].y;
    s2.a = mVertices[1].x;
    s2.b = mVertices[1].y;
    s3.a = mVertices[2].x;
    s3.b = mVertices[2].y;
    if (PointInTriangle(&s0, &s1, &s2, &s3))
        return 1;

    t0.a = p->a;
    t0.b = p->b;
    t1.a = mVertices[3].x;
    t1.b = mVertices[3].y;
    t2.a = mVertices[1].x;
    t2.b = mVertices[1].y;
    t3.a = mVertices[2].x;
    t3.b = mVertices[2].y;
    if (PointInTriangle(&t0, &t1, &t2, &t3))
        return 1;

    u0.a = p->a;
    u0.b = p->b;
    u1.a = mVertices[4].x;
    u1.b = mVertices[4].y;
    u2.a = mVertices[5].x;
    u2.b = mVertices[5].y;
    u3.a = mVertices[6].x;
    u3.b = mVertices[6].y;
    if (PointInTriangle(&u0, &u1, &u2, &u3))
        return 1;

    v0.a = p->a;
    v0.b = p->b;
    v1.a = mVertices[7].x;
    v1.b = mVertices[7].y;
    v2.a = mVertices[5].x;
    v2.b = mVertices[5].y;
    v3.a = mVertices[6].x;
    v3.b = mVertices[6].y;
    return PointInTriangle(&v0, &v1, &v2, &v3) != 0;
}

void cMgSmartball_propeller_c::RebuildVerts() {
    mVertices[0].x = 0x20000;
    mVertices[0].y = 0;
    mVertices[1].x = 0;
    mVertices[1].y = -0xb000;
    mVertices[2].x = 0;
    mVertices[2].y = 0xb000;
    mVertices[3].x = -0x20000;
    mVertices[3].y = 0;
    mVertices[4].x = 0;
    mVertices[4].y = 0x20000;
    mVertices[5].x = -0xb000;
    mVertices[5].y = 0;
    mVertices[6].x = 0xb000;
    mVertices[6].y = 0;
    mVertices[7].x = 0;
    mVertices[7].y = -0x20000;

    func_0203d388((V2 *)&mVertices[0], mAngle);
    func_0203d388((V2 *)&mVertices[1], mAngle);
    func_0203d388((V2 *)&mVertices[2], mAngle);
    func_0203d388((V2 *)&mVertices[3], mAngle);
    func_0203d388((V2 *)&mVertices[4], mAngle);
    func_0203d388((V2 *)&mVertices[5], mAngle);
    func_0203d388((V2 *)&mVertices[6], mAngle);
    func_0203d388((V2 *)&mVertices[7], mAngle);

    mVertices[0].x += mCurrent0;
    mVertices[0].y += mCurrent1;
    mVertices[1].x += mCurrent0;
    mVertices[1].y += mCurrent1;
    mVertices[2].x += mCurrent0;
    mVertices[2].y += mCurrent1;
    mVertices[3].x += mCurrent0;
    mVertices[3].y += mCurrent1;
    mVertices[4].x += mCurrent0;
    mVertices[4].y += mCurrent1;
    mVertices[5].x += mCurrent0;
    mVertices[5].y += mCurrent1;
    mVertices[6].x += mCurrent0;
    mVertices[6].y += mCurrent1;
    mVertices[7].x += mCurrent0;
    mVertices[7].y += mCurrent1;
}

/* Slot 1: draws the two propeller caps rotated by the base angle. */
void cMgSmartball_propeller_c::Update() {
    int b = (short)((((long long)FX_SinCosTable_[((unsigned short)mAngle >> 4) * 2 + 1] << 12) +
                     0x800) >>
                    12);
    int a;
    int vec[4];

    vec[0] = b;
    a = (short)((((long long)FX_SinCosTable_[((unsigned short)mAngle >> 4) * 2] << 12) + 0x800) >>
                12);
    vec[3] = b;
    vec[1] = a;
    vec[2] = -a;

    RenderOamBothScreens(sSmartballPropellerSprites[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1,
                         vec);
    RenderOamBothScreens(sSmartballPropellerSprites[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1,
                         2, vec);
}

/* Slot 0: steps the spin speed toward its target, advances the angle and
 * rebuilds the blades.
 * Leftover: the step writes through a short pointer; a plain assignment
 * produces a shorter body. */
void cMgSmartball_propeller_c::SaveSnapshot() {
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mpManager->mRoundEnded != 0)
        return;
    if (mSpinSpeed < mSpinSpeedTarget) {
        short *p = &mSpinSpeed;
        *p = *p + 8;
        if (mSpinSpeed > mSpinSpeedTarget)
            mSpinSpeed = mSpinSpeedTarget;
    } else {
        short *p = &mSpinSpeed;
        *p = *p - 8;
        if (mSpinSpeed < mSpinSpeedTarget)
            mSpinSpeed = mSpinSpeedTarget;
    }
    {
        short *q = &mAngle;
        *q = *q + mSpinSpeed;
    }
    RebuildVerts();
}

/* Slot 2: stops the propeller, resets its angle and rebuilds the blades. */
void cMgSmartball_propeller_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mAngle = 0;
    mSpinSpeedTarget = 0x40;
    mSpinSpeed = 0;
    RebuildVerts();
}

 /* C linkage, as in the original .c file */
cMgSmartball_propeller_c *cMgSmartball_propeller_c_Construct(cMgSmartball_propeller_c *c,
                                                             dScMgSmartball_c *b, int d, V2 *s) {
    int tmp[2];
    tmp[0] = s->x;
    tmp[1] = s->y;
    CallObjectConstruct(c, b, d, tmp);
    c->~cMgSmartball_propeller_c();
    c->mRadius = 0x20000;
    return c;
}


/* Slot 1: draws the spring pair for the current level mode. */
void cMgSmartball_spring_c::Update() {
    if ((mpManager->param1 & 0xff) == 0) {
        Mg_RenderSpriteBoth(sSmartballSpringSprites[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        Mg_RenderSpriteBoth(sSmartballSpringSprites[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1,
                            2);
    } else {
        Mg_RenderSpriteBoth(sSmartballSpringSpritesAlt[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        Mg_RenderSpriteBoth(sSmartballSpringSpritesAlt[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5,
                            -1, 2);
    }
}

/* Slot 0: touch-controlled spring: grab, drive, release, then fall back. */
void cMgSmartball_spring_c::SaveSnapshot() {
    int slot;
    int held;
    int flag;
    int d;

    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mpManager->mRoundEnded != 0)
        return;

    held = gTouchHeld[gActivePlayerSlot * 4];
    slot = gActivePlayerSlot;
    flag = 0;
    if (held != 0)
        flag = gTouchEdge[slot * 4] != 0;

    if (flag != 0) {
        mTriggered = 1;
        mVel0 = 0;
        mVel1 = 0;
        mGrabX = gTouchX[gActivePlayerSlot * 4] << 12;
        mGrabXSaved = mGrabX;
        mDriveNow = gTouchY[gActivePlayerSlot * 4] << 12;
        mDriveBase = mDriveNow;
        if (mCurrent1 <= 0xa0000)
            return;
        mDriveBase -= (mCurrent1 - 0xa0000);
        return;
    }

    if (mTriggered == 1 && held != 0) {
        mDriveNow = gTouchY[slot * 4] << 12;
        d = mDriveNow - mDriveBase;
        if (d >= 0x38000)
            mDriveNow = mDriveBase + 0x38000;
        else if (mDriveNow < mDriveBase)
            mDriveNow = mDriveBase;
        mCurrent1 = mDriveNow - mDriveBase + 0xa0000;
        if (mSnapshot1 == mCurrent1)
            return;
        mSoundHandle = Sound_PlayIfNotActive(mSoundHandle, 2, 0x16c, 0);
        return;
    }

    {
        int m;
        if (held == 0 && gTouchEdge[slot * 4] != 0)
            m = 1;
        else
            m = 0;
        if (m != 0) {
            mTriggered = 0;
            mVel0 = 0;
            mVel1 = 0;
            return;
        }
    }

    mVel1 -= (mCurrent1 - 0xa0000) / 16;
    mCurrent1 += mVel1;
    if (mCurrent1 < 0xa0000) {
        mCurrent1 = 0xa0000;
        mVel1 = 0;
    }
}

/* Slot 2: clears the sound handle, the only field this override resets. */
void cMgSmartball_spring_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mSoundHandle = 0;
}

 /* C linkage, as in the original .c file */
cMgSmartball_spring_c *cMgSmartball_spring_c_Construct(cMgSmartball_spring_c *c,
                                                       dScMgSmartball_c *b, int d, V2 *s) {
    int tmp[2];
    tmp[0] = s->x;
    tmp[1] = s->y;
    CallObjectConstruct(c, b, d, tmp);
    c->~cMgSmartball_spring_c();
    c->mRadius = 0x20000;
    return c;
}


int cMgSmartball_ana_c::HasBall() {
    return mHasBall;
}

void cMgSmartball_ana_c::Swallow() {
    int tmp1[2];
    int tmp2[2];
    mRespawnTimer = 0x3c;
    mHasBall = 1;
    tmp1[0] = mCurrent0;
    tmp1[1] = mCurrent1;
    mpManager->EmitSparks((int *)tmp1, 0);
    tmp2[0] = mCurrent0;
    tmp2[1] = mCurrent1;
    mpManager->EmitScore((int *)tmp2, (int)0x7d0, (int)0, (int)1);
    mpManager->CountAna();
    func_02012718(0xe0, mCurrent0);
}

/* Slot 1: draws the hole, with a warning sprite while it holds a ball. */
void cMgSmartball_ana_c::Update() {
    if (mHasBall == 1) {
        int v = mRespawnTimer;
        int idx;
        if (v < 0x14)
            idx = 2;
        else if (v < 0x28)
            idx = 1;
        else
            idx = 0;
        if (idx >= 1) {
            Mg_RenderSpriteBoth(sSmartballAnaSprites[idx], mCurrent0 >> 0xc, mCurrent1 >> 0xc, -1,
                                2);
        }
    }
    Mg_RenderSpriteBoth(sSmartballAnaSprites[0], mCurrent0 >> 0xc, mCurrent1 >> 0xc, -1, 3);
}

/* Slot 0: counts down a swallowed ball, then serves the next one. */
void cMgSmartball_ana_c::SaveSnapshot() {
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mRespawnTimer <= 0)
        return;
    mRespawnTimer = mRespawnTimer - 1;
    if (mRespawnTimer > 0)
        return;
    mpManager->ServeNextBall();
    mServed = 1;
}

/* Slot 2: clears the hole's state. */
void cMgSmartball_ana_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mHasBall = 0;
    mServed = 0;
    mRespawnTimer = 0;
}

 /* C linkage, as in the original .c file */
cMgSmartball_ana_c *cMgSmartball_ana_c_Construct(cMgSmartball_ana_c *c, dScMgSmartball_c *b, int d,
                                                 V2 *s) {
    int tmp[2];
    tmp[0] = s->x;
    tmp[1] = s->y;
    CallObjectConstruct(c, b, d, tmp);
    c->~cMgSmartball_ana_c();
    c->mRadius = 0x10000;
    return c;
}


/* Slot 1: draws the mushroom (variant 0) or its turning shell (variant 1). */
void cMgSmartball_kinoko_c::Update() {
    switch (mVariant) {
    case 0:
        Mg_RenderSpriteBoth(sSmartballKinokoSprites[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        Mg_RenderSpriteBoth(sSmartballKinokoSprites[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1,
                            2);
        break;
    case 1: {
        int d;
        int b, a;
        int vec[4];
        if (mRadius < 0x1000)
            return;
        d = FX_Div(0x7000, mRadius);
        b = FX_MUL(FX_SinCosTable_[1], d);
        a = FX_MUL(FX_SinCosTable_[0], d);
        vec[0] = b;
        vec[1] = a;
        vec[2] = -a;
        vec[3] = b;
        RenderOamBothScreens(sSmartballKinokoTurnSprites[0], mCurrent0 >> 12, mCurrent1 >> 12, -1,
                             1, vec);
        RenderOamBothScreens(sSmartballKinokoTurnSprites[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5,
                             -1, 2, vec);
    } break;
    }
}

/* Slot 0: scores a hit, then grows or shrinks the mushroom. */
void cMgSmartball_kinoko_c::SaveSnapshot() {
    V2 pair;
    V2 bufA;
    V2 bufB;
    dScMgSmartball_c *obj;
    int i;

    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;

    if (mWasHit == 1) {
        if (mHitTimer == 0) {
            pair.x = mCurrent0;
            pair.y = mCurrent1;
            mpManager->EmitScore(&pair.x, (int)0x12c, (int)0, (int)1);
            mpManager->CountKinoko();
            func_02012718(0x19f, mCurrent0);
        }
        mWasHit = 0;
        if (mVariant == 1)
            mHitTimer = 0x100;
        else
            mHitTimer = 4;
    } else if (mHitTimer > 0) {
        mHitTimer -= 1;
        if (mHitTimer < 4) {
            obj = mpManager;
            i = 0;
            if (obj->mBallCount > 0) {
                do {
                    cMgSmartball_ball_c *n = (i >= 0xd) ? 0 : obj->mpBalls[i];
                    if (n->mIsActive != 0) {
                        cMgSmartball_ball_c *m = (i >= 0xd) ? 0 : obj->mpBalls[i];
                        int hit;
                        CopyObjectPos(&bufA, m);
                        Vec2_Sub(&bufB.x, &mCurrent0, &bufA.x);
                        hit = Vec2_Len((const Vector2 *)(void *)&bufB) < 0x11000;
                        if (hit != 0) {
                            mHitTimer = 4;
                            goto done;
                        }
                    }
                    obj = mpManager;
                    i++;
                } while (i < obj->mBallCount);
            }
        }
    }

done:
    if (mVariant == 1) {
        if (mHitTimer <= 0) {
            mRadius += 0x1000;
            if (mRadius > 0x7000)
                mRadius = 0x7000;
        } else {
            mRadius -= 0x1000;
            if (mRadius < 0)
                mRadius = 0;
        }
    }
}

/* Slot 2: clears the hit state. */
void cMgSmartball_kinoko_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mWasHit = 0;
    mHitTimer = 0;
}

 /* C linkage, as in the original .c file */
cMgSmartball_kinoko_c *cMgSmartball_kinoko_c_Construct(cMgSmartball_kinoko_c *self,
                                                       dScMgSmartball_c *x, int y, V2 *src) {
    int v[2];
    v[0] = src->x;
    v[1] = src->y;
    CallObjectConstruct(self, x, y, v);
    self->~cMgSmartball_kinoko_c();
    self->mRadius = 0x7000;
    self->mVariant = 2;
    return self;
}


void cMgSmartball_ball_c::PlayHitSound(int a1, int *vel) {
    int spd;

    if (soundTimer > 0) {
        soundTimer = 3;
        return;
    }
    {
        int v[2];
        Vec2_Sub(v, &mVel0, vel);
        spd = Vec2_Len((const Vector2 *)v);
    }
    if (spd < 0x1000)
        return;
    switch (a1) {
    case 0x16f: {
        int vol = (spd << 4) >> 12;
        if (vol > 0x7f)
            vol = 0x7f;
        func_020126ac(a1, 5, vol, 0, func_020126e8(mCurrent0));
        {
            int pos[2];
            pos[0] = mCurrent0;
            pos[1] = mCurrent1;
            mpManager->SpawnDust((int *)pos);
        }
        break;
    }
    case 0x171: {
        int vol = (((spd - 0x800) << 2) >> 12) + 0x32;
        if (vol > 0x7f)
            vol = 0x7f;
        func_020126ac(a1, 5, vol, 0, func_020126e8(mCurrent0));
        break;
    }
    case 0x172: {
        int vol = (((spd - 0x800) << 2) >> 12) + 0x32;
        if (vol > 0x7f)
            vol = 0x7f;
        func_020126ac(a1, 5, vol, 0, func_020126e8(mCurrent0));
        break;
    }
    case 0x1a3: {
        int vol = (((spd - 0x800) << 2) >> 12) + 0x32;
        if (vol > 0x7f)
            vol = 0x7f;
        func_020126ac(a1, 5, vol, 0, func_020126e8(mCurrent0));
        break;
    }
    }
    soundTimer = 3;
}

void cMgSmartball_ball_c::Freeze() {
    mFrozenPos0 = mCurrent0;
    mFrozenPos1 = mCurrent1;
    mIsFrozen = 1;
    state3b = 1;
}

int cMgSmartball_ball_c::HasHit() {
    return anyHit;
}

int cMgSmartball_ball_c::IsOut() {
    if (mIsActive == 0)
        return 1;
    if (state3a == 1)
        return 1;
    int v = mCurrent1;
    if (v < -0xa0000)
        return 1;
    if (v < 0) {
        if (mCurrent0 < 0xd0000)
            return 1;
    }
    return 0;
}

int cMgSmartball_ball_c::ArmExpire(int val) {
    if (mIsActive == 0)
        return 0;
    if (mExpireTimer > 0)
        return 0;
    mExpireTimer = val;
    return 1;
}

int cMgSmartball_ball_c::IsFinished() {
    if ((mpManager->param1 & 0xff) != 0 && mCurrent0 < 0xe0000 && mCurrent1 >= 0x50000 &&
        mCurrent1 < 0x88000) {
        if (mStuckFrames >= 0x78)
            return 1;
    }
    if (mInPlay == 0)
        return 1;
    return state3b;
}

void cMgSmartball_ball_c::Burst() {
    int v[2];
    v[0] = mCurrent0;
    v[1] = mCurrent1;
    mpManager->EmitSparks((int *)v, 0);
}

void cMgSmartball_ball_c::PutInPlay() {
    mInPlay = 1;
    Burst();
}

/* Tests the ball against the wing.
 * Leftover: the two wing reads stay member accesses on the manager. */
int cMgSmartball_ball_c::HitWing(int *src) {
    cMgSmartball_wing_c *wing = mpManager->mpWing;
    if (wing == 0)
        return 0;
    struct V2 v;
    v.x = src[0];
    v.y = src[1];
    return mpManager->mpWing->HitTest(&v.x);
}

int cMgSmartball_ball_c::HitSwitches(int *arg) {
    int buf[2];
    int i = 0;
    while (i < mpManager->mPushSwitchCount) {
        cMgSmartball_pushswitch_c *p;
        dScMgSmartball_c *base;
        buf[0] = arg[0];
        buf[1] = arg[1];
        base = mpManager;
        p = (i >= base->mPushSwitchCount) ? 0 : base->mpPushSwitches[i];
        if (p->CheckBall(buf))
            return 1;
        i++;
    }
    return 0;
}

int cMgSmartball_ball_c::HitPakkuns(int *arg) {
    int buf[2];
    int i = 0;
    while (i < mpManager->mPakkunCount) {
        cMgSmartball_pakkun_c *p;
        dScMgSmartball_c *base;
        buf[0] = arg[0];
        buf[1] = arg[1];
        base = mpManager;
        p = (i >= base->mPakkunCount) ? 0 : base->mpPakkuns[i];
        if (p->CheckBall(buf))
            return 1;
        i++;
    }
    return 0;
}

int cMgSmartball_ball_c::HitDokans(int *arg) {
    int buf[2];
    int i = 0;
    while (i < mpManager->mDokanCount) {
        cMgSmartball_dokan_c *p;
        dScMgSmartball_c *base;
        buf[0] = arg[0];
        buf[1] = arg[1];
        base = mpManager;
        p = (i >= base->mDokanCount) ? 0 : base->mpDokans[i];
        if (p->CheckBall(buf))
            return 1;
        i++;
    }
    return 0;
}

int cMgSmartball_ball_c::HitPropellers(int *pos) {
    struct S *r7 = (struct S *)pos;
    struct S loc;
    int i;
    if (r7->b >= 0x90000)
        return 0;
    i = 0;
    if (mpManager->mPropellerCount > 0) {
        do {
            dScMgSmartball_c *base;
            cMgSmartball_propeller_c *propeller;
            loc.a = r7->a;
            loc.b = r7->b;
            base = mpManager;
            if (i >= base->mPropellerCount)
                propeller = 0;
            else
                propeller = base->mpPropellers[i];
            if (propeller->HitTest((int *)(&loc)) != 0)
                return 1;
            i++;
        } while (i < mpManager->mPropellerCount);
    }
    return 0;
}

int cMgSmartball_ball_c::HitKinokos(int *pos) {
    int i;
    dScMgSmartball_c *base;
    int n;
    int r7;
    int delta[2];
    int out[2];
    if (pos[1] >= 0x90000)
        return 0;
    base = mpManager;
    n = base->mKinokoCount;
    i = 0;
    if (n > 0) {
        do {
            if (nearby[i]) {
                r7 = (i >= n ? 0 : base->mpKinokos[i])->HitRadiusSq();
                if (r7 > 0) {
                    delta[0] = pos[0];
                    delta[1] = pos[1];
                    CopyObjectPos(out, i >= mpManager->mKinokoCount ? 0 : mpManager->mpKinokos[i]);
                    delta[0] = delta[0] - out[0];
                    delta[1] = delta[1] - out[1];
                    if (func_0203d5bc(delta) < r7) {
                        void *e =
                            i >= mpManager->mKinokoCount ? 0 : (void *)mpManager->mpKinokos[i];
                        ((cMgSmartball_kinoko_c *)e)->mWasHit = 1;
                        return 1;
                    }
                }
            }
            i++;
            base = mpManager;
            n = base->mKinokoCount;
        } while (i < n);
    }
    return 0;
}

int cMgSmartball_ball_c::CheckExits(int *pos) {
    struct S *arg = (struct S *)pos;
    struct S p, q0, q1, q2;
    struct S d, e, f, g;
    if (arg->b >= 0)
        return 0;
    if (arg->a >= 0x8000 && arg->a < 0x28000) {
        if (arg->b >= -0x90000 && arg->b < -0x88000) {
            mpManager->KickGate(0);
            return 1;
        }
        p.a = arg->a;
        p.b = arg->b;
        q0.a = 0x8000;
        q0.b = -0x92000;
        q1.a = 0x8000;
        q1.b = -0x90000;
        q2.a = 0x28000;
        q2.b = -0x90000;
        if (PointInTriangle(&p, &q0, &q1, &q2)) {
            mpManager->KickGate(0);
            return 1;
        }
    }
    if (mExitGateOpen == 1 && mVel1 >= 0 && arg->a >= 0xd0000 && arg->a < 0xf8000) {
        d.a = arg->a;
        d.b = arg->b;
        e.a = 0xd8000;
        e.b = -0x80000;
        f.a = 0xf8000;
        f.b = -0x80000;
        g.a = 0xf8000;
        g.b = -0x88000;
        if (PointInTriangle(&d, &e, &f, &g)) {
            mpManager->KickGate(1);
            return 1;
        }
    }
    return 0;
}

int cMgSmartball_ball_c::AtSpring(int *b) {
    int local[2];
    dScMgSmartball_c *base = mpManager;
    if (mIndex != base->mServeIndex)
        return 0;
    if (b[0] >= 0xdc000) {
        CopyObjectPos(local, base->mpSpring);
        if (b[1] >= local[1] - 0x20000)
            return 1;
    }
    return 0;
}

int cMgSmartball_ball_c::OnSlope(int *p) {
    int local[2];
    unsigned short v;
    int x, z, diff;

    local[0] = p[0];
    local[1] = p[1];
    v = mpManager->GetTile(local);

    if (v == 0x24d) {
        z = (p[1] >> 12) & 7;
        x = (p[0] >> 12) & 7;
        diff = x - z;
        if (x + z >= 8 && diff >= 0)
            return 1;
    } else if (v == 0x24e) {
        z = (p[1] >> 12) & 7;
        x = (p[0] >> 12) & 7;
        diff = x - z;
        if (x + z < 8 && diff < 0)
            return 1;
    } else if (v == 0x24f) {
        z = (p[1] >> 12) & 7;
        x = (p[0] >> 12) & 7;
        diff = x - z;
        if (x + z >= 4 && x + z < 12 && diff >= -4 && diff < 4)
            return 1;
    } else if (v == 0x28f) {
        x = (p[0] >> 12) & 7;
        if (x >= 1 && x < 7)
            return 1;
    } else if (v == 0x2af) {
        x = (p[0] >> 12) & 7;
        if (x >= 1 && x < 7)
            return 1;
    }
    return 0;
}

int cMgSmartball_ball_c::NeedsRespawn(int *pos) {
    struct S *arg = (struct S *)pos;
    int z, z2, x;

    if (state3a == 1) {
        int dz = arg->b - mSwallowZ;
        int dx = arg->a - mSwallowX;
        int dxsq = FX_MUL(dx, dx);
        int dzsq = FX_MUL(dz, dz);
        return dxsq + dzsq >= 0x100000;
    }

    x = arg->a;
    if (x < 0x8000)
        return 1;
    if (x >= 0xf8000)
        return 1;
    if (x >= 0xd8000 && x < 0xe0000) {
        z = arg->b;
        if (z >= -0x80000) {
            if (z < 0x78000)
                return 1;
        }
        if (mUpperWallSolid != 0) {
            if (z >= 0x78000)
                return 1;
        }
        if (mExitGateOpen == 0) {
            if (z < -0x80000)
                return 1;
        }
    }

    z2 = arg->b;
    if (z2 < 0x80000)
        goto L1dc;
    if (z2 >= 0x88000)
        goto L14c;
    if (x < 0xe0000)
        return 1;
L14c: {
    struct S p, q0, q1, q2;
    p.a = x;
    p.b = arg->b;
    q0.a = 0x8000;
    q0.b = 0x98000;
    q1.a = 0x8000;
    q1.b = 0xa2000;
    q2.a = 0xe0000;
    q2.b = 0xa2000;
    if (PointInTriangle(&p, &q0, &q1, &q2))
        return 1;
}
    if (arg->a < 0x8000)
        goto L238;
    if (arg->a >= 0xe0000)
        goto L238;
    if (arg->b < 0xa2000)
        goto L238;
    return 1;
L1dc:
    if (z2 >= -0x70000)
        goto L238;
    {
        int u = x - 0x80000;
        int v = z2 + 0x70000;
        int usq = FX_MUL(u, u);
        int vsq = FX_MUL(v, v);
        if (usq + vsq >= 0x3840000)
            return 1;
    }
L238:
    return 0;
}

void cMgSmartball_ball_c::QueueBehind() {
    int cur;
    int val;
    dScMgSmartball_c *mgr;
    int vm1;
    cMgSmartball_ball_c *p;
    int local1[2];
    int local2[2];
    int newpos;
    int pos;

    val = mIndex;
    mgr = mpManager;
    cur = mgr->mServeIndex;

    if (val == cur) {
        if (mInPlay != 0) {
            UpdatePhysics();
            return;
        }
    }

    vm1 = val - 1;
    if (vm1 == cur) {
        p = (cur >= 0xd) ? 0 : mgr->mpBalls[cur];
        if (p->mInPlay != 0)
            goto block1;
    }

    if (val != cur || mInPlay != 0)
        goto block2;

block1:
    p = (cur >= 0xd) ? 0 : mgr->mpBalls[cur];
    CopyObjectPos(local1, p);
    newpos = local1[0] - 0x10000;
    pos = mCurrent0;
    if (pos < newpos && pos < 0xc8000)
        mCurrent0 = newpos;
    else
        mCurrent0 = mSnapshot0;
    mVel0 = 0;
    mVel1 = 0;

    mCurrent1 = FX_Div(FX_MUL(mCurrent0, 0xa000), 0xd8000) + 0x90000;

    if (mIndex > mpManager->mServeIndex) {
        mQueueGap = (mIndex - mpManager->mServeIndex) * 0xf;
    }
    return;

block2:
    p = (vm1 >= 0xd) ? 0 : mgr->mpBalls[vm1];
    CopyObjectPos(local2, p);
    mCurrent0 = local2[0] - 0x10000;
    mVel0 = 0;
    mVel1 = 0;

    mCurrent1 = FX_Div(FX_MUL(mCurrent0, 0xa000), 0xd8000) + 0x90000;

    if (mIndex > mpManager->mServeIndex) {
        mQueueGap = (mIndex - mpManager->mServeIndex) * 0xf;
    }
}

/* Leftover: UpdatePhysics needs strength reduction off. */
#pragma push
#pragma opt_strength_reduction off
/* Leftover: the owner selection keeps its own address computation.
   Dropping this extern "C" changes UpdatePhysics by 53 words. */
extern "C" {

void cMgSmartball_ball_c::UpdatePhysics() {
    State2 state;
    V2xz work;
    /* volatile: the ROM keeps the entry velocity on the stack and reloads it. */
    volatile V2xz savedVel;
    V2xz dist;
    V2xz delta;
    V2xz otherPos;
    V2xz normal;
    V2xz unit;
    V2xz response;
    V2xz radial;
    V2xz hazardDelta;
    V2xz correction;
    V2xz pos0;
    V2xz pos1;
    V2xz pos2;
    V2xz pos3;
    V2xz pos4;
    V2xz pos5;
    V2xz pos6;
    V2xz pos7;
    V2xz pos8;
    V2xz pos9;
    V2xz pos10;
    V2xz pos11;
    V2xz pos12;
    V2xz entityPos;
    V2xz entityDelta;
    V2xz entityPos2;
    V2xz radialDelta;
    V2xz moveDelta;
    V2xz normalized;
    V2xz specialPos;
    V2xz specialDelta0;
    V2xz specialDelta1;
    V2xz specialDelta2;
    V2xz event0;
    V2xz event1;
    V2xz event2;
    V2xz event3;
    V2xz hazardPos;
    V2xz hazardPos2;
    int blocked;
    int i;
    int entityLoopIndex;
    int angle;
    int responseScale;
    int dot;
    int volume;
    void *other;
    void *fallback;
    dScMgSmartball_c *mgr;
    s64 square;
    int correctionZ;
    int correctionX;
    int *velXPtr;
    int *velZPtr;

    savedVel.x = mVel0;
    state.flags[0] = 0;
    savedVel.z = mVel1;
    targetIndex = -1;
    state.flags[1] = 0;
    state.flags[2] = 0;
    state.flags[3] = 0;
    state.flags[4] = 0;
    state.flags[5] = 0;
    state.iteration = 0;
    state.flags[6] = 0;
    i = 0;
    state.iteration = 0;
    do {
        hitA[i] = (u8)state.iteration;
        hitC[i] = (u8)state.iteration;
        hitB[i] = (u8)state.iteration;
        i++;
    } while (i < 0x20);
    anyHit = (u8)state.iteration;
    specialHit = (u8)state.iteration;
    state.iteration = 0;

    if (mCurrent1 >= 0x77000) {
        int clearIndex = state.iteration;
        do {
            nearby[clearIndex] = 0;
            clearIndex++;
        } while (clearIndex < 0x19);
    } else {
        int probeIndex = state.iteration;
        do {
            nearby[probeIndex] = 0;
            if (probeIndex < mpManager->mKinokoCount) {
                dist.x = mCurrent0;
                dist.z = mCurrent1;
                mgr = mpManager;
                CopyObjectPos(&pos0,
                              probeIndex >= mgr->mKinokoCount ? 0 : mgr->mpKinokos[probeIndex]);
                dist.x -= pos0.x;
                dist.z -= pos0.z;
                if (func_0203d5bc(&dist.x) <= 0x900000LL) {
                    nearby[probeIndex] = 1;
                    state.iteration = 1;
                }
            }
            probeIndex++;
        } while (probeIndex < 0x19);
    }

    state.depth = 0;
    state.zero = 0;
    state.null34 = (void *)0;
    state.null38 = (void *)0;
    state.null3c = 0;
    state.angle40 = 0;
    state.index44 = 0;
    state.zero4c = 0;
    state.baseZ = 0x800;
    state.zero48 = 0;
    state.zeroAngle = 0;

outer_loop:
    angle = state.zeroAngle;
    blocked = angle;
    i = angle;
    do {
        hit[i] = (u8)state.zero;
        work.x = state.zero;
        work.z = state.zero;
        work.z += mRadius;
        func_0203d388((V2 *)&work, angle);
        dot = func_0203d570((V2 *)&work, (V2 *)&mVel0);
        if (dot >= 0) {
            work.x += mCurrent0;
            work.z += mCurrent1;
            if ((u8)mpManager->param1 == 0) {
                pos1 = work;
                if (NeedsRespawn(&pos1.x) != 0) {
                    hit[i] = 1;
                    hitA[i] = 1;
                    anyHit = 1;
                    blocked = 1;
                    goto final_checks;
                } else {
                    pos2 = work;
                    if (OnSlope(&pos2.x) != 0) {
                        hit[i] = 1;
                        hitA[i] = 1;
                        anyHit = 1;
                        state.flags[0] = 1;
                        blocked = 1;
                    } else {
                        pos3 = work;
                        if (CheckExits(&pos3.x) != 0) {
                            hit[i] = 1;
                            hitA[i] = 1;
                            anyHit = 1;
                            hitB[i] = 1;
                            state.flags[2] = 1;
                            blocked = 1;
                        }
                    }
                }
            } else {
                pos4 = work;
                if (NeedsRespawn(&pos4.x) != 0) {
                    hit[i] = 1;
                    hitA[i] = 1;
                    anyHit = 1;
                    blocked = 1;
                } else {
                    pos5 = work;
                    if (CheckExits(&pos5.x) != 0) {
                        hit[i] = 1;
                        hitA[i] = 1;
                        anyHit = 1;
                        hitB[i] = 1;
                        state.flags[2] = 1;
                        blocked = 1;
                        goto final_checks;
                    } else {
                        if (state.iteration == 1) {
                            pos6 = work;
                            if (HitKinokos(&pos6.x) != 0) {
                                hit[i] = 1;
                                hitA[i] = 1;
                                anyHit = 1;
                                specialHit = 1;
                                state.flags[3] = 1;
                                blocked = 1;
                                goto final_checks;
                            }
                        }
                        pos7 = work;
                        if (HitDokans(&pos7.x) != 0) {
                            hit[i] = 1;
                            hitA[i] = 1;
                            anyHit = 1;
                            blocked = 1;
                        } else {
                            pos8 = work;
                            if (HitPakkuns(&pos8.x) != 0) {
                                hit[i] = 1;
                                hitA[i] = 1;
                                anyHit = 1;
                                blocked = 1;
                            } else {
                                pos9 = work;
                                if (HitSwitches(&pos9.x) != 0) {
                                    hit[i] = 1;
                                    hitA[i] = 1;
                                    anyHit = 1;
                                    blocked = 1;
                                } else {
                                    pos10 = work;
                                    if (HitWing(&pos10.x) != 0) {
                                        hit[i] = 1;
                                        hitA[i] = 1;
                                        anyHit = 1;
                                        blocked = 1;
                                        state.flags[4] = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            work.x += mCurrent0;
            work.z += mCurrent1;
        }

    final_checks:
        if (blocked == 0) {
            pos11 = work;
            if (AtSpring(&pos11.x) != 0) {
                hit[i] = 1;
                hitA[i] = 1;
                anyHit = 1;
                blocked = 1;
                if (angle == 0)
                    state.flags[5] = 1;
            } else {
                pos12 = work;
                if (HitPropellers(&pos12.x) != 0) {
                    hit[i] = 1;
                    hitA[i] = 1;
                    anyHit = 1;
                    blocked = 1;
                    state.flags[1] = 1;
                }
            }
        }
        angle = (s16)(angle + 0x800);
        i++;
    } while (i < 0x20);

    if (state.flags[6] < 0x11 && state3a == 0) {
        entityLoopIndex = 0;
        if (mpManager->mBallCount > 0) {
            do {
                if (entityLoopIndex != mIndex) {
                    other =
                        entityLoopIndex >= 0xd ? state.null34 : mpManager->mpBalls[entityLoopIndex];
                    if (((cMgSmartball_ball_c *)other)->state3a == 0) {
                        other = entityLoopIndex >= 0xd ? state.null38
                                                       : mpManager->mpBalls[entityLoopIndex];
                        if (((cMgSmartball_object_c *)other)->mIsActive != 0) {
                            other = entityLoopIndex >= 0xd ? (void *)state.null3c
                                                           : mpManager->mpBalls[entityLoopIndex];
                            CopyObjectPos(&entityPos, other);
                            Vec2_Sub(&entityDelta.x, &mCurrent0, &entityPos.x);
                            delta = entityDelta;
                            if (func_0203d5bc(&delta.x) <= 0x100000LL) {
                                angle = state.angle40;
                                mgr = mpManager;
                                other = entityLoopIndex >= 0xd ? (void *)angle
                                                               : mgr->mpBalls[entityLoopIndex];
                                CopyObjectPos(&entityPos2, other);
                                otherPos = entityPos2;
                                if (mCurrent1 < 0x90000 || mCurrent0 <= otherPos.x) {
                                    i = state.index44;
                                    do {
                                        work.z = mRadius;
                                        work.x = 0;
                                        func_0203d388((V2 *)&work, angle);
                                        work.x += mCurrent0;
                                        work.z += mCurrent1;
                                        Vec2_Sub(&radialDelta.x, &work.x, &otherPos.x);
                                        delta = radialDelta;
                                        if (func_0203d5bc(&delta.x) < 0x40000LL) {
                                            targetIndex = entityLoopIndex;
                                            hit[i] = 1;
                                            hitA[i] = 1;
                                            hitC[i] = 1;
                                            blocked = 1;
                                        }
                                        angle = (s16)(angle + 0x800);
                                        i++;
                                    } while (i < 0x20);
                                }
                            }
                        }
                    }
                }
                entityLoopIndex++;
            } while (entityLoopIndex < mpManager->mBallCount);
        }
    }

    normal.x = state.zero48;
    normal.z = state.zero48;
    angle = state.zero48;
    i = state.zero48;
    do {
        if (hit[i] == 1) {
            unit.x = state.zero4c;
            unit.z = state.baseZ;
            func_0203d388((V2 *)&unit, angle);
            normal.x += unit.x;
            normal.z += unit.z;
        }
        angle = (s16)(angle + 0x800);
        i++;
    } while (i < 0x20);
    func_0203d434((V2 *)&normal);
    func_0203d630((V2 *)&normal, state.baseZ);
    {
        V2xz *position = (V2xz *)&mCurrent0;
        cMgSmartball_ball_c *owner = position ? this : this;
        int *pz = &owner->mCurrent1;
        position->x -= normal.x;
        *pz -= normal.z;
        Vec2_Sub(&moveDelta.x, (int *)position, &mSnapshot0);
    }
    dot = Vec2_Len((const Vector2 *)&moveDelta);
    state.depth += 0x80;
    state.flags[6]++;
    if (blocked == 1 && state.flags[6] < 0x21 && state.depth <= dot)
        goto outer_loop;

    i = 0;
    response.x = 0;
    response.z = 0;
    if (state.flags[2] == 1) {
        angle = 0;
        do {
            if (hitB[i] == 1) {
                radial.x = 0;
                radial.z = 0x1000;
                func_0203d388((V2 *)&radial, (s16)(angle / 0x20));
                response.x -= radial.x;
                response.z -= radial.z;
            }
            i++;
            angle += 0x10000;
        } while (i < 0x20);
    } else {
        angle = 0;
        do {
            if (hitA[i] == 1 && hitC[i] == 0) {
                radial.x = 0;
                radial.z = 0x1000;
                func_0203d388((V2 *)&radial, (s16)(angle / 0x20));
                response.x -= radial.x;
                response.z -= radial.z;
            }
            i++;
            angle += 0x10000;
        } while (i < 0x20);
    }

    if (response.x != 0 || response.z != 0) {
        func_0203d480((Fix12i *)&normalized, (Fix12i *)&response);
        dot = FX_MUL(mVel0, response.x) + FX_MUL(mVel1, response.z);
        if (state.flags[5] == 1) {
            responseScale = FX_MUL(dot, 0x1800);
        } else if (state.flags[0] == 1) {
            u32 randomScale;
            u32 randomAngle;
            responseScale = FX_MUL(dot, 0x1400);
            randomScale =
                ((((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * 0x10) >> 15)
                 << 12);
            responseScale += FIX_MUL_SU(responseScale, randomScale >> 6);
            randomAngle = (u32)RandomIntInternal(&data_0209d4b8.value);
            func_0203d388((V2 *)&response,
                          (s16)(((((randomAngle >> 16) & 0x7fff) << 12) >> 15) - 0x800));
        } else if (state.flags[3] == 1) {
            u32 randomScale;
            u32 randomAngle;
            mVel0 = 0;
            mVel1 = 0;
            responseScale = -0x2000;
            randomScale =
                ((((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * 0x10) >> 15)
                 << 12);
            responseScale += FIX_MUL_US(randomScale >> 5, -0x2000);
            randomAngle = (u32)RandomIntInternal(&data_0209d4b8.value);
            func_0203d388((V2 *)&response,
                          (s16)(((((randomAngle >> 16) & 0x7fff) << 12) >> 15) - 0x800));
        } else if (state.flags[2] == 1) {
            u32 randomScale;
            responseScale = FX_MUL(dot, 0x1a00);
            randomScale =
                ((((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * 0x10) >> 15)
                 << 12);
            responseScale += FIX_MUL_SU(responseScale, randomScale >> 6);
            if (mCurrent0 < 0x18000 && response.z > 0) {
                response.x = 0;
                response.z = 0x1000;
            }
        } else {
            responseScale = FX_MUL(dot, 0x1200);
        }
        velXPtr = &mVel0;
        *velXPtr -= FX_MUL(responseScale, response.x);
        velZPtr = &mVel1;
        *velZPtr -= FX_MUL(responseScale, response.z);
    }

    if (state.flags[5] == 1) {
        if (mCurrent0 < 0xd0000)
            return;
        if (mCurrent0 < 0xd8000) {
            mCurrent0 = 0xd8000;
            return;
        }
        if (mCurrent0 < 0xe8000)
            mCurrent0 = 0xe8000;
        CopyObjectPos(&specialPos, mpManager->mpSpring);
        mCurrent1 = specialPos.z - 0x20001 - mRadius;
        GetObjectDelta(&specialDelta0, mpManager->mpSpring);
        if (specialDelta0.z >= -0x1000)
            return;
        GetObjectDelta(&specialDelta1, mpManager->mpSpring);
        if (mVel1 < specialDelta1.z)
            return;
        mVel0 = 0;
        GetObjectDelta(&specialDelta2, mpManager->mpSpring);
        mVel1 = specialDelta2.z;
        if (mVel1 < -0x6000)
            mVel1 = (mVel1 + 0x6000) / 4 - 0x6000;

        if (soundTimer == 0) {
            if (mVel1 < -0x6000) {
                func_02012718(0x16d, mCurrent0);
                func_02012718(0x16e, mCurrent0);
            } else {
                volume = (-mVel1 << 7) / 0x6000;
                if (volume > 0x7f)
                    volume = 0x7f;
                if (volume < 0)
                    volume = 0;
                func_020126ac(0x16d, 5, volume, 0, func_020126e8(mCurrent0));
                if (volume >= 0x40)
                    func_020126ac(0x16e, 5, volume, 0, func_020126e8(mCurrent0));
            }
            soundTimer = 5;
        }
        if (soundPlayed != 0)
            return;
        if (mVel1 < -0x6000)
            func_02012718(0x16e, mCurrent0);
        soundPlayed = 1;
        return;
    }

    if (state.flags[0] == 1) {
        event0.x = savedVel.x;
        event0.z = savedVel.z;
        PlayHitSound(0x171, &event0.x);
    } else if (state.flags[1] == 1) {
        event1.x = savedVel.x;
        event1.z = savedVel.z;
        PlayHitSound(0x172, &event1.x);
    } else if (state.flags[2] == 1) {
        event2.x = savedVel.x;
        event2.z = savedVel.z;
        PlayHitSound(0x16f, &event2.x);
    } else if (state.flags[4] == 1) {
        event3.x = savedVel.x;
        event3.z = savedVel.z;
        PlayHitSound(0x1a3, &event3.x);
    }

    dot = mRadius + 0x10000;
    square = FX_MUL(dot, dot);
    i = 0;
    if (mpManager->mAnaCount <= 0)
        return;
    do {
        other = i >= mpManager->mAnaCount ? (void *)0 : mpManager->mpAnas[i];
        if (((cMgSmartball_ana_c *)other)->HasBall() == 0) {
            hazardDelta.x = mCurrent0;
            hazardDelta.z = mCurrent1;
            other = i >= mpManager->mAnaCount ? (void *)0 : mpManager->mpAnas[i];
            CopyObjectPos(&hazardPos, other);
            hazardDelta.x -= hazardPos.x;
            hazardDelta.z -= hazardPos.z;
            hazardDelta.z *= 2;
            if (func_0203d5bc(&hazardDelta.x) < square) {
                dot = mRadius + 0x10000 - Vec2_Len((const Vector2 *)&hazardDelta);
                if (mRadius < dot) {
                    other = i >= mpManager->mAnaCount ? (void *)0 : mpManager->mpAnas[i];
                    CopyObjectPos(&hazardPos2, other);
                    mSwallowX = hazardPos2.x;
                    mSwallowZ = hazardPos2.z;
                    other = i >= mpManager->mAnaCount ? (void *)0 : mpManager->mpAnas[i];
                    ((cMgSmartball_ana_c *)other)->Swallow();
                    ArmExpire(0x80);
                    state3a = 1;
                    return;
                }
                correction = hazardDelta;
                func_0203d434((V2 *)&correction);
                correction.x = FX_MUL(correction.x, dot);
                correction.x /= 64;
                correction.z = FX_MUL(correction.z, dot);
                correction.z /= 64;
                correctionX = correction.x;
                correctionZ = correction.z;
                velXPtr = &mVel0;
                *velXPtr -= correctionX;
                velZPtr = &mVel1;
                *velZPtr -= correction.z;
                return;
            }
        }
        i++;
    } while (i < mpManager->mAnaCount);
}}

#pragma pop

void cMgSmartball_ball_c::DrawMarker() {
    long long sq;
    int v;
    int i;
    int s[6];

    if (mIsActive == 0)
        return;
    if (mInPlay == 0)
        return;
    if (state3a == 1)
        return;

    if ((mpManager->param1 & 0xff) != 0) {
        int x = mCurrent0;
        if (x >= 0x40000 && x < 0xa0000) {
            int y = mCurrent1;
            if (y >= -0xb5000 && y < -0x85000)
                return;
        }
        v = mRadius + 0x15000;
        sq = FX_MUL(v, v);
        for (i = 0; i < mpManager->mAnaCount; i++) {
            if (((cMgSmartball_ana_c *)((
                     unsigned char *)((i >= mpManager->mAnaCount) ? 0 : mpManager->mpAnas[i])))
                    ->HasBall() == 0) {
                s[0] = mCurrent0;
                s[1] = mCurrent1;
                s[1] += 0x5000;
                CopyObjectPos(&s[4], (i >= mpManager->mAnaCount) ? 0 : mpManager->mpAnas[i]);
                s[0] = s[0] - s[4];
                s[1] = s[1] - s[5];
                s[1] = s[1] * 2;
                if (func_0203d5bc(&s[0]) < sq)
                    return;
            }
        }
    }

    if ((mpManager->param1 & 0xff) == 0)
        RenderOamBothScreens(sSmartballBallSprites[1], (mCurrent0 + 0x800) >> 12,
                             ((mCurrent1 + 0x800) >> 12) + 5, -1, 2, 0);
    else
        RenderOamBothScreens(sSmartballBallSpritesAlt[1], (mCurrent0 + 0x800) >> 12,
                             ((mCurrent1 + 0x800) >> 12) + 5, -1, 2, 0);
}

/* Slot 1: draws the ball sprite for the current level mode. */
void cMgSmartball_ball_c::Update() {
    int r2;
    if (mIsActive == 0)
        return;
    if (mInPlay == 0)
        return;
    r2 = 1;
    if (state3a == 1)
        r2 = 3;
    if ((mpManager->param1 & 0xff) == 0) {
        int x = mCurrent0;
        int y = mCurrent1;
        Mg_RenderSpriteBoth(sSmartballBallSprites[0], (x + 0x800) >> 12, (y + 0x800) >> 12, -1, r2);
    } else {
        int x = mCurrent0;
        int y = mCurrent1;
        Mg_RenderSpriteBoth(sSmartballBallSpritesAlt[0], (x + 0x800) >> 12, (y + 0x800) >> 12, -1,
                            r2);
    }
}

/* Slot 0: the ball's frame -- expiry, freeze, goal zone, bounce and physics. */
void cMgSmartball_ball_c::SaveSnapshot() {
    V2xz vc;
    V2xz vd;
    V2xz d1;
    V2xz pa;
    V2xz pb;
    V2xz pc;
    V2xz pd;
    V2xz pe;
    V2xz pf;
    V2xz pg;
    V2xz ph;
    V2xz v5c;
    V2xz d2;
    int t;
    int spd;
    int mag;
    int dist;

    if (soundTimer > 0)
        soundTimer -= 1;
    if (mIsActive == 0)
        return;
    if (mExpireTimer > 0) {
        mExpireTimer -= 1;
        if (mExpireTimer <= 0) {
            mIsActive = 0;
            return;
        }
    }
    if (mIsFrozen == 1) {
        mCurrent0 = mFrozenPos0;
        mCurrent1 = mFrozenPos1;
        mVel0 = 0;
        mVel1 = 0;
        return;
    }
    if (mCurrent0 >= 0x8000 && mCurrent0 < 0xd8000 && mCurrent1 >= 0x74000 && mCurrent1 < 0x7c000) {
        if (mZoneDwell < 0x1e) {
            mZoneDwell += 1;
        } else if (mZoneDwell < 0x12c) {
            state3b = 1;
            mZoneDwell += 1;
        } else {
            state3b = 1;
            if (ArmExpire(1) != 0) {
                Burst();
                return;
            }
        }
    } else {
        mZoneDwell = 0;
    }
    if (mIndex == mpManager->mServeIndex) {
        if (mInPlay != 0)
            mIsWaiting = 0;
    }
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mCurrent0 < 0 || mCurrent0 >= 0x100000 || mCurrent1 < -0xf0000 || mCurrent1 >= 0xc0000) {
        mStuckFrames += 1;
    } else {
        if ((mpManager->param1 & 0xff) != 0) {
            Vec2_Sub(&d1.x, &mCurrent0, &mLastPos0);
            t = Vec2_Len((const Vector2 *)&d1) < 0x8000;
            if (t != 0) {
                mStuckFrames += 1;
                goto after2;
            }
        }
        mStuckFrames = 0;
        mLastPos0 = mCurrent0;
        mLastPos1 = mCurrent1;
    }
after2:
    if (mCollisionCooldown > 0)
        mCollisionCooldown -= 1;
    if (mZoneCooldown > 0)
        mZoneCooldown -= 1;
    if (state3a != 1) {
        if (mUpperWallSolid == 0) {
            if (mCurrent0 >= 0xe8000 && mCurrent0 < 0xf0000 && mCurrent1 >= 0x78000 &&
                mCurrent1 < 0xa8000) {
                if (mZoneCooldown <= 0) {
                    mUpperWallSolid = 1;
                    unk_039 = 0;
                    state3a = 0;
                }
            }
        }
    }
    if (state3a == 0) {
        pa.x = mCurrent0;
        pa.z = mCurrent1;
        pb.x = 0x8000;
        pb.z = 0x8c000;
        pc.x = 0x8000;
        pc.z = 0x92000;
        pd.x = 0xe0000;
        pd.z = 0x96000;
        if (PointInTriangle(&pa, &pb, &pc, &pd) != 0)
            goto bounce;
        pe.x = mCurrent0;
        pe.z = mCurrent1;
        pf.x = 0x8000;
        pf.z = 0x92000;
        pg.x = 0xe0000;
        pg.z = 0x96000;
        ph.x = 0xe0000;
        ph.z = 0x9e000;
        if (PointInTriangle(&pe, &pf, &pg, &ph) != 0) {
        bounce:
            mVel0 += 0x80;
            if (mVel0 >= 0x2000)
                mVel0 = 0x2000;
            mVel1 += 0x80;
        } else {
            mVel1 += 0x100;
        }
    }
    if (mVel0 != 0 || mVel1 != 0) {
        spd = Vec2_Len((const Vector2 *)(V2xz *)&mVel0);
        if (spd >= 0x8000)
            spd = 0x8000;
        if (func_0203d434((V2 *)&mVel0) != 0)
            func_0203d630((V2 *)&mVel0, spd);
    }
    mCurrent0 += mVel0;
    mCurrent1 += mVel1;
    if (mIsWaiting == 1) {
        QueueBehind();
    } else {
        UpdatePhysics();
        if (mExitGateOpen == 0) {
            vc.x = 0x20000;
            vc.z = -0x8000;
            func_0203d480((Fix12i *)&v5c, (Fix12i *)&vc);
            {
                int dz = mCurrent1 + 0x80000;
                int dx = mCurrent0 - 0xd8000;
                vd.x = dx;
                vd.z = dz;
            }
            if (func_0203d524((V2 *)&vc, (V2 *)&vd) < -mRadius)
                mExitGateOpen = 1;
        }
    }
    mag = Vec2_Len((const Vector2 *)(V2xz *)&mVel0);
    Vec2_Sub(&d2.x, &mCurrent0, &mSnapshot0);
    dist = Vec2_Len((const Vector2 *)&d2);
    if (specialHit != 0)
        return;
    if (mag < dist)
        return;
    if (mag < 0x800)
        return;
    if (func_0203d434((V2 *)&mVel0) == 0)
        return;
    func_0203d630((V2 *)&mVel0, dist);
}

/* Slot 2: resets the ball and every hit-test array. */
void cMgSmartball_ball_c::RestoreInitial() {
    int i;

    cMgSmartball_object_c::RestoreInitial();
    mLastPos0 = mCurrent0;
    mLastPos1 = mCurrent1;
    mVel0 = 0;
    mVel1 = 0;
    for (i = 0; i < 0x20; i++) {
        hit[i] = 0;
        hitA[i] = 0;
        hitC[i] = 0;
        hitB[i] = 0;
    }
    for (i = 0; i < 0x19; i++) {
        nearby[i] = 0;
    }
    anyHit = 0;
    specialHit = 0;
    mUpperWallSolid = 0;
    unk_039 = 0;
    mZoneCooldown = 0;
    state3a = 0;
    state3b = 0;
    mZoneDwell = 0;
    targetIndex = -1;
    mCollisionCooldown = 0;
    mIsWaiting = 1;
    mQueueGap = 0;
    mExpireTimer = 0;
    mIsFrozen = 0;
    mStuckFrames = 0;
    mPipeUsed = 0;
    mInPlay = 1;
    if ((mpManager->param1 & 0xff) && mIndex >= 9)
        mInPlay = 0;
    else
        mInPlay = 1;
    soundTimer = 0;
    soundPlayed = 0;
    mExitGateOpen = 0;
}

 /* C linkage, as in the original .c file */
cMgSmartball_ball_c *cMgSmartball_ball_c_Construct(cMgSmartball_ball_c *c, dScMgSmartball_c *b,
                                                   int d, V2 *s) {
    int tmp[2];
    tmp[0] = s->x;
    tmp[1] = s->y;
    CallObjectConstruct(c, b, d, tmp);
    c->~cMgSmartball_ball_c();
    c->mRadius = 0x8000;
    return c;
}


int cMgSmartball_object_c::PointInTriangle(int *p, int *q0, int *q1, int *q2) {
    int v0[2], v1[2], v2[2];
    int t0[2], t1[2], t2[2];
    long long d0, d1, d2;
    Vec2_Sub(t0, p, q0);
    v0[0] = t0[0];
    v0[1] = t0[1];
    Vec2_Sub(t1, p, q1);
    v1[0] = t1[0];
    v1[1] = t1[1];
    Vec2_Sub(t2, p, q2);
    v2[0] = t2[0];
    v2[1] = t2[1];
    d0 = func_0203d4d0(p, v0, v1);
    d1 = func_0203d4d0(p, v1, v2);
    d2 = func_0203d4d0(p, v2, v0);
    return (d0 >= 0 && d1 >= 0 && d2 >= 0) || (d0 <= 0 && d1 <= 0 && d2 <= 0);
}

int cMgSmartball_kinoko_c::HitRadiusSq() {
    return FX_MUL(mRadius, mRadius);
}

void cMgSmartball_object_c::SetVel(int *velocity) {
    mVel0 = velocity[0];
    mVel1 = velocity[1];
}

void CopyObjectVel(void *a, void *b) {
    int *out = (int *)a;
    cMgSmartball_object_c *object = (cMgSmartball_object_c *)b;
    out[0] = object->mVel0;
    out[1] = object->mVel1;
}


void GetObjectDelta(void *a, cMgSmartball_object_c *b) {
    Vec2_Sub((int *)a, &b->mCurrent0, &b->mSnapshot0);
}


void CopyObjectPos(void *a, void *b) {
    int *out = (int *)a;
    cMgSmartball_object_c *object = (cMgSmartball_object_c *)b;
    out[0] = object->mCurrent0;
    out[1] = object->mCurrent1;
}


/* Slot 1: empty here; every child overrides it. */
void cMgSmartball_object_c::Update() {}

/* Slot 0: stores the current position as the previous one. */
void cMgSmartball_object_c::SaveSnapshot() {
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
}

/* Restores the starting position.
   Leftover: both loads happen before both stores. */
void cMgSmartball_object_c::RestoreInitial() {
    s32 hi = mInitial1;
    s32 lo = mInitial0;
    mCurrent0 = lo;
    mCurrent1 = hi;
}

 /* C linkage, as in the original .c file */
cMgSmartball_object_c *cMgSmartball_object_c_Construct(cMgSmartball_object_c *o,
                                                       dScMgSmartball_c *a1, int a2, int *a3) {
    int lo, hi;
    *(VFunc **)o = cMgSmartball_object_c_vtbl;
    o->mpManager = a1;
    o->mIndex = a2;
    hi = a3[1];
    lo = a3[0];
    o->mInitial0 = lo;
    o->mInitial1 = hi;
    hi = a3[1];
    lo = a3[0];
    o->mCurrent0 = lo;
    o->mCurrent1 = hi;
    o->mRadius = 0;
    o->mVel0 = 0;
    o->mVel1 = 0;
    o->mIsActive = 1;
    return o;
}


/* Slot 25: forwards the graph callback to the minigame base. */
int dScMgSmartball_c::OnGraphCallback3() {
    void *t = (void *)this;
    return ((dScMgBase_c *)t)->dScMgBase_c::OnGraphCallback3() != 0;
}

int dScMgSmartball_c::SparksActive() {
    int i;
    for (i = 0; i < 0x40; i++) {
        if (mSparks[i].mActive == 1)
            return 1;
    }
    return 0;
}

/* Seeds 0x18 sparks around a point, reusing free slots before random ones. */
#define SPARK_RAND (((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff)
void dScMgSmartball_c::EmitSparks(int *pair, int a3) {
    int count = 0;
    int i;
    u32 r;

    for (i = 0; i < 0x40; i++) {
        if (mSparks[i].mActive == 1)
            continue;
        mSparks[i].mOrigin[0] = pair[0];
        mSparks[i].mOrigin[1] = pair[1];
        r = RandomIntInternal(&data_0209d4b8.value);
        mSparks[i].mPos[0] = 0;
        mSparks[i].mPos[1] = (((r >> 16) & 0x7fff) * 0x10000) >> 15;
        func_0203d388((V2 *)mSparks[i].mPos, (s16)(u16)(SPARK_RAND * 2));
        mSparks[i].mPos[1] *= 2;
        mSparks[i].mPos[0] += mSparks[i].mOrigin[0];
        mSparks[i].mPos[1] += mSparks[i].mOrigin[1];
        mSparks[i].mVel[0] = 0;
        mSparks[i].mVel[1] = 0x10;
        mSparks[i].mActive = 1;
        mSparks[i].mFrame = (SPARK_RAND * 0x10) >> 15;
        mSparks[i].mBursts = ((SPARK_RAND * 3) >> 15) + 5;
        count++;
        mSparks[i].mAlt = a3;
        if (count >= 0x18)
            return;
    }

    while (count < 0x18) {
        i = SPARK_RAND * 0x40 >> 15;
        mSparks[i].mOrigin[0] = pair[0];
        mSparks[i].mOrigin[1] = pair[1];
        r = RandomIntInternal(&data_0209d4b8.value);
        mSparks[i].mPos[0] = 0;
        mSparks[i].mPos[1] = (((r >> 16) & 0x7fff) * 0xc000) >> 15;
        func_0203d388((V2 *)mSparks[i].mPos, (s16)(u16)(SPARK_RAND * 2));
        mSparks[i].mPos[1] *= 2;
        mSparks[i].mPos[0] += mSparks[i].mOrigin[0];
        mSparks[i].mPos[1] += mSparks[i].mOrigin[1];
        mSparks[i].mVel[0] = 0;
        mSparks[i].mVel[1] = 0x10;
        mSparks[i].mActive = 1;
        mSparks[i].mFrame = (SPARK_RAND * 0x10) >> 15;
        mSparks[i].mBursts = ((SPARK_RAND * 3) >> 15) + 5;
        count++;
        mSparks[i].mAlt = a3;
    }
}

void dScMgSmartball_c::RenderSparks() {
    int i;
    for (i = 0; i < 0x40; i++) {
        if (mSparks[i].mActive != 0) {
            if ((param1 & 0xff) == 0) {
                int idx = mSparks[i].mFrame / 4 + 8;
                if (mSparks[i].mAlt != 0)
                    idx += 4;
                Mg_RenderSpriteBoth(sSmartballSparkSprites[idx], mSparks[i].mPos[0] >> 12,
                                    mSparks[i].mPos[1] >> 12, -1, 0);
            } else {
                int idx = mSparks[i].mFrame / 4 + 4;
                if (mSparks[i].mAlt != 0)
                    idx += 4;
                Mg_RenderSpriteBoth(sSmartballSparkSpritesAlt[idx], mSparks[i].mPos[0] >> 12,
                                    mSparks[i].mPos[1] >> 12, -1, 0);
            }
        }
    }
}

void dScMgSmartball_c::UpdateSparks() {
    int i;
    u32 r;
    int len;

    for (i = 0; i < 0x40; i++) {
        if (mSparks[i].mActive) {
            mSparks[i].mPos[0] += mSparks[i].mVel[0];
            mSparks[i].mPos[1] += mSparks[i].mVel[1];
            len = Vec2_Len((const Vector2 *)mSparks[i].mVel);
            len = len * 7 / 8;
            if (func_0203d434((V2 *)mSparks[i].mVel))
                func_0203d630((V2 *)mSparks[i].mVel, len);
            mSparks[i].mFrame++;
            if (mSparks[i].mFrame >= 0x10) {
                mSparks[i].mFrame = 0;
                mSparks[i].mBursts--;
                if (mSparks[i].mBursts <= 0) {
                    mSparks[i].mActive = 0;
                } else {
                    r = RandomIntInternal(&data_0209d4b8.value);
                    mSparks[i].mPos[0] = 0;
                    mSparks[i].mPos[1] = (((r >> 16) & 0x7fff) * 0x10000) >> 15;
                    func_0203d388((V2 *)mSparks[i].mPos, (s16)(u16)(SPARK_RAND * 2));
                    mSparks[i].mPos[1] *= 2;
                    mSparks[i].mPos[0] += mSparks[i].mOrigin[0];
                    mSparks[i].mPos[1] += mSparks[i].mOrigin[1];
                    mSparks[i].mVel[0] = 0;
                    mSparks[i].mVel[1] = 0x10;
                }
            }
        }
    }
}

void dScMgSmartball_c::ResetSparks() {
    int i;
    for (i = 0; i < 0x40; i++) {
        mSparks[i].mActive = 0;
        mSparks[i].mBursts = 0;
        mSparks[i].mFrame = 0;
    }
}

void dScMgSmartball_c::EndRound() {
    int i;
    mDrainTimer = 0;
    mDraining = 1;
    mRoundEnded = 1;
    Mg_ShowBanner(6, 0x80, -0x60, -1, -1, 0xd);
    Mg_PlayJingle(0);
    for (i = 0; i < mBallCount; i++) {
        if (((i >= 0xd) ? 0 : mpBalls[i])->mIsActive)
            ((i >= 0xd) ? 0 : mpBalls[i])->Freeze();
    }
}

static inline cMgSmartball_ball_c *get(dScMgSmartball_c *self, int i) {
    return i >= 13 ? 0 : self->mpBalls[i];
}
void dScMgSmartball_c::ServeNextBall() {
    int t[3];
    int n = mBallCount;
    int i;
    for (i = 0; i < n; i++) {
        if (get(this, i)->mIsActive == 0)
            continue;
        if (get(this, i)->mInPlay != 0)
            continue;
        get(this, i)->PutInPlay();
        CopyObjectPos(t, get(this, i));
        func_02012718(0x1a5, t[0]);
        return;
    }
}

void dScMgSmartball_c::CountOut() {
    mOutTally++;
}

void dScMgSmartball_c::CountAward(int i) {
    mAwardTally[i]++;
}

void dScMgSmartball_c::CountDokan() {
    mDokanTally++;
}

void dScMgSmartball_c::CountAna() {
    mAnaTally++;
}

void dScMgSmartball_c::CountKinoko() {
    mKinokoTally++;
}

void dScMgSmartball_c::CountBingo() {
    mBingoTally++;
}

int dScMgSmartball_c::CountLine(int i) {
    int *arr = mLineTally;
    int v = arr[i - 1] + 1;
    arr[i - 1] = v;
    return v;
}

void dScMgSmartball_c::CountCell() {
    mCellTally++;
}

unsigned short dScMgSmartball_c::GetTile(int *pos) {
    unsigned char *base = (unsigned char *)this;
    int a = pos[1];
    if (a >= 0 && a < 0xc0000) {
        int b = pos[0];
        int ai = (a >> 12) / 8;
        int bi = (b >> 12) / 8;
        unsigned short h = mBg2Map[ai * 32 + bi];
        return (unsigned short)(h & 0x3ff);
    }
    if (a < -0x30000 && a >= -0xf0000) {
        int b = pos[0];
        int ai = ((a >> 12) + 0xf0) / 8;
        int bi = (b >> 12) / 8;
        unsigned short h = mBg1Map[ai * 32 + bi];
        return (unsigned short)(h & 0x3ff);
    }
    return 0;
}

#pragma push
#pragma opt_strength_reduction off
/* Leftover: UpdateDust needs strength reduction off. */
void dScMgSmartball_c::UpdateDust() {
    char *self = (char *)this;
    int i;
    char *obj = (char *)mDustVel;
    for (i = 0; i < 0x10; i++) {
        unsigned char *flag = (unsigned char *)((dScMgSmartball_c *)(self + i))->mDustActive;
        if (*flag != 0) {
            dScMgSmartball_c *entry = (dScMgSmartball_c *)(self + i * 8);
            *(int *)entry->mDustPos += *(int *)entry->mDustVel;
            *(int *)&entry->mDustPos[4] += *(int *)&entry->mDustVel[4];
            {
                int m = Vec2_Len((const Vector2 *)obj) * 6 / 8;
                if (func_0203d434((V2 *)obj)) {
                    func_0203d630((V2 *)obj, m);
                }
            }
            {
                int *counter = ((dScMgSmartball_c *)(self + i * 4))->mDustAge;
                *counter += 1;
                if (*counter >= 0xf) {
                    *flag = 0;
                    *counter = 0;
                }
            }
        }
        obj += 8;
    }
}

#pragma pop

/* Spawns up to four dust puffs at a point.
 * Leftover: TVec2's empty destructor keeps `v` in memory, and `sa` must be its
 * own assignment before the first product. */
void dScMgSmartball_c::SpawnDust(int *origin) {
    char *c = (char *)this;
    int i;
    int j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 0x10; j++) {
            u8 *flag = ((dScMgSmartball_c *)(c + j))->mDustActive;
            if (*flag == 1) {
                continue;
            }
            {
                int rnd1 = RandomIntInternal(&data_0209d4b8.value);
                int rnd2 = RandomIntInternal(&data_0209d4b8.value);
                int angB = ((((unsigned int)rnd2 >> 16) & 0x7fff) * 0x8000) >> 16;
                int angA = ((((unsigned int)rnd1 >> 16) & 0x7fff) * 0x20000) >> 16;
                s16 sinB = FX_SinCosTable_[(angB >> 4) * 2 + 1];
                s16 sinA = FX_SinCosTable_[(angA >> 4) * 2 + 1];
                s16 cosA;
                s64 sa;
                TVec2 v;
                sa = (s64)sinA;
                v.x = FX_MUL(sinB, FX_MUL(sinA, 0x8000));
                *(int *)((dScMgSmartball_c *)(c + j * 8))->mDustPos = v.x;
                cosA = FX_SinCosTable_[(angA >> 4) * 2];
                v.y = FX_MUL(sinB, FX_MUL(cosA, 0x8000));
                *(int *)&((dScMgSmartball_c *)(c + j * 8))->mDustPos[4] = v.y;
                *(int *)((dScMgSmartball_c *)(c + j * 8))->mDustPos += origin[0];
                *(int *)&((dScMgSmartball_c *)(c + j * 8))->mDustPos[4] += origin[1];
                *(int *)((dScMgSmartball_c *)(c + j * 8))->mDustVel =
                    FX_MUL(sinB, FX_MUL(sa, 0x4000));
                *(int *)&((dScMgSmartball_c *)(c + j * 8))->mDustVel[4] =
                    FX_MUL(sinB, FX_MUL(cosA, 0x4000));
                mDustAge[j] = 0;
                *flag = 1;
            }
            break;
        }
    }
}

#pragma push
#pragma opt_strength_reduction off
/* Leftover: UpdateScores needs strength reduction off. */
void dScMgSmartball_c::UpdateScores() {
    char *o = (char *)this;
    int i;
    for (i = 0; i < 5; i++) {
        int *score = ((dScMgSmartball_c *)(o + i * 4))->mPopupScore;
        int *delay = ((dScMgSmartball_c *)(o + i * 4))->mPopupDelay;
        dScMgSmartball_c *popup = (dScMgSmartball_c *)(o + i * 8);
        int *posX = (int *)popup->mPopupPos;
        int *posY = (int *)&popup->mPopupPos[4];
        int *multiplier = ((dScMgSmartball_c *)(o + i * 4))->mPopupMultiplier;
        int *age = ((dScMgSmartball_c *)(o + i * 4))->mPopupAge;
        if (*score > 0) {
            if (*delay > 0) {
                *delay -= 1;
                if (*delay <= 0) {
                    int tmp[2];
                    tmp[0] = *posX;
                    tmp[1] = *posY;
                    EmitScore((int *)tmp, *score, 0, *multiplier);
                    continue;
                }
            }
            *age += 1;
            if ((param1 & 0xff) == 0)
                *posY -= 0x1000;
            else
                *posY -= 0x800;
            if (*age > 0x3c) {
                *score = 0;
                *age = 0;
            }
        }
    }
}

#pragma pop

void dScMgSmartball_c::EmitScore(int *src, int v2, int v3, int v5) {
    int i;
    for (i = 0; i < 5; i++) {
        if (mPopupScore[i] > 0)
            continue;
        if (v3 > 0) {
            mPopupDelay[i] = v3;
        } else {
            Mg_SubmitScore(v2 + Mg_GetSubmittedScore());
            if ((unsigned int)Mg_GetSubmittedScore() > 0xf423f) {
                Mg_SubmitScore(0xf423f);
            }
        }
        mPopupScore[i] = v2;
        mPopupMultiplier[i] = v5;
        mPopupAge[i] = 0;
        ((int(*)[2])mPopupPos)[i][0] = src[0];
        ((int(*)[2])mPopupPos)[i][1] = src[1];
        return;
    }
    Mg_SubmitScore(v2 * v5 + Mg_GetSubmittedScore());
    if ((unsigned int)Mg_GetSubmittedScore() > 0xf423f) {
        Mg_SubmitScore(0xf423f);
    }
}

void dScMgSmartball_c::KickGate(int idx) {
    int i;
    if (mGateLampTimer[idx] == 0) {
        i = 0;
        if (mPropellerCount > 0) {
            do {
                cMgSmartball_propeller_c *x;
                if (i >= mPropellerCount)
                    x = 0;
                else
                    x = mpPropellers[i];
                i++;
                x->mSpinSpeedTarget = -x->mSpinSpeedTarget;
            } while (i < mPropellerCount);
        }
    }
    mGateLampTimer[idx] = 0x14;
}

int dScMgSmartball_c::IsTableAtRest() {
    int i;

    for (i = 0; i < mBallCount; i++) {
        if (((i >= 0xd) ? 0 : mpBalls[i])->mIsActive != 0) {
            if (((i >= 0xd) ? 0 : mpBalls[i])->IsFinished() == 0)
                return 0;
        }
    }
    if (mDraining != 0)
        return 0;
    if (mpSlot != 0) {
        if ((mpSlot->mSpinCooldown > 0 ? 0 : mpSlot->mIsSpinning) != 0)
            return 0;
    }
    if (mpBoard != 0) {
        if (mpBoard->AnyLineActive() != 0)
            return 0;
    }
    if (mpWing != 0) {
        if (mpWing->IsSettled() == 0)
            return 0;
        mpWing->mAngleSettled = 1;
    }
    if (SparksActive() != 0)
        return 0;
    return 1;
}

#pragma push
#pragma opt_common_subs off
/* Leftover: ResolveBallPair needs common subexpression folding off. */
void dScMgSmartball_c::ResolveBallPair(int firstIndex, int secondIndex, u8 *firstObject,
                                       u8 *secondObject) {
    // Leftover: keep the pair materialized in memory; direct casts keep the
    // register assignment.
    TVec2 delta;
    V2 firstPos;
    V2 secondPos;
    V2 firstVelocity;
    V2 secondVelocity;
    V2 firstZeroVelocity;
    V2 secondZeroVelocity;
    V2 positionDelta;
    V2 firstSavedVelocity;
    V2 secondSavedVelocity;
    V2 firstResult;
    V2 secondResult;
    s16 collisionAngle;
    int inverseAngle;

    if (firstIndex == secondIndex) {
        return;
    }
    if (((cMgSmartball_ball_c *)firstObject)->targetIndex != secondIndex) {
        return;
    }
    if (firstIndex > secondIndex) {
        if (((cMgSmartball_ball_c *)secondObject)->targetIndex == firstIndex) {
            return;
        }
    }
    if (((cMgSmartball_ball_c *)firstObject)->mIsFrozen == 1 ||
        ((cMgSmartball_ball_c *)secondObject)->mIsFrozen == 1) {
        firstZeroVelocity.x = 0;
        firstZeroVelocity.y = 0;
        ((cMgSmartball_ball_c *)firstObject)->SetVel(&firstZeroVelocity.x);
        secondZeroVelocity.x = 0;
        secondZeroVelocity.y = 0;
        ((cMgSmartball_ball_c *)secondObject)->SetVel(&secondZeroVelocity.x);
        ((cMgSmartball_ball_c *)firstObject)->state3b = 1;
        ((cMgSmartball_ball_c *)secondObject)->state3b = 1;
        return;
    }
    if (((cMgSmartball_ball_c *)firstObject)->mCollisionCooldown > 0 ||
        ((cMgSmartball_ball_c *)secondObject)->mCollisionCooldown > 0) {
        ((cMgSmartball_ball_c *)firstObject)->mCollisionCooldown = 3;
        ((cMgSmartball_ball_c *)secondObject)->mCollisionCooldown = 3;
        return;
    }
    ((cMgSmartball_ball_c *)firstObject)->mCollisionCooldown = 3;
    ((cMgSmartball_ball_c *)secondObject)->mCollisionCooldown = 3;
    CopyObjectPos(&firstPos.x, firstObject);
    CopyObjectPos(&secondPos.x, secondObject);
    Vec2_Sub(&positionDelta.x, &firstPos.x, &secondPos.x);
    delta.x = positionDelta.x;
    delta.y = positionDelta.y;
    collisionAngle = cstd::atan2(delta.y, delta.x);
    if (((cMgSmartball_ball_c *)firstObject)->HasHit() != 0) {
        firstVelocity.x = 0;
        firstVelocity.y = 0;
    } else {
        CopyObjectVel(&firstSavedVelocity, firstObject);
        firstVelocity = firstSavedVelocity;
    }
    if (((cMgSmartball_ball_c *)secondObject)->HasHit() != 0) {
        secondVelocity.x = 0;
        secondVelocity.y = 0;
    } else {
        CopyObjectVel(&secondSavedVelocity, secondObject);
        secondVelocity = secondSavedVelocity;
    }
    // Rotate into the ball-to-ball line, swap the shared component, rotate back.
    inverseAngle = -collisionAngle;
    func_0203d388(&firstVelocity, (s16)inverseAngle);
    func_0203d388(&secondVelocity, (s16)inverseAngle);
    {
        int normalVelocity = firstVelocity.x;
        firstVelocity.x = secondVelocity.x;
        secondVelocity.x = normalVelocity;
    }
    func_0203d388(&firstVelocity, collisionAngle);
    func_0203d388(&secondVelocity, collisionAngle);
    firstResult = firstVelocity;
    ((cMgSmartball_ball_c *)firstObject)->SetVel(&firstResult.x);
    secondResult = secondVelocity;
    ((cMgSmartball_ball_c *)secondObject)->SetVel(&secondResult.x);
}

#pragma pop

void dScMgSmartball_c::CollideBalls() {
    int ballCount = mBallCount;
    int firstIndex, secondIndex;
    for (firstIndex = 0; firstIndex < ballCount; firstIndex++) {
        for (secondIndex = 0; secondIndex < ballCount; secondIndex++) {
            cMgSmartball_ball_c *firstBall = (firstIndex >= 13) ? 0 : mpBalls[firstIndex];
            cMgSmartball_ball_c *secondBall = (secondIndex >= 13) ? 0 : mpBalls[secondIndex];
            ResolveBallPair(firstIndex, secondIndex, (u8 *)firstBall, (u8 *)secondBall);
            ballCount = mBallCount;
        }
    }
}

#pragma push
#pragma opt_strength_reduction off
/* Tears the whole table down and rebuilds it from the two tile maps.
 * Leftovers: the vtable stores, the spawn counters and the scan stores are
 * volatile, and every slot walk steps `this` instead of indexing. */
void dScMgSmartball_c::SpawnObjects() {
    char *c = (char *)this;
    V2 pos[25];

    Mg_SubmitScore(0);

    for (int i = 0; i < 0xd; i++) {
        cMgSmartball_ball_c **slot = ((cMgSmartball_ball_c **)this) + i + 0x4688 / 4;
        cMgSmartball_ball_c *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(VFunc *volatile *)p = cMgSmartball_ball_c_vtbl;
                *(VFunc *volatile *)p = cMgSmartball_object_c_vtbl;
                ::operator delete(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 8; i++) {
        cMgSmartball_ana_c **slot = ((cMgSmartball_ana_c **)this) + i + 0x4720 / 4;
        cMgSmartball_ana_c *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(VFunc *volatile *)p = cMgSmartball_ana_c_vtbl;
                *(VFunc *volatile *)p = cMgSmartball_object_c_vtbl;
                ::operator delete(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 0x19; i++) {
        cMgSmartball_kinoko_c **slot = ((cMgSmartball_kinoko_c **)this) + i + 0x46bc / 4;
        cMgSmartball_kinoko_c *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(VFunc *volatile *)p = cMgSmartball_kinoko_c_vtbl;
                *(VFunc *volatile *)p = cMgSmartball_object_c_vtbl;
                ::operator delete(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 3; i++) {
        cMgSmartball_propeller_c **slot = ((cMgSmartball_propeller_c **)this) + i + 0x4740 / 4;
        cMgSmartball_propeller_c *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(VFunc *volatile *)p = cMgSmartball_propeller_c_vtbl;
                *(VFunc *volatile *)p = cMgSmartball_object_c_vtbl;
                ::operator delete(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 6; i++) {
        cMgSmartball_dokan_c **slot = ((cMgSmartball_dokan_c **)this) + i + 0x474c / 4;
        cMgSmartball_dokan_c *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(VFunc *volatile *)p = cMgSmartball_dokan_c_vtbl;
                *(VFunc *volatile *)p = cMgSmartball_object_c_vtbl;
                ::operator delete(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 3; i++) {
        cMgSmartball_pakkun_c **slot = ((cMgSmartball_pakkun_c **)this) + i + 0x4764 / 4;
        cMgSmartball_pakkun_c *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(VFunc *volatile *)p = cMgSmartball_pakkun_c_vtbl;
                *(VFunc *volatile *)p = cMgSmartball_object_c_vtbl;
                ::operator delete(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 2; i++) {
        cMgSmartball_pushswitch_c **slot = ((cMgSmartball_pushswitch_c **)this) + i + 0x4770 / 4;
        cMgSmartball_pushswitch_c *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(VFunc *volatile *)p = cMgSmartball_pushswitch_c_vtbl;
                *(VFunc *volatile *)p = cMgSmartball_object_c_vtbl;
                ::operator delete(p);
            }
            *slot = 0;
        }
    }

    {
        cMgSmartball_slot_c *p = mpSlot;
        if (p != 0) {
            if (p != 0) {
                *(VFunc **)p = cMgSmartball_slot_c_vtbl;
                __cxa_vec_cleanup(p->mReelVel, 3, 8, (dtor_t)NullDestructor_0203d47c);
                __cxa_vec_cleanup(p->mReelOffset, 3, 8, (dtor_t)NullDestructor_0203d47c);
                *(VFunc *volatile *)p = cMgSmartball_object_c_vtbl;
                ::operator delete(p);
            }
            mpSlot = 0;
        }
    }
    {
        cMgSmartball_board_c *p = mpBoard;
        if (p != 0) {
            if (p != 0) {
                *(VFunc **)p = cMgSmartball_board_c_vtbl;
                *(VFunc *volatile *)p = cMgSmartball_object_c_vtbl;
                ::operator delete(p);
            }
            mpBoard = 0;
        }
    }
    {
        cMgSmartball_wing_c *p = mpWing;
        if (p != 0) {
            if (p != 0) {
                *(VFunc **)p = cMgSmartball_wing_c_vtbl;
                *(VFunc *volatile *)p = cMgSmartball_object_c_vtbl;
                ::operator delete(p);
            }
            mpWing = 0;
        }
    }

    Mg_SetTopScreenYOffset(0x30);

    if (GetOwnerLanguage() == 5) {
        OS_SPrintf(sSmartballPathBuf, sSmartballLayoutNcgFmtSpanish, (int)mLevel & 1);
    } else if (GetOwnerLanguage() == 4) {
        OS_SPrintf(sSmartballPathBuf, sSmartballLayoutNcgFmtItalian, (int)mLevel & 1);
    } else if (GetOwnerLanguage() == 3) {
        OS_SPrintf(sSmartballPathBuf, sSmartballLayoutNcgFmtGerman, (int)mLevel & 1);
    } else if (GetOwnerLanguage() == 2) {
        OS_SPrintf(sSmartballPathBuf, sSmartballLayoutNcgFmtFrench, (int)mLevel & 1);
    } else if (((int)param1 & 0xff) == 0) {
        OS_SPrintf(sSmartballPathBuf, sSmartballLayoutNcgFmtE, (int)mLevel & 1);
    } else {
        OS_SPrintf(sSmartballPathBuf, sSmartballLayoutNcgFmt, (int)mLevel & 1);
    }
    {
        void *d = Ov004_LoadFileByPath(sSmartballPathBuf);
        DecompressLZ16(d, G2_GetOBJCharPtr());
        DecompressLZ16(d, G2S_GetOBJCharPtr());
        Ov004_Deallocate(d);
    }
    OS_SPrintf(sSmartballPathBuf, sSmartballBg1NscFmt, (int)mLevel & 1);
    {
        void *d = Ov004_LoadFileByPath(sSmartballPathBuf);
        DecompressLZ16(d, mBg1Map);
        Ov004_Deallocate(d);
    }
    OS_SPrintf(sSmartballPathBuf, sSmartballBg2NscFmt, (int)mLevel & 1);
    {
        void *d = Ov004_LoadFileByPath(sSmartballPathBuf);
        DecompressLZ16(d, mBg2Map);
        Ov004_Deallocate(d);
    }
    if (((int)param1 & 0xff) == 0) {
        void *d = Ov004_LoadFileByPath((const char *)sSmartballLay0Bg1FrameNscPath);
        DecompressLZ16(d, G2_GetBG2ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath((const char *)sSmartballLay0Bg2FrameNscPath);
        DecompressLZ16(d, G2S_GetBG2ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath((const char *)sSmartballLay0Bg1NscPath);
        DecompressLZ16(d, G2_GetBG3ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath((const char *)sSmartballLay0Bg2NscPath);
        DecompressLZ16(d, G2S_GetBG3ScrPtr());
        Ov004_Deallocate(d);
    } else {
        void *d = Ov004_LoadFileByPath((const char *)sSmartballLay1Bg1FrameNscPath);
        DecompressLZ16(d, G2_GetBG2ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath((const char *)sSmartballLay1Bg2FrameNscPath);
        DecompressLZ16(d, G2S_GetBG2ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath((const char *)sSmartballLay1Bg1NscPath);
        DecompressLZ16(d, G2_GetBG3ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath((const char *)sSmartballLay1Bg2NscPath);
        DecompressLZ16(d, G2S_GetBG3ScrPtr());
        Ov004_Deallocate(d);
    }

    mBallCount = 0;
    mAnaCount = 0;
    mKinokoCount = 0;
    mDokanCount = 0;
    mPakkunCount = 0;
    mPropellerCount = 0;
    mPushSwitchCount = 0;

    __cxa_vec_ctor(pos, 0x19, 8, (ctor_t)func_0203d738, (dtor_t)NullDestructor_0203d47c);

    {
        int mask = 0x3ff;
        for (int i = 0; i < 0x300; i++) {
            int k = mKinokoCount;
            if (k >= 0x19)
                break;
            if ((mBg1Map[i] & mask) == 0x2e8) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                V2 &pp = pos[k];
                pp.x = x;
                pp.y = y << 0xc;
                mKinokoCount++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = mKinokoCount;
            if (k >= 0x19)
                break;
            if ((mBg2Map[i] & mask) == 0x2e8) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                V2 &pp = pos[k];
                pp.x = x;
                pp.y = y << 0xf;
                mKinokoCount++;
            }
        }
    }
    {
        int n = mKinokoCount;
        int j;
        for (j = 0; j < n; j++) {
            cMgSmartball_kinoko_c *spawned = (cMgSmartball_kinoko_c *)::operator new(0x40);
            if (spawned != 0) {
                V2 tmp = pos[j];
                spawned = cMgSmartball_kinoko_c_Construct(spawned, this, j, &tmp);
            }
            {
                cMgSmartball_kinoko_c **slot = ((cMgSmartball_kinoko_c **)this) + j + 0x46bc / 4;
                *slot = spawned;
                if (*slot != 0)
                    (*slot)->mVariant = 0;
            }
        }
        {
            int mask = 0x3ff;
            for (int i = 0; i < 0x300; i++) {
                int k = mKinokoCount;
                if (k >= 0x19)
                    break;
                if ((mBg1Map[i] & mask) == 0x328) {
                    int x = (i % 0x20) << 0xf;
                    int y = (i / 0x20) * 8 - 0xf0;
                    V2 &pp = pos[k];
                    pp.x = x;
                    pp.y = y << 0xc;
                    mKinokoCount++;
                }
            }
            for (int i = 0; i < 0x300; i++) {
                int k = mKinokoCount;
                if (k >= 0x19)
                    break;
                if ((mBg2Map[i] & mask) == 0x328) {
                    int x = (i % 0x20) << 0xf;
                    int y = i / 0x20;
                    V2 &pp = pos[k];
                    pp.x = x;
                    pp.y = y << 0xf;
                    mKinokoCount++;
                }
            }
        }
        for (; n < mKinokoCount; n++) {
            cMgSmartball_kinoko_c *spawned = (cMgSmartball_kinoko_c *)::operator new(0x40);
            if (spawned != 0) {
                V2 tmp;
                int ty = pos[n].y;
                int tx = pos[n].x;
                tmp.x = tx;
                tmp.y = ty;
                spawned = cMgSmartball_kinoko_c_Construct(spawned, this, n, &tmp);
            }
            {
                cMgSmartball_kinoko_c **slot = ((cMgSmartball_kinoko_c **)this) + n + 0x46bc / 4;
                *slot = spawned;
                if (*slot != 0)
                    (*slot)->mVariant = 1;
            }
        }
    }

    {
        int mask = 0x3ff;
        int tile = 0x2cb;
        for (int i = 0; i < 0x300; i++) {
            int k = mAnaCount;
            if (k >= 8)
                break;
            if ((mBg1Map[i] & mask) == tile) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                V2 &pp = pos[k];
                pp.x = x;
                pp.y = y << 0xc;
                mAnaCount++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = mAnaCount;
            if (k >= 8)
                break;
            if ((mBg2Map[i] & mask) == tile) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                V2 &pp = pos[k];
                pp.x = x;
                pp.y = y << 0xf;
                mAnaCount++;
            }
        }
        {
            int n = mAnaCount;
            int j;
            for (j = 0; j < n; j++) {
                cMgSmartball_ana_c *spawned = (cMgSmartball_ana_c *)::operator new(0x38);
                if (spawned != 0) {
                    V2 tmp = pos[j];
                    spawned = cMgSmartball_ana_c_Construct(spawned, this, j, &tmp);
                }
                mpAnas[j] = spawned;
                n = mAnaCount;
            }
        }
    }

    if (((int)param1 & 0xff) == 0) {
        mBallCount = 0xc;
    } else {
        mBallCount = 0xd;
    }
    {
        int j;
        for (j = 0; j < mBallCount; j++) {
            cMgSmartball_ball_c *spawned;
            pos[j].x = ((1 - j) << 0x10) + 0xc0000;
            pos[j].y = FX_Div(FX_MUL(pos[j].x, 0xa000), 0xd8000) + 0x90000;
            spawned = (cMgSmartball_ball_c *)::operator new(0x12c);
            if (spawned != 0) {
                V2 tmp = pos[j];
                spawned = cMgSmartball_ball_c_Construct(spawned, this, j, &tmp);
            }
            mpBalls[j] = spawned;
        }
    }

    {
        int mask = 0x3ff;
        int tile = 0x3d2;
        int zero = 0;
        for (int i = 0; i < 0x300; i++) {
            u16 raw;
            int k = mDokanCount;
            if (k >= 6)
                break;
            raw = mBg1Map[i];
            if ((raw & mask) == tile && (raw & 0x800) == 0) {
                int x = (i % 0x20) << 0xf;
                int y = zero;
                pos[k].x = x;
                pos[k].y = y;
                mDokanCount++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            u16 raw;
            int k = mDokanCount;
            if (k >= 6)
                break;
            raw = mBg2Map[i];
            if ((raw & mask) == tile && (raw & 0x800) == 0) {
                int x = (i % 0x20) << 0xf;
                int y = zero;
                pos[k].x = x;
                pos[k].y = y;
                mDokanCount++;
            }
        }
        {
            int n = mDokanCount;
            int j;
            for (j = 0; j < n; j++) {
                cMgSmartball_dokan_c *spawned = (cMgSmartball_dokan_c *)::operator new(0x34);
                if (spawned != 0) {
                    V2 tmp = pos[j];
                    spawned = cMgSmartball_dokan_c_Construct(spawned, this, j, &tmp);
                }
                mpDokans[j] = spawned;
                n = mDokanCount;
            }
        }
    }

    {
        int mask = 0x3ff;
        int tile = 0x292;
        for (int i = 0; i < 0x300; i++) {
            int k = mPakkunCount;
            if (k >= 3)
                break;
            if ((mBg1Map[i] & mask) == tile) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                V2 &pp = pos[k];
                pp.x = x;
                pp.y = y << 0xc;
                mPakkunCount++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = mPakkunCount;
            if (k >= 3)
                break;
            if ((mBg2Map[i] & mask) == tile) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                V2 &pp = pos[k];
                pp.x = x;
                pp.y = y << 0xf;
                mPakkunCount++;
            }
        }
        {
            int n = mPakkunCount;
            int j;
            for (j = 0; j < n; j++) {
                cMgSmartball_pakkun_c *spawned = (cMgSmartball_pakkun_c *)::operator new(0x3c);
                if (spawned != 0) {
                    V2 tmp = pos[j];
                    spawned = cMgSmartball_pakkun_c_Construct(spawned, this, j, &tmp);
                }
                mpPakkuns[j] = spawned;
                n = mPakkunCount;
            }
        }
    }

    {
        int mask = 0x3ff;
        for (int i = 0; i < 0x300; i++) {
            int k = mPropellerCount;
            if (k >= 3)
                break;
            if ((mBg1Map[i] & mask) == 0x84) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                V2 &pp = pos[k];
                pp.x = x;
                pp.y = y << 0xc;
                mPropellerCount++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = mPropellerCount;
            if (k >= 3)
                break;
            if ((mBg2Map[i] & mask) == 0x84) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                V2 &pp = pos[k];
                pp.x = x;
                pp.y = y << 0xf;
                mPropellerCount++;
            }
        }
        {
            int n = mPropellerCount;
            int j;
            for (j = 0; j < n; j++) {
                cMgSmartball_propeller_c *spawned =
                    (cMgSmartball_propeller_c *)::operator new(0x78);
                if (spawned != 0) {
                    V2 tmp = pos[j];
                    spawned = cMgSmartball_propeller_c_Construct(spawned, this, j, &tmp);
                }
                mpPropellers[j] = spawned;
                n = mPropellerCount;
            }
        }
    }

    {
        int k = 0;
        int mask = 0x3ff;
        int j;
        for (j = 0; j < 0x300; j++) {
            if (k >= 1)
                break;
            if ((mBg1Map[j] & mask) == 0x57) {
                int x = (j % 0x20) << 0xf;
                int y = ((j / 0x20) * 8 - 0xf0) << 0xc;
                pos[k].x = x;
                pos[k].y = y;
                k++;
            }
        }
        {
            int mask2 = 0x3ff;
            for (j = 0; j < 0x300; j++) {
                if (k >= 1)
                    break;
                if ((mBg2Map[j] & mask2) == 0x57) {
                    int x = (j % 0x20) << 0xf;
                    int y = (j / 0x20) << 0xf;
                    pos[k].x = x;
                    pos[k].y = y;
                    k++;
                }
            }
        }
        if (k >= 1) {
            cMgSmartball_slot_c *spawned = (cMgSmartball_slot_c *)::operator new(0x88);
            if (spawned != 0) {
                V2 tmp = pos[0];
                spawned = cMgSmartball_slot_c_Construct(spawned, this, 0, &tmp);
            }
            mpSlot = spawned;
        }
    }

    {
        int k = 0;
        int mask = 0x3ff;
        int j;
        for (j = 0; j < 0x300; j++) {
            if (k >= 9)
                break;
            if ((mBg1Map[j] & mask) == 0x4a) {
                int x = (j % 0x20) << 0xf;
                int y = ((j / 0x20) * 8 - 0xf0) << 0xc;
                pos[k].x = x;
                pos[k].y = y;
                k++;
            }
        }
        {
            int mask2 = 0x3ff;
            for (j = 0; j < 0x300; j++) {
                if (k >= 9)
                    break;
                if ((mBg2Map[j] & mask2) == 0x4a) {
                    int x = (j % 0x20) << 0xf;
                    int y = (j / 0x20) << 0xf;
                    pos[k].x = x;
                    pos[k].y = y;
                    k++;
                }
            }
        }
        if (k >= 9) {
            cMgSmartball_board_c *spawned = (cMgSmartball_board_c *)::operator new(0x9c);
            if (spawned != 0) {
                V2 tmp = pos[4];
                spawned = cMgSmartball_board_c_Construct(spawned, this, 0, &tmp);
            }
            mpBoard = spawned;
        }
    }

    {
        int mask = 0x3ff;
        for (int i = 0; i < 0x300; i++) {
            int k = mPushSwitchCount;
            if (k >= 2)
                break;
            if ((mBg1Map[i] & mask) == 0x50) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                V2 &pp = pos[k];
                pp.x = x;
                pp.y = y << 0xc;
                mPushSwitchCount++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = mPushSwitchCount;
            if (k >= 2)
                break;
            if ((mBg2Map[i] & mask) == 0x50) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                V2 &pp = pos[k];
                pp.x = x;
                pp.y = y << 0xf;
                mPushSwitchCount++;
            }
        }
        {
            int n = mPushSwitchCount;
            int j;
            for (j = 0; j < n; j++) {
                cMgSmartball_pushswitch_c *spawned =
                    (cMgSmartball_pushswitch_c *)::operator new(0x38);
                if (spawned != 0) {
                    V2 tmp = pos[j];
                    spawned = cMgSmartball_pushswitch_c_Construct(spawned, this, j, &tmp);
                }
                mpPushSwitches[j] = spawned;
                n = mPushSwitchCount;
            }
        }
    }

    {
        int k = 0;
        int mask = 0x3ff;
        int j;
        for (j = 0; j < 0x300; j++) {
            if (k >= 1)
                break;
            if ((mBg1Map[j] & mask) == 0xab) {
                int x = (j % 0x20) << 0xf;
                int y = ((j / 0x20) * 8 - 0xf0) << 0xc;
                pos[k].x = x;
                pos[k].y = y;
                k++;
            }
        }
        {
            int mask2 = 0x3ff;
            for (j = 0; j < 0x300; j++) {
                if (k >= 1)
                    break;
                if ((mBg2Map[j] & mask2) == 0xab) {
                    int x = (j % 0x20) << 0xf;
                    int y = (j / 0x20) << 0xf;
                    pos[k].x = x;
                    pos[k].y = y;
                    k++;
                }
            }
        }
        if (k >= 1) {
            cMgSmartball_wing_c *spawned = (cMgSmartball_wing_c *)::operator new(0x88);
            if (spawned != 0) {
                V2 tmp = pos[0];
                spawned = cMgSmartball_wing_c_Construct(spawned, this, 0, &tmp);
            }
            mpWing = spawned;
        }
    }

    for (int i = 0; i < mAnaCount; i++) {
        void *p = mpAnas[i];
        if (p != 0)
            (*(VFunc **)p)[2](p);
    }
    for (int i = 0; i < mKinokoCount; i++) {
        void *p = mpKinokos[i];
        if (p != 0)
            (*(VFunc **)p)[2](p);
    }
    for (int i = 0; i < mBallCount; i++) {
        void *p = mpBalls[i];
        if (p != 0)
            (*(VFunc **)p)[2](p);
    }
    {
        void *p = mpSpring;
        (*(VFunc **)p)[2](p);
    }
    for (int i = 0; i < mPropellerCount; i++) {
        void *p = mpPropellers[i];
        if (p != 0)
            (*(VFunc **)p)[2](p);
    }
    for (int i = 0; i < mDokanCount; i++) {
        void *p = mpDokans[i];
        if (p != 0)
            (*(VFunc **)p)[2](p);
    }
    for (int i = 0; i < mPakkunCount; i++) {
        void *p = mpPakkuns[i];
        if (p != 0)
            (*(VFunc **)p)[2](p);
    }
    for (int i = 0; i < mPushSwitchCount; i++) {
        void *p = mpPushSwitches[i];
        if (p != 0)
            (*(VFunc **)p)[2](p);
    }
    {
        void *p = mpSlot;
        if (p != 0)
            (*(VFunc **)p)[2](p);
    }
    {
        void *p = mpBoard;
        if (p != 0)
            (*(VFunc **)p)[2](p);
    }
    {
        void *p = mpWing;
        if (p != 0)
            (*(VFunc **)p)[2](p);
    }

    mServeIndex = 0;
    mGateLampTimer[0] = 0;
    mGateLampTimer[1] = 0;
    mRestFrames = 0;
    mDrainTimer = 0;
    mDraining = 0;
    mRoundEnded = 0;
    for (int i = 0; i < 4; i++)
        mLineTally[i] = 0;
    mCellTally = 0;
    mBingoTally = 0;
    mKinokoTally = 0;
    mAnaTally = 0;
    mDokanTally = 0;
    for (int i = 0; i < 3; i++)
        mAwardTally[i] = 0;
    mOutTally = 0;
    mResultTimer = 0;
    for (int i = 0; i < 5; i++) {
        mPopupScore[i] = 0;
        mPopupMultiplier[i] = 0;
        mPopupAge[i] = 0;
        dScMgSmartball_c *popup = (dScMgSmartball_c *)(c + i * 8);
        *(int *)popup->mPopupPos = 0;
        *(int *)&popup->mPopupPos[4] = 0;
        mPopupDelay[i] = 0;
    }
    for (int i = 0; i < 0x10; i++) {
        mDustActive[i] = 0;
        mDustAge[i] = 0;
        dScMgSmartball_c *dust = (dScMgSmartball_c *)(c + i * 8);
        *(int *)dust->mDustPos = 0;
        *(int *)&dust->mDustPos[4] = 0;
        *(int *)dust->mDustVel = 0;
        *(int *)&dust->mDustVel[4] = 0;
    }
    mPhase = 0;
    mResultSlideX = 0x100;
    SetSubBg0Offset(mResultSlideX, 0);
    ResetSparks();
    __cxa_vec_cleanup(pos, 0x19, 8, (dtor_t)NullDestructor_0203d47c);
}

#pragma pop

/* Leftover: Render needs both pragmas off, and the locals below must stay in
 * this order (each one pins a stack slot). */
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
/* Slot 9: the frame draw. On the result screen it reveals the tally rows one
 * at a time, then draws the side lamps, every board object and the score
 * popups. */
s32 dScMgSmartball_c::Render() {
    char *g = (char *)this;
    int col;
    int xpos;
    int row;
    int limit;
    int i;
    int j;
    int limG;
    int *pcnt;
    int val;
    int *slot[2];

    SetSubBg0Offset(mResultSlideX, 0);
    col = 0;

    if (mPhase == 1 && mResultSlideX <= 0) {
        xpos = 0x18;
        slot[0] = (int *)0;

        /* Cells row. */
        if (mCellTally > 0) {
            Mg_RenderSpriteBoth(data_ov006_02138ae0[0], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0x64, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth(TextSpriteGfx(sSmartballTextSpritesByLanguage[i]), 0x90, xpos, -1,
                                -1);
            Mg_DrawNumber(0xa0, xpos, mCellTally, 0, 0, 1, 0);
            Mg_DrawNumber(0xe0, xpos, mCellTally * 0x64, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Line rows. */
        limit = col * 0x14;
        for (row = 0; row < 4; row++) {
            if (mResultTimer >= limit && mLineTally[row] > 0) {
                Mg_RenderSpriteBoth(data_ov006_02138ae0[row + 1], 0x30, xpos, -1, -1);
                val = sSmartballCellScores[row];
                Mg_DrawNumber(0x80, xpos, val, 0, 0, 1, 0);
                i = GetGameLanguage();
                Mg_RenderSpriteBoth(TextSpriteGfx(sSmartballTextSpritesByLanguage[i]), 0x90, xpos,
                                    -1, -1);
                /* Leftover: stepped base, element 0. */
                pcnt = ((dScMgSmartball_c *)(g + row * 4))->mLineTally;
                Mg_DrawNumber(0xa0, xpos, *pcnt, 0, 0, 0, 0);
                Mg_DrawNumber(0xe0, xpos, val * *pcnt, 0, 0, 1, 0);
                xpos += 0x18;
                limit += 0x14;
                col++;
            }
        }

        /* Mushrooms. */
        if (mKinokoTally > 0 && mResultTimer >= col * 0x14) {
            Mg_RenderSpriteBoth(sSmartballKinokoSprites[0], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0x12c, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth(TextSpriteGfx(sSmartballTextSpritesByLanguage[i]), 0x90, xpos, -1,
                                -1);
            Mg_DrawNumber(0xa0, xpos, mKinokoTally, 0, 0, 0, 0);
            Mg_DrawNumber(0xe0, xpos, mKinokoTally * 0x12c, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Pipes. */
        if (mDokanTally > 0 && mResultTimer >= col * 0x14) {
            Mg_RenderSpriteBoth(data_ov006_02137a78[0], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0xbb8, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth(TextSpriteGfx(sSmartballTextSpritesByLanguage[i]), 0x90, xpos, -1,
                                -1);
            Mg_DrawNumber(0xa0, xpos, mDokanTally, 0, 0, 0, 0);
            Mg_DrawNumber(0xe0, xpos, mDokanTally * 0xbb8, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Holes. */
        if (mAnaTally > 0 && mResultTimer >= col * 0x14) {
            Mg_RenderSpriteBoth(data_ov006_02137a6c[0], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0x7d0, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth(TextSpriteGfx(sSmartballTextSpritesByLanguage[i]), 0x90, xpos, -1,
                                -1);
            Mg_DrawNumber(0xa0, xpos, mAnaTally, 0, 0, 0, 0);
            Mg_DrawNumber(0xe0, xpos, mAnaTally * 0x7d0, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Award rows. */
        limG = col * 0x14;
        for (j = 0; j < 3; j++) {
            /* Leftover: stepped base, element 0. */
            slot[1] = ((dScMgSmartball_c *)(g + j * 4))->mAwardTally;
            if (*slot[1] > 0 && mResultTimer >= limG) {
                int mult;
                switch (j) {
                case 0:
                    Mg_RenderSpriteBoth(sSmartballMarkerSprites[1], 0x20, xpos, -1, -1);
                    Mg_RenderSpriteBoth(sSmartballMarkerSprites[1], 0x30, xpos, -1, -1);
                    Mg_RenderSpriteBoth(sSmartballMarkerSprites[1], 0x40, xpos, -1, -1);
                    mult = 5000;
                    break;
                case 1:
                    Mg_RenderSpriteBoth(sSmartballMarkerSprites[0], 0x20, xpos, -1, -1);
                    Mg_RenderSpriteBoth(sSmartballMarkerSprites[0], 0x30, xpos, -1, -1);
                    Mg_RenderSpriteBoth(sSmartballMarkerSprites[0], 0x40, xpos, -1, -1);
                    mult = 8000;
                    break;
                case 2:
                    Mg_RenderSpriteBoth(sSmartballMarkerSprites[2], 0x20, xpos, -1, -1);
                    Mg_RenderSpriteBoth(sSmartballMarkerSprites[2], 0x30, xpos, -1, -1);
                    Mg_RenderSpriteBoth(sSmartballMarkerSprites[2], 0x40, xpos, -1, -1);
                    mult = 10000;
                    break;
                }
                Mg_DrawNumber(0x80, xpos, mult, 0, 0, 1, 0);
                i = GetGameLanguage();
                Mg_RenderSpriteBoth(TextSpriteGfx(sSmartballTextSpritesByLanguage[i]), 0x90, xpos,
                                    -1, -1);
                Mg_DrawNumber(0xa0, xpos, *slot[1], 0, 0, 0, 0);
                Mg_DrawNumber(0xe0, xpos, mult * *slot[1], 0, 0, 1, 0);
                xpos += 0x18;
                limG += 0x14;
                col++;
            }
        }

        /* Bingos. */
        if (mBingoTally > 0 && mResultTimer >= col * 0x14) {
            Mg_RenderSpriteBoth(data_ov006_02138ae0[5], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0x2710, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth(TextSpriteGfx(sSmartballTextSpritesByLanguage[i]), 0x90, xpos, -1,
                                -1);
            Mg_DrawNumber(0xa0, xpos, mBingoTally, 0, 0, 0, 0);
            Mg_DrawNumber(0xe0, xpos, mBingoTally * 0x2710, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Balls lost. */
        if (mOutTally > 0 && mResultTimer >= col * 0x14) {
            Mg_RenderSpriteBoth(sSmartballBallSprites[0], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0x7d0, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth(TextSpriteGfx(sSmartballTextSpritesByLanguage[i]), 0x90, xpos, -1,
                                -1);
            Mg_DrawNumber(0xa0, xpos, mOutTally, 0, 0, 0, 0);
            Mg_DrawNumber(0xe0, xpos, mOutTally * 0x7d0, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Timer row. */
        if (mResultTimer >= col * 0x14) {
            if ((param1 & 0xff) == 0)
                Mg_RenderSpriteBoth(sSmartballTimerSprite[0], 0x40, xpos, -1, -1);
            else
                Mg_RenderSpriteBoth(sSmartballTimerSpriteAlt[0], 0x40, xpos, -1, -1);
            Mg_DrawNumber(0xe0, xpos, Mg_GetSubmittedScore(), 0, 0, 1, 0);
            col++;
            if (mResultTimer < 0x118)
                mResultTimer = 0x118;
        }

        /* Continue prompt. */
        if (mPhase == 1 && mResultTimer >= 0x12c) {
            if (col >= 7)
                Mg_DrawBlinkSprite(g, 0x80, 0xa8);
            else if (col == 6)
                Mg_DrawBlinkSprite(g, 0xe0, 0xa8);
            else
                Mg_DrawBlinkSprite(g, 0xe0, 0xa0);
        }
    }

    /* Sparks, except while draining on the main screen. */
    if ((param1 & 0xff) != 0 || mDraining == 0)
        ((dScMgSmartball_c *)(g))->RenderSparks();

    /* Score counters at the top of both screens. */
    Mg_DrawTopRightCounter(Mg_GetSubmittedScore(), 6);
    Mg_DrawTopLeftCounter(Mg_GetTopRecord());

    /* Score popups. */
    {
        int sb;
        for (sb = 0; sb < 5; sb++) {
            int *pcnt = LP(((dScMgSmartball_c *)(g + sb * 4))->mPopupScore);
            int cnt = *pcnt;
            if (cnt > 0) {
                slot[0] = LP(((dScMgSmartball_c *)(g + sb * 4))->mPopupMultiplier);
                int mode = *slot[0];
                if (mode >= 2) {
                    unsigned int u;
                    int nd;
                    int *py = LP(((dScMgSmartball_c *)(g + sb * 8))->mPopupPos + 4);
                    int *px = LP(((dScMgSmartball_c *)(g + sb * 8))->mPopupPos);
                    Mg_DrawNumber((*px >> 12) - 0x10, *py >> 12, cnt, -1, 0, 0, 0);
                    nd = 0;
                    /* Leftover: the score is read again through an int address. */
                    u = (unsigned int)*(int *)SlotAddress(
                        this, OFFSET_OF(dScMgSmartball_c, mPopupScore), sb, 4);
                    if (u != 0) {
                        do {
                            u /= 10;
                            nd++;
                        } while (u != 0);
                    }
                    i = GetGameLanguage();
                    Mg_RenderSpriteBoth(TextSpriteGfx(sSmartballTextSpritesByLanguage[i]),
                                        (*px >> 12) + nd * 8 - 8, *py >> 12, -1, -1);
                    nd = 0;
                    u = (unsigned int)*pcnt;
                    if (u != 0) {
                        do {
                            u /= 10;
                            nd++;
                        } while (u != 0);
                    }
                    Mg_DrawNumber((*px >> 12) + nd * 8 + 8, *py >> 12, *slot[0], -1, 0, 0, 0);
                } else {
                    dScMgSmartball_c *popup = (dScMgSmartball_c *)(g + sb * 8);
                    int y = *(int *)(popup->mPopupPos + 4);
                    int x = *(int *)popup->mPopupPos;
                    Mg_DrawNumber(x >> 12, y >> 12, cnt, -1, 0, 0, 0);
                }
            }
        }
    }

    /* Dust puffs. */
    {
        int slot;
        int two = 2, neg = -1, zero = 0;
        for (slot = 0; slot < 0x10; slot++) {
            if (mDustActive[slot] == 1) {
                int idx = mDustAge[slot] / 5;
                if (idx > 3)
                    idx = two;
                dScMgSmartball_c *dust = (dScMgSmartball_c *)(g + slot * 8);
                Mg_RenderSpriteBoth(data_ov006_02137a60[idx], *(int *)dust->mDustPos >> 12,
                                    *(int *)(dust->mDustPos + 4) >> 12, neg, zero);
            }
        }
    }

    /* Gate lamp, left. */
    {
        int v = mDrainTimer;
        int flag = 0;
        if (v > 0) {
            if (v % 30 > 0xf)
                flag = 1;
        } else {
            if (mGateLampTimer[0] > 0)
                flag = 1;
        }
        if ((param1 & 0xff) == 0)
            Mg_RenderSpriteBoth(sSmartballSideSprites[flag], 0x18, -0x90, -1, 0);
        else
            Mg_RenderSpriteBoth(sSmartballSideSpritesAlt[flag], 0x18, -0x90, -1, 0);
    }

    /* Gate lamp, right. */
    {
        int v = mDrainTimer;
        int flag = 0;
        if (v > 0) {
            if (v % 30 > 0xf)
                flag = 1;
        } else {
            if (mGateLampTimer[1] > 0)
                flag = 1;
        }
        if ((param1 & 0xff) == 0)
            Mg_RenderSpriteBoth(sSmartballSideSprites[flag], 0xe8, -0x90, -1, -1);
        else
            Mg_RenderSpriteBoth(sSmartballSideSpritesAlt[flag], 0xe8, -0x90, -1, -1);
    }

    /* Draw every board object. */
    {
        int k;
        for (k = 0; k < mPropellerCount; k++)
            ((DispObj *)mpPropellers[k])->f1();
    }
    if (mpSlot != 0)
        ((DispObj *)mpSlot)->f1();
    {
        int k;
        for (k = 0; k < mBallCount; k++)
            ((DispObj *)mpBalls[k])->f1();
    }
    {
        int k;
        for (k = 0; k < mAnaCount; k++)
            ((DispObj *)mpAnas[k])->f1();
    }
    {
        int k;
        for (k = 0; k < mKinokoCount; k++)
            ((DispObj *)mpKinokos[k])->f1();
    }
    {
        int k;
        for (k = 0; k < mDokanCount; k++)
            ((DispObj *)mpDokans[k])->f1();
    }
    {
        int k;
        for (k = 0; k < mPakkunCount; k++)
            ((DispObj *)mpPakkuns[k])->f1();
    }
    {
        int k;
        for (k = 0; k < mPushSwitchCount; k++)
            ((DispObj *)mpPushSwitches[k])->f1();
    }
    ((DispObj *)mpSpring)->f1();
    if (mpBoard != 0)
        ((DispObj *)mpBoard)->f1();
    if (mpWing != 0)
        ((DispObj *)mpWing)->f1();
    {
        int k;
        for (k = 0; k < mBallCount; k++)
            mpBalls[k]->DrawMarker();
    }

    if ((param1 & 0xff) == 0 && mDraining != 0)
        ((dScMgSmartball_c *)(g))->RenderSparks();

    return 1;
}

#pragma pop

/* Leftover: Behavior needs strength reduction off. */
#pragma push
#pragma opt_strength_reduction off
/* Slot 6: the scene's per-frame rules. Phase 0 updates every object, then the
 * score, dust and ball collisions, and advances the serve index; phase 1
 * slides in the result screen and waits for a touch; phase 2 stays there.
 * Leftovers: the slot addresses go through an intptr_t (the ROM keeps the
 * address in a register instead of folding it into an indexed load), and the
 * declarations below must keep C linkage and their `extern` keyword. */
s32 dScMgSmartball_c::Behavior() {
    char *c = (char *)this;
    int vec[3];

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }

    switch (mPhase) {
    case 0: {
        char *o;
        int i;

        o = (char *)mpSpring;
        (**(VFunc **)o)(o);

        for (i = 0; i < mBallCount; i++) {
            o = (char *)TableEntry(this, OFFSET_OF(dScMgSmartball_c, mpBalls), i);
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mKinokoCount; i++) {
            o = (char *)TableEntry(this, OFFSET_OF(dScMgSmartball_c, mpKinokos), i);
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mAnaCount; i++) {
            o = (char *)TableEntry(this, OFFSET_OF(dScMgSmartball_c, mpAnas), i);
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mPropellerCount; i++) {
            o = (char *)TableEntry(this, OFFSET_OF(dScMgSmartball_c, mpPropellers), i);
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mDokanCount; i++) {
            o = (char *)TableEntry(this, OFFSET_OF(dScMgSmartball_c, mpDokans), i);
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mPakkunCount; i++) {
            o = (char *)TableEntry(this, OFFSET_OF(dScMgSmartball_c, mpPakkuns), i);
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mPushSwitchCount; i++) {
            o = (char *)TableEntry(this, OFFSET_OF(dScMgSmartball_c, mpPushSwitches), i);
            (**(VFunc **)o)(o);
        }

        o = (char *)mpSlot;
        if (o != 0) {
            (**(VFunc **)o)(o);
        }
        o = (char *)mpBoard;
        if (o != 0) {
            (**(VFunc **)o)(o);
        }
        o = (char *)mpWing;
        if (o != 0) {
            (**(VFunc **)o)(o);
        }

        UpdateScores();
        UpdateDust();
        CollideBalls();

        if (IsTableAtRest() != 0) {
            if (mRestFrames >= 0x3c) {
                mPhase = 1;
            } else {
                int *p = &mRestFrames;
                (*p)++;
            }
        } else {
            int cur;
            int next;
            mRestFrames = 0;
            cur = mServeIndex;
            next = cur + 1;
            if (next >= mBallCount) {
                next = 0;
            }
            if (mpBalls[cur]->IsOut() != 0) {
                mServeIndex = next;
            }
        }

        if (mDraining != 0) {
            int t;
            {
                int *p = &mDrainTimer;
                (*p)++;
            }
            if (mDrainTimer == 0xb4) {
                Mg_RestoreMusic();
                FreeGfxSlotsById(0x1d);
            }
            t = mDrainTimer;
            if (t >= 0xb4 && t % 0x1e == 0) {
                int found = 0;
                for (i = 0; i < mBallCount; i++) {
                    o = (i >= 13) ? 0 : (char *)mpBalls[i];
                    if (((cMgSmartball_ball_c *)o)->mIsActive != 0 &&
                        ((i >= 13) ? 0 : mpBalls[i])->ArmExpire(1) != 0) {
                        ((i >= 13) ? 0 : mpBalls[i])->Burst();
                        CopyObjectPos(vec, (i >= 13) ? 0 : mpBalls[i]);
                        ((dScMgSmartball_c *)c)->EmitScore(vec, 0x7d0, 0, 1);
                        Sound::PlayBank2_2D(0x175);
                        ((dScMgSmartball_c *)(c))->CountOut();
                        found = 1;
                        break;
                    }
                }
                if (found == 0) {
                    mDrainTimer = 0;
                    mDraining = 0;
                }
            }
        }
    } break;
    case 1:
        UpdateScores();
        mPromptEnabled = 0;
        if (mResultTimer == 0) {
            gSubScreenLayers[0] |= 1;
        }
        if (mResultSlideX > 0) {
            int *p = &mResultSlideX;
            *p -= 4;
        } else {
            int flag = 0;
            mResultSlideX = 0;
            if (mResultTimer < 0x12c) {
                int *p = &mResultTimer;
                (*p)++;
            } else {
                int idx = gActivePlayerSlot;
                if (gTouchHeld[idx * 4] != 0 && gTouchEdge[idx * 4] != 0) {
                    flag = 1;
                }
                if (flag != 0) {
                    if (((int)param1 & 0xff) == 0) {
                        sMgResultsBannerX = 0x80;
                        sMgResultsBannerY = -0x30;
                    }
                    Mg_SetSharedState(MG_STATE_RESULTS);
                    mPhase = 2;
                    func_02012790(0x62);
                }
            }
        }
        break;
    case 2:
        UpdateScores();
        break;
    }

    if (mGateLampTimer[0] > 0) {
        int *p = &mGateLampTimer[0];
        (*p)--;
    }
    if (mGateLampTimer[1] > 0) {
        int *p = &mGateLampTimer[1];
        (*p)--;
    }

    ((dScMgSmartball_c *)(c))->UpdateSparks();
    return 1;
}

#pragma pop

#include "nitro/gx/g2.h"
#include "types.h"
#include "dScMgSmartball_c.h"
/* Slot 18: starts a round and sets up both screen blends. */
void dScMgSmartball_c::StartRound(int /* arg */) {
    SpawnObjects();
    G2_SetBlendAlpha(0, 0x18, 4, 0xa);
    G2S_SetBlendAlpha(0, 0x18, 4, 0xa);
}

/* Slot 31: loads the option palette and tilemap for the sub screen. */
int dScMgSmartball_c::InitSubScreenBg() {
    int f;
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
    G2S_BG1Mosaic(0);
    G2S_SetBG1Priority(0);
    SetSubBg1Offset(0, 0);
    gSubScreenLayers[0] &= ~2;
    f = GetGameLanguage();
    LoadCompressedFileAt(sSmartballOptionNcgByLanguage[f], (void *)G2S_GetBG1CharPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_OPTION_NSC_BIN, G2S_GetBG1ScrPtr());
}

/* G2_SetOBJMode/G2_GetOBJColorParam: mode in attr0-1 bits 10-11, colour in
 * attr2 bits 12-15. */
enum { GX_OAM_MODE_XLU = 1 };

inline int G2_GetOBJColorParam(const OamAttr *oam) {
    return (oam->attr2 & 0xf000) >> 12;
}

inline void G2_SetOBJMode(OamAttr *oam, int mode, int cParam) {
    *(u32 *)oam = (*(u32 *)oam & ~0xc00) | (mode << 10);
    oam->attr2 = (u16)((oam->attr2 & ~0xf000) | (cParam << 12));
}

/* Slot 0: scene setup. Clamps the mode byte into mLevel, brings up both
 * screens' BG layers, loads the level's tile and palette sets, and spawns the
 * table.
 * Leftover: the mode byte needs both the mask and the (u8) cast, and the last
 * Pakkun G2_SetOBJMode reads its colour param from [2] instead of [2]+1. */
s32 dScMgSmartball_c::InitResources() {
    char *c = (char *)this;
    void *f;
    s32 i;
    if ((*((s32 *)(c + 8))) & 0xff) {
        mLevel = (u8)((*((s32 *)(c + 8))) & 0xFF);
        if ((mLevel) > 0x270e) {
            mLevel = 0x270e;
        }
    }
    data_0208ee44 = 1;
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_512x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                      GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01);
    G2S_BG0Mosaic(false);
    G2S_SetBG0Offset(0x100, 0);
    G2S_SetBG0Priority(0);
    f = Ov004_LoadFileByPath((const char *)&sSmartballResultBgNcgPath);
    DecompressLZ16(f, G2S_GetBG0CharPtr());
    Ov004_Deallocate(f);
    {
        /* volatile: same stack round trip of the fill value as PaintCell. */
        volatile u16 v;
        MI_CpuFill16(G2S_GetBG0ScrPtr(), (u16)(v = 0), 0x1000);
    }
    f = Ov004_LoadFileByPath((const char *)&sSmartballResultBgNscPath);
    DecompressLZ16(f, G2S_GetBG0ScrPtr());
    Ov004_Deallocate(f);
    f = Ov004_LoadFileByPath((const char *)&sSmartballResultNclPath);
    DC_FlushRange(f, 0x20);
    GXS_LoadBGPltt(((char *)f) + 2, 2, 0x1e);
    Ov004_Deallocate(f);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xa000,
                         GX_BG_CHARBASE_0x10000);
    G2_BG2Mosaic(false);
    G2_SetBG2Offset(0, 0);
    G2_SetBG2Priority(1);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xa000,
                          GX_BG_CHARBASE_0x10000);
    G2S_BG2Mosaic(false);
    G2S_SetBG2Offset(0, 0);
    G2S_SetBG2Priority(1);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xb000,
                         GX_BG_CHARBASE_0x18000);
    G2_BG3Mosaic(false);
    G2_SetBG3Offset(0, 0);
    G2_SetBG3Priority(2);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xb000,
                          GX_BG_CHARBASE_0x18000);
    G2S_BG3Mosaic(false);
    G2S_SetBG3Offset(0, 0);
    G2S_SetBG3Priority(2);
    if (((u8)(*((s32 *)(c + 8)))) == 0) {
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay0Bg1FrameNcgPath);
        DecompressLZ16(f, G2_GetBG2CharPtr());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay0Bg2FrameNcgPath);
        DecompressLZ16(f, G2S_GetBG2CharPtr());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay0Bg1NcgPath);
        DecompressLZ16(f, (void *)G2_GetBG3CharPtr());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay0Bg2NcgPath);
        DecompressLZ16(f, G2S_GetBG3CharPtr());
        Ov004_Deallocate(f);
    } else {
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay1Bg1FrameNcgPath);
        DecompressLZ16(f, G2_GetBG2CharPtr());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay1Bg2FrameNcgPath);
        DecompressLZ16(f, G2S_GetBG2CharPtr());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay1Bg1NcgPath);
        DecompressLZ16(f, (void *)G2_GetBG3CharPtr());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay1Bg2NcgPath);
        DecompressLZ16(f, G2S_GetBG3CharPtr());
        Ov004_Deallocate(f);
    }
    if (((u8)(*((s32 *)(c + 8)))) == 0) {
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay0Bg1NclPath);
        DC_FlushRange(f, 0x1a0);
        GX_LoadBGPltt(f, 0x60, 0x1a0);
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay0Bg2NclPath);
        DC_FlushRange(f, 0x1a0);
        GXS_LoadBGPltt(f, 0x60, 0x1a0);
        Ov004_Deallocate(f);
    } else {
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay1Bg1NclPath);
        DC_FlushRange(f, 0x1a0);
        GX_LoadBGPltt(f, 0x60, 0x1a0);
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath((const char *)&sSmartballLay1Bg2NclPath);
        DC_FlushRange(f, 0x1a0);
        GXS_LoadBGPltt(f, 0x60, 0x1a0);
        Ov004_Deallocate(f);
    }
    gMainScreenLayers &= ~DISPLAY_LAYER_BG0;
    gMainScreenLayers |= DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3;
    gSubScreenLayers[0] |= 0xd;
    f = Ov004_LoadFileByPath((const char *)&sSmartballNclPath);
    DC_FlushRange(f, 0x100);
    GX_LoadOBJPltt(f, 0, 0x100);
    GXS_LoadOBJPltt(f, 0, 0x100);
    Ov004_Deallocate(f);
    i = 0;
    do {
        mpBalls[i] = 0;
        i++;
    } while (i < 0xd);
    i = 0;
    do {
        mpKinokos[i] = 0;
        i++;
    } while (i < 0x19);
    i = 0;
    do {
        mpAnas[i] = 0;
        i++;
    } while (i < 8);
    i = 0;
    do {
        mpPropellers[i] = 0;
        i++;
    } while (i < 3);
    i = 0;
    do {
        mpDokans[i] = 0;
        i++;
    } while (i < 6);
    i = 0;
    do {
        mpPakkuns[i] = 0;
        i++;
    } while (i < 3);
    i = 0;
    do {
        mpPushSwitches[i] = 0;
        i++;
    } while (i < 2);
    {
        V2 r1;
        cMgSmartball_spring_c *spawned;
        r1.x = 0xec000;
        r1.y = 0xa0000;
        spawned = (cMgSmartball_spring_c *)::operator new(0x48);
        if (spawned != 0) {
            V2 r2 = r1;
            spawned = cMgSmartball_spring_c_Construct(spawned, this, 0, &r2);
        }
        mpSpring = spawned;
    }
    mpSlot = 0;
    mpBoard = 0;
    mpWing = 0;
    G2_SetBlendAlpha(0, 0x18, 4, 0xa);
    G2S_SetBlendAlpha(0, 0x18, 4, 0xa);
    G2_SetOBJMode(sSmartballBallSprites[1], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballBallSprites[1]));
    G2_SetOBJMode(sSmartballBallSpritesAlt[1], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballBallSpritesAlt[1]));
    G2_SetOBJMode(sSmartballPushSwitchSprites[2], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballPushSwitchSprites[2]));
    G2_SetOBJMode(sSmartballPropellerSprites[1], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballPropellerSprites[1]));
    G2_SetOBJMode(sSmartballKinokoSprites[1], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballKinokoSprites[1]));
    G2_SetOBJMode(sSmartballKinokoTurnSprites[1], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballKinokoTurnSprites[1]));
    G2_SetOBJMode(sSmartballSpringSprites[1], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballSpringSprites[1]));
    G2_SetOBJMode(sSmartballSpringSpritesAlt[1], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballSpringSpritesAlt[1]));
    G2_SetOBJMode(sSmartballPakkunSprites[2], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballPakkunSprites[2]));
    G2_SetOBJMode(sSmartballPakkunSprites[2] + 1, GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballPakkunSprites[2]));
    G2_SetOBJMode(sSmartballWingSprites[1], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballWingSprites[1]));
    G2_SetOBJMode(sSmartballWingBodySprites[1], GX_OAM_MODE_XLU,
                  G2_GetOBJColorParam(sSmartballWingBodySprites[1]));
    SpawnObjects();
    return 1;
}

/* Leftover: AfterCleanupResources needs strength reduction off. */
#pragma push
#pragma opt_strength_reduction off
/* Slot 5: frees every board object, then chains to the base. */
void dScMgSmartball_c::AfterCleanupResources(u32 vfSuccess) {
    if (vfSuccess != 2)
        return;

    for (int i = 0; i < 0xd; i++) {
        cMgSmartball_ball_c **slot =
            (cMgSmartball_ball_c **)SlotAddress(this, OFFSET_OF(dScMgSmartball_c, mpBalls), i, 4);
        cMgSmartball_ball_c *object = *slot;
        if (object != 0) {
            if (object != 0) {
                *(VFunc *volatile *)object = cMgSmartball_ball_c_vtbl;
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            *slot = 0;
        }
    }
    mBallCount = 0;

    for (int i = 0; i < 0x19; i++) {
        cMgSmartball_kinoko_c **slot = (cMgSmartball_kinoko_c **)SlotAddress(
            this, OFFSET_OF(dScMgSmartball_c, mpKinokos), i, 4);
        cMgSmartball_kinoko_c *object = *slot;
        if (object != 0) {
            if (object != 0) {
                *(VFunc *volatile *)object = cMgSmartball_kinoko_c_vtbl;
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            *slot = 0;
        }
    }
    mKinokoCount = 0;

    for (int i = 0; i < 8; i++) {
        cMgSmartball_ana_c **slot =
            (cMgSmartball_ana_c **)SlotAddress(this, OFFSET_OF(dScMgSmartball_c, mpAnas), i, 4);
        cMgSmartball_ana_c *object = *slot;
        if (object != 0) {
            if (object != 0) {
                *(VFunc *volatile *)object = cMgSmartball_ana_c_vtbl;
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            *slot = 0;
        }
    }
    mAnaCount = 0;

    {
        cMgSmartball_spring_c *object = mpSpring;
        if (object != 0) {
            if (object != 0) {
                *(VFunc *volatile *)object = cMgSmartball_spring_c_vtbl;
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            mpSpring = 0;
        }
    }

    for (int i = 0; i < 3; i++) {
        cMgSmartball_propeller_c **slot = (cMgSmartball_propeller_c **)SlotAddress(
            this, OFFSET_OF(dScMgSmartball_c, mpPropellers), i, 4);
        cMgSmartball_propeller_c *object = *slot;
        if (object != 0) {
            if (object != 0) {
                *(VFunc *volatile *)object = cMgSmartball_propeller_c_vtbl;
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            *slot = 0;
        }
    }

    for (int i = 0; i < 6; i++) {
        cMgSmartball_dokan_c **slot =
            (cMgSmartball_dokan_c **)SlotAddress(this, OFFSET_OF(dScMgSmartball_c, mpDokans), i, 4);
        cMgSmartball_dokan_c *object = *slot;
        if (object != 0) {
            if (object != 0) {
                *(VFunc *volatile *)object = cMgSmartball_dokan_c_vtbl;
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            *slot = 0;
        }
    }
    mDokanCount = 0;

    for (int i = 0; i < 3; i++) {
        cMgSmartball_pakkun_c **slot = (cMgSmartball_pakkun_c **)SlotAddress(
            this, OFFSET_OF(dScMgSmartball_c, mpPakkuns), i, 4);
        cMgSmartball_pakkun_c *object = *slot;
        if (object != 0) {
            if (object != 0) {
                *(VFunc *volatile *)object = cMgSmartball_pakkun_c_vtbl;
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            *slot = 0;
        }
    }
    mPakkunCount = 0;

    for (int i = 0; i < 2; i++) {
        cMgSmartball_pushswitch_c **slot = (cMgSmartball_pushswitch_c **)SlotAddress(
            this, OFFSET_OF(dScMgSmartball_c, mpPushSwitches), i, 4);
        cMgSmartball_pushswitch_c *object = *slot;
        if (object != 0) {
            if (object != 0) {
                *(VFunc *volatile *)object = cMgSmartball_pushswitch_c_vtbl;
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            *slot = 0;
        }
    }
    mPushSwitchCount = 0;

    {
        cMgSmartball_slot_c *object = mpSlot;
        if (object != 0) {
            if (object != 0) {
                *(VFunc **)object = cMgSmartball_slot_c_vtbl;
                __cxa_vec_cleanup(object->mReelVel, 3, 8, (dtor_t)NullDestructor_0203d47c);
                __cxa_vec_cleanup(object->mReelOffset, 3, 8, (dtor_t)NullDestructor_0203d47c);
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            mpSlot = 0;
        }
    }
    {
        cMgSmartball_board_c *object = mpBoard;
        if (object != 0) {
            if (object != 0) {
                *(VFunc **)object = cMgSmartball_board_c_vtbl;
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            mpBoard = 0;
        }
    }
    {
        cMgSmartball_wing_c *object = mpWing;
        if (object != 0) {
            if (object != 0) {
                *(VFunc **)object = cMgSmartball_wing_c_vtbl;
                *(VFunc *volatile *)object = cMgSmartball_object_c_vtbl;
                ::operator delete(object);
            }
            mpWing = 0;
        }
    }

    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

#pragma pop

/* The scene factory and its empty particle-element constructor. */
void SmartballObj_ElemCtor(void);


/* Builds the four heap arrays the destructor tears down. */
inline dScMgSmartball_c::dScMgSmartball_c() {
    __cxa_vec_ctor(mPopupPos, 5, 8, (ctor_t)func_0203d738, (dtor_t)NullDestructor_0203d47c);
    __cxa_vec_ctor(mDustPos, 0x10, 8, (ctor_t)func_0203d738, (dtor_t)NullDestructor_0203d47c);
    __cxa_vec_ctor(mDustVel, 0x10, 8, (ctor_t)func_0203d738, (dtor_t)NullDestructor_0203d47c);
    __cxa_vec_ctor(mSparks, 0x40, 0x24, (ctor_t)SmartballObj_ElemCtor,
                   (dtor_t)SmartballObj_ElemDtor);
}

/* Allocates and constructs the scene; null if allocation fails. */
 void *dScMgSmartball_c_classInit(void) {
    return new dScMgSmartball_c;
}

 void SmartballObj_ElemCtor(void) {}
