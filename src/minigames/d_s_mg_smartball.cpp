//cpp
/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgSmartball_c_AssetId { ASSET_D_2D_MG_BG_OPTION_NSC_BIN = 0x005b };

#pragma defer_codegen off
/* Smartball scene and its board objects, ov006:0x0210d740..0x02119904.
 * Definitions follow ROM address order. Immediate code generation makes each
 * optimizer bracket local to its function under mwccarm 2004/b56.
 *
 * _ZTS16dScMgSmartball_c 0x0213ec7c. _ZTI16dScMgSmartball_c 0x0213ec28.
 * _ZTV16dScMgSmartball_c 0x0213eefc. The destructor is the key function:
 * D1 at 0x0210d740, D0 at 0x0210d7e0.
 *
 * The virtual methods use the class hierarchy from the shared headers. The
 * remaining C ABI helpers and local layout views keep the individually
 * verified reconstruction; consolidating this unit does not establish their
 * original names or interfaces.
 */

// leftovers:
// - SmartballBall_UpdatePhysics (the ball per-frame physics) stays a free extern "C"
//   function. Compiled as C++ -- as a cMgSmartball_ball_c member or a plain
//   free function -- mwccarm folds `angle / 0x20` into a `+= 0x800` induction
//   accumulator in the two fan-response loops (53 words differ); the C front
//   end keeps the divide sequence. Its callers call it by name.
// - The twelve constructor-shaped helpers (cMgSmartball_wing_c_Construct,
//   cMgSmartball_pushswitch_c_Construct, cMgSmartball_board_c_Construct, cMgSmartball_slot_c_Construct,
//   cMgSmartball_pakkun_c_Construct, cMgSmartball_dokan_c_Construct, cMgSmartball_propeller_c_Construct,
//   cMgSmartball_spring_c_Construct, cMgSmartball_ana_c_Construct, cMgSmartball_kinoko_c_Construct,
//   cMgSmartball_ball_c_Construct, cMgSmartball_object_c_Construct) stay free: they placement-
//   construct objects they do not own. SmartballObj_ElemDtor and
//   SmartballObj_ElemCtor are the manager array element destructor and the
//   empty final hook.
// - The layout shadows are gone. The scene-side ones (Mgr, Mgr2, SArrays)
//   were dScMgSmartball_c's fields; the object-side ones (Obj, Obj2, SBoard,
//   Owl, C2, V2ab, A, S28, S24) were the cMgSmartball_*_c members they
//   overlaid, and those sites read as members now. The objects reach the
//   scene by casting the base's s32 mpManager. The types still declared
//   below are local value types, not views of a class.
// - A few object-side sites keep a pinned shape, each with its reason beside
//   it: slot BeginStop writes the stop counts through a copy of `this`, and
//   propeller SaveSnapshot steps the spin speed through a short pointer.
// - The object tables, popups, dust and score block from 0x4660 on are
//   dScMgSmartball_c members now. A few strided sites (SpawnObjects,
//   AfterCleanup, SpawnDust/UpdateDust, CollideBalls' first ball, Render's
//   popup list, line tallies and award tallies) still step a cast `this` by
//   the element size and read element 0, because indexing moves bytes; Render re-reads one popup score through an int address,
//   and EmitSparks/UpdateSparks keep a few raw spark writes. Each has its
//   reason beside it. The objects' angle at +0x32 stays a per-reader cast,
//   as cMgSmartball_object_c.h explains.

#include "dScMgSmartball_c.h"
#include "CP15.h"
#include "cMgSmartball_wing_c.h"
#include "OamAttr.h"
#include "cMgSmartball_pushswitch_c.h"
#include "cMgSmartball_board_c.h"
#include "types.h"
#include "cMgSmartball_slot_c.h"
#include "cMgSmartball_pakkun_c.h"
#include "cMgSmartball_dokan_c.h"
#include "cMgSmartball_propeller_c.h"
#include "cMgSmartball_spring_c.h"
#include "cMgSmartball_ana_c.h"
#include "cMgSmartball_kinoko_c.h"
#include "cMgSmartball_ball_c.h"
#include "cMgSmartball_object_c.h"
#include "dScMgBase_c.h"
#include "launder.h"
#include "private/disp_obj_vtbl.h"
#include "private/ov006_smartball_pair.h"
#include "PlayerInput.h"
#include "Sound.h"
#include "Memory.h"
#include "nitro/fx/fx.h"
#include "GX.h"
#include "cstd.h"
#include "G2.h"

/* Local value types carried over from the legacy files. None of these is a
 * view of a class any more: the object-side views (Obj, Obj2, SBoard, Owl,
 * C2, V2ab, A, S28, S24) were each a cast of `this` or of a table entry, and
 * every one of their fields now reads as a cMgSmartball_*_c or
 * dScMgSmartball_c member. What is left is plain locals and parameters: the
 * two-word position/velocity pairs, a nine-word table, the sprite scale
 * typedef, a vtable-slot pointer and the physics step's search state. No
 * shared header declares an equivalent type, so they stay here. */

/* The two-word (x, y) pair the fx helpers (func_0203d388, func_0203d434,
   func_0203d570) take. */
typedef struct {
    int x, y;
} V2;

/* The same two words, named x/z by the ball physics. */
typedef struct V2xz {
    int x;
    int z;
} V2xz;

/* The pair Vec2_Len measures. */
typedef struct {
    int x, y;
} Vec2;

/* The point and triangle corners the pakkun, propeller and ball hit tests
   hand to PointInTriangle. */
struct S {
    int a, b;
};

/* Nine words: the shape of sSmartballCellStartOrder, copied whole by
   ShuffleCells. */
struct W9 {
    int w[9];
};

/* OAM::Render's scale arguments. */
typedef int Fix12i;

/* Three pairs: the dokan's scratch block. */
typedef struct {
    V2 d;
    V2 v;
    V2 e;
} W;

/* A pair with an empty destructor, used by SpawnDust and ResolveBallPair;
   the comment above SpawnDust says why the destructor matters. */
struct TVec2 {
    int x;
    int y;
    ~TVec2() {}
};

/* A first-vtable-slot call, as Behavior's per-object dispatch spells it. */
typedef void (*VFunc)(void *);

/* SmartballBall_UpdatePhysics's search state. */
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
/* Of this helper's five recorded parameter views three spell the first
   argument int and two spell it a pointer, and of its call sites in this file
   all but two pass an int. The int spelling is kept and those two sites read
   their table entry the same way the other eighty do. */
extern "C" void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
extern "C" struct OamAttr *sSmartballWingSprites[];
struct OAM {
    static void Render(bool, OamAttr *, int, int, int, int, int, int, int, int);
};
extern "C" void OamRender10_d_s_mg_smartball(int sub, struct OamAttr *data, s32 x, s32 y,
                                             s32 palette, s32 priority, Fix12i scaleX,
                                             Fix12i scaleY, s32 rotation, s32 mode);
extern int Vec2_Len(Vec2 *v);
/* Five of this helper's six recorded views spell the first and last arguments
   pointers and one spells them ints; the pointer view is kept. */
extern "C" void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern "C" int GetOwnerLanguage(void);
extern OamAttr *sSmartballLineSpritesItalian[];
extern OamAttr *sSmartballLineSpritesSpanish[];
extern OamAttr *sSmartballLineSpritesGerman[];
extern OamAttr *sSmartballLineSpritesFrench[];
extern OamAttr *sSmartballLineSprites[];
extern OamAttr *sSmartballCellSprites[];
extern "C" s16 data_02082214[];
extern "C" s32 sSmartballCellOrder[];
extern OamAttr *sSmartballCellRowSprites[];

/* The two legacy views of this helper disagreed on the destination pointer
   (char * against void *). void * is the reconciliation that accepts every
   call site in this file unchanged: the byte-stepping callers convert to it
   implicitly, and the screen-pointer caller already yields one. A pointer
   parameter is passed the same way either way, so this is a spelling choice,
   not a change of contract. */
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern "C" void func_02012718(int a, int b);
extern struct W9 sSmartballCellStartOrder;
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern "C" void *sSmartballSlotFrameSprite;
extern "C" void *sSmartballSlotReelEndSprite;
extern "C" void *sSmartballSlotReelLowerSprite;
extern "C" void *sSmartballSlotReelUpperSprite;
extern "C" void Vec2_Sub(int *o, int *a, int *b);
extern OamAttr *sSmartballDokanSprites[];
extern OamAttr *sSmartballAnaSprites[];
extern int func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);
extern int func_0203d570(V2 *a, V2 *b);
extern int func_0203d434(V2 *p);
extern void func_0203d630(V2 *p, int scale);
extern void func_0203d480(V2 *out, V2 *in);
extern "C" int func_0203d524(V2 *a, V2 *b);
extern int func_0203d4d0(void *p, int *a, int *b);
extern OamAttr *sSmartballSparkSpritesAlt[];
extern OamAttr *sSmartballSparkSprites[];
extern void Mg_ShowBanner(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void Mg_PlayJingle(void *c);
extern "C" int Mg_GetSubmittedScore(void);
extern "C" void Mg_SubmitScore(int self);
extern s16 atan2_hold_d_s_mg_smartball_1(int y, int x);
extern void Mg_SetTopScreenYOffset(int a);
extern void func_02057d00(void *dst, void *src, int flag);
extern void *Ov004_LoadFileByPath(void *p);
extern void DecompressLZ16(void *src, void *dst);
extern void Ov004_Deallocate(void *p);

extern void __cxa_vec_ctor(void *p, int a, int b, void *cb1, void *cb2);
extern void SetSubBg0Offset(int x, int y);
extern void func_0203d738(void);
extern int sSmartballPathBuf[];
extern int sSmartballLayoutNcgFmtSpanish[];
extern int sSmartballLayoutNcgFmtItalian[];
extern int sSmartballLayoutNcgFmtGerman[];
extern int sSmartballLayoutNcgFmtFrench[];
extern int sSmartballLayoutNcgFmtE[];
extern int sSmartballLayoutNcgFmt[];
extern int sSmartballBg1NscFmt[];
extern int sSmartballBg2NscFmt[];
extern int sSmartballLay0Bg1FrameNscPath[];
extern int sSmartballLay0Bg2FrameNscPath[];
extern int sSmartballLay0Bg1NscPath[];
extern int sSmartballLay0Bg2NscPath[];
extern int sSmartballLay1Bg1FrameNscPath[];
extern int sSmartballLay1Bg2FrameNscPath[];
extern int sSmartballLay1Bg1NscPath[];
extern int sSmartballLay1Bg2NscPath[];
void Mg_DrawNumber(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
int GetGameLanguage(void);
int Mg_GetTopRecord(void);
void Mg_DrawTopRightCounter(int a0, int a1);
int Mg_DrawTopLeftCounter(void *self);
void Mg_DrawBlinkSprite(void *c, int a1, int a2);
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
void FreeGfxSlotsById(int a);

extern unsigned int LoadCompressedFileAt(int fileID, void *target);

extern unsigned char data_0209d454[];
extern u8 data_0209d45c;
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
/* The Render shard read 0x5998 as a void * and this one read it as an int;
   both now read the header's s32 mResultSlideX. */
/* The alternate MultiStore16 view carried by the InitResources shard is the
   void * one adopted above; the two are reconciled, not in conflict. */
}

extern int _ZTV25cMgSmartball_pushswitch_c[];
extern int _ZTV20cMgSmartball_board_c[];
extern int _ZTV21cMgSmartball_pakkun_c[];
extern int _ZTV20cMgSmartball_dokan_c[];
extern int _ZTV24cMgSmartball_propeller_c[];
extern int _ZTV21cMgSmartball_spring_c[];
extern int _ZTV18cMgSmartball_ana_c[];
extern int _ZTV19cMgSmartball_ball_c[];
extern int _ZTV21cMgSmartball_object_c[];
extern int _ZTV21cMgSmartball_kinoko_c[];
extern int _ZTV19cMgSmartball_slot_c[];
extern int _ZTV19cMgSmartball_wing_c[];
/* Local helper declarations. */
/* These declarations also cover helpers called before their ROM-ordered definition. */
extern "C" {
char *cMgSmartball_wing_c_Construct(char *self, int x, int y, int *src);
char *cMgSmartball_kinoko_c_Construct(char *self, int x, int y, int *src);
int cMgSmartball_pushswitch_c_Construct(char *c, int b, int d, int *s);
int cMgSmartball_board_c_Construct(char *c, int b, int d, int *s);
int cMgSmartball_pakkun_c_Construct(char *c, int b, int d, int *s);
int cMgSmartball_dokan_c_Construct(char *c, int b, int d, int *s);
int cMgSmartball_propeller_c_Construct(char *c, int b, int d, int *s);
int cMgSmartball_spring_c_Construct(char *c, int b, int d, int *s);
int cMgSmartball_ana_c_Construct(char *c, int b, int d, int *s);
int cMgSmartball_ball_c_Construct(char *c, int b, int d, int *s);
void CopyObjectVel(int *a, int *b);
void CopyObjectPos(int *a, int *b);
void SmartballBall_UpdatePhysics(cMgSmartball_ball_c *self);
void *cMgSmartball_slot_c_Construct(char *c, int a1, int a2, int *src);
}

/* Declarations this TU still needs from the shared common header, which is
   deliberately not included here: three of its parameter views disagree with
   definitions in this file and pulling it in makes each an illegal overload.
   Mangled names are declared with C linkage so the C++ front end does not
   mangle them a second time. */
extern "C" {

extern int sMgResultsBannerY;
extern int sMgResultsBannerX;
extern int sSmartballOptionNcgByLanguage[];
extern int func_02054d88(void);
extern void SetSubBg1Offset(int, int);
extern void Mg_RestoreMusic(void);
extern void Mg_SetSharedState(int);
}

extern "C" {
void GetObjectDelta(void *a, char *b);
void cMgSmartball_object_c_Construct(char *c, int a1, int a2, int *a3);
}

#define FIX_MUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))
#define L LAUNDER_A
#define M LAUNDER_B
#define N LAUNDER_C
#define FX(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))
#define LP(e) ((int *)(e))
#define FIX_MUL_SU(a, b) \
    ((int)(((s64)((u64)(s64)(a) * (b)) + 0x800) >> 12))
#define FIX_MUL_US(a, b) \
    ((int)(((s64)((a) * (u64)(s64)(b)) + 0x800) >> 12))


// @symbol _ZN16dScMgSmartball_cD1Ev
// @symbol _ZN16dScMgSmartball_cD0Ev
/* One definition, both variants. The legacy shard tree carried D0 and D1 as two
   separate files with byte-identical bodies, which is an artefact of how those
   shards were cut, not of the ROM: mwccarm emits the complete-object destructor
   (D1) and the deleting destructor (D0) from this single out-of-line
   definition, so writing the body twice is an `object redefined` error.

   D1 is the body below -- the four explicit teardowns reproduce the ROM's own
   body (func_ov006_0210d740, pre-migration) exactly, destroying the
   arrays in reverse of the factory's construction order. The own-vtable store
   and the chain to ~dScMgBase_c() are compiler-generated; dScMgBase_c's
   destructor remains an out-of-line call under the pinned 2004/b56 build,
   like the ROM's bl to _ZN11dScMgBase_cD2Ev.

   D0 is that same teardown followed by the heap return, through an inline
   `operator delete`, which is why nothing below mentions a heap. Nobody writes
   D0; declaring the destructor is enough. */
dScMgSmartball_c::~dScMgSmartball_c() {
    __cxa_vec_cleanup(mSparks, 0x40, 0x24, (void *)SmartballObj_ElemDtor);
    __cxa_vec_cleanup(mDustVel, 0x10, 8, (void *)NullDestructor_0203d47c);
    __cxa_vec_cleanup(mDustPos, 0x10, 8, (void *)NullDestructor_0203d47c);
    __cxa_vec_cleanup(mPopupPos, 5, 8, (void *)NullDestructor_0203d47c);
}
// @symbol SmartballObj_ElemDtor
extern "C" { /* .c-derived member: C linkage for the whole block */
void SmartballObj_ElemDtor(void) {}
}

// @symbol _ZN19cMgSmartball_wing_c9IsSettledEv
int cMgSmartball_wing_c::IsSettled() {
    if (mTriggerCount == 0)
        return 1;
    return GetAngle() == 0x3000;
}

// @symbol _ZN19cMgSmartball_wing_c7TriggerEv
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

// @symbol _ZN19cMgSmartball_wing_c12RebuildVertsEv
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

    func_0203d388((V2 *)&mVertices[0], (s16)-GetAngle());
    func_0203d388((V2 *)&mVertices[1], (s16)-GetAngle());
    func_0203d388((V2 *)&mVertices[2], (s16)-GetAngle());
    func_0203d388((V2 *)&mVertices[3], (s16)-GetAngle());
    func_0203d388((V2 *)&mVertices[4], GetAngle());
    func_0203d388((V2 *)&mVertices[5], GetAngle());
    func_0203d388((V2 *)&mVertices[6], GetAngle());
    func_0203d388((V2 *)&mVertices[7], GetAngle());

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

// @symbol _ZN19cMgSmartball_wing_c7HitTestEPi
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

// @symbol _ZN19cMgSmartball_wing_c6UpdateEv
/* Slot 1. While mTriggerCount is 0 or 1, draws two extra base sprites under the
 * main pair; then four OAM::Render calls draw a mirrored quad using the
 * eased angle at offset 0x32 (a BASE field -- see the header) to
 * spread each half away from vertical. */
void cMgSmartball_wing_c::Update() {
    if (mTriggerCount <= 1) {
        Mg_RenderSpriteBoth(sSmartballWingBodySprites[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        Mg_RenderSpriteBoth(sSmartballWingBodySprites[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5,
                            -1, 2);
    }
    OAM::Render(0, sSmartballWingSprites[0], (mCurrent0 >> 12) - 0x10, (mCurrent1 >> 12) + 0x118,
                -1, 1, 0x1000, 0x1000, (unsigned short)(short)(-0x4000 - GetAngle()), -1);
    OAM::Render(0, sSmartballWingSprites[0], (mCurrent0 >> 12) + 0x10, (mCurrent1 >> 12) + 0x118,
                -1, 1, 0x1000, 0x1000, (unsigned short)(short)(GetAngle() - 0x4000), -1);
    OAM::Render(0, sSmartballWingSprites[1], (mCurrent0 >> 12) - 0x10, (mCurrent1 >> 12) + 0x11d,
                -1, 2, 0x1000, 0x1000, (unsigned short)(short)(-0x4000 - GetAngle()), -1);
    OAM::Render(0, sSmartballWingSprites[1], (mCurrent0 >> 12) + 0x10, (mCurrent1 >> 12) + 0x11d,
                -1, 2, 0x1000, 0x1000, (unsigned short)(short)(GetAngle() - 0x4000), -1);
}

// @symbol _ZN19cMgSmartball_wing_c12SaveSnapshotEv
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family (the
 * base's copy is out-of-line and there is no bl here). Then, unless mAngleSettled
 * is 1 or mTriggerCount is not positive, eases the angle at offset 0x32 (see the
 * header -- it belongs to the BASE, not this class) toward
 * 0x3000 by steps of 0x200, clamping on overshoot in either direction.
 * Always tail-calls the shared helper RebuildVerts, which touches
 * the eight collision vertices at 0x48..0x87. */
void cMgSmartball_wing_c::SaveSnapshot() {
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mAngleSettled != 1 && mTriggerCount > 0) {
        if (*(s16 *)((char *)this + 0x32) < 0x3000) {
            (*(s16 *)((char *)this + 0x32)) += 0x200;
            if (*(s16 *)((char *)this + 0x32) >= 0x3000)
                *(s16 *)((char *)this + 0x32) = 0x3000;
        } else {
            (*(s16 *)((char *)this + 0x32)) -= 0x200;
            if (*(s16 *)((char *)this + 0x32) < 0x3000)
                *(s16 *)((char *)this + 0x32) = 0x3000;
        }
    }
    RebuildVerts();
}

// @symbol _ZN19cMgSmartball_wing_c14RestoreInitialEv
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then
 * zeroes the pad-region angle at 0x32 (see the header) and every unk_
 * field this class adds, in the ROM's own (non-address-order) sequence --
 * 0x34, then 0x3c, then 0x38, then 0x40, then 0x44. Tail-calls the shared
 * helper RebuildVerts, same as SaveSnapshot. */
void cMgSmartball_wing_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    *(unsigned short *)((char *)this + 0x32) = 0;
    mTriggerTime = 0;
    mTriggerFlag = 0;
    mTriggerPhase = 0;
    mTriggerCount = 0;
    mAngleSettled = 0;
    RebuildVerts();
}

// @symbol cMgSmartball_wing_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
char *cMgSmartball_wing_c_Construct(char *self, int x, int y, int *src) {
    int v[2];
    v[0] = src[0];
    v[1] = src[1];
    cMgSmartball_object_c_Construct(self, x, y, v);
    *(void **)(self) = (void *)&_ZTV19cMgSmartball_wing_c[2];
    ((cMgSmartball_object_c *)self)->mRadius = 0x18000;
    *(short *)(self + 0x32) = 0;
    return self;
}
}

// @symbol _ZN25cMgSmartball_pushswitch_c10BallsClearEv
int cMgSmartball_pushswitch_c::BallsClear() {
    int i;
    Vec2 delta, pos;
    for (i = 0; i < ((dScMgSmartball_c *)mpManager)->mBallCount; i++) {
        int *o = (i >= 13) ? 0 : (int *)((dScMgSmartball_c *)mpManager)->mpBalls[i];
        if (((cMgSmartball_object_c *)o)->mIsActive != 0) {
            int *o2 = (i >= 13) ? 0 : (int *)((dScMgSmartball_c *)mpManager)->mpBalls[i];
            CopyObjectPos((int *)&pos, o2);
            delta = pos;
            delta.x -= mCurrent0;
            delta.y -= mCurrent1;
            if (Vec2_Len(&delta) < 0x12000)
                return 0;
        }
    }
    return 1;
}

// @symbol _ZN25cMgSmartball_pushswitch_c9CheckBallEPi
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
        ((dScMgSmartball_c *)mpManager)->mpSlot->StartSpin();
        mTriggered = 1;
        mReleaseDelay = 0xa;
        Sound::PlayBank2_2D(0x1a4);
    }
    return 1;
}

// @symbol _ZN25cMgSmartball_pushswitch_c6UpdateEv
/* Slot 1. Draws two OAM sprites: the switch itself, selected by the base's
 * mTriggered flag (0/1 used directly as the table index), and a fixed second
 * frame beneath it. */
void cMgSmartball_pushswitch_c::Update() {
    RenderOamBothScreens(sSmartballPushSwitchSprites[mTriggered], mCurrent0 >> 12, mCurrent1 >> 12,
                         -1, 1, 0);
    RenderOamBothScreens(sSmartballPushSwitchSprites[2], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1,
                         2, 0);
}

// @symbol _ZN25cMgSmartball_pushswitch_c12SaveSnapshotEv
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family. Then
 * decrements this class's own countdown (mReleaseDelay) while it is positive.
 * While the base's mTriggered flag is set, calls the out-of-scope helper
 * cMgSmartball_pushswitch_c::BallsClear (an escape/proximity check over the actor list): if
 * it reports clear and the countdown has drained to zero, mTriggered is
 * cleared; otherwise the countdown is reloaded to 0xa. */
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

// @symbol _ZN25cMgSmartball_pushswitch_c14RestoreInitialEv
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then
 * clears the base's mTriggered flag and this class's own countdown. */
void cMgSmartball_pushswitch_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mTriggered = 0;
    mReleaseDelay = 0;
}

// @symbol cMgSmartball_pushswitch_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
int cMgSmartball_pushswitch_c_Construct(char *c, int b, int d, int *s) {
    int tmp[2];
    tmp[0] = s[0];
    tmp[1] = s[1];
    cMgSmartball_object_c_Construct(c, b, d, tmp);
    *(int *)c = (int)&_ZTV25cMgSmartball_pushswitch_c[2];
    ((cMgSmartball_object_c *)c)->mRadius = 0x8000;
    return (int)c;
}
}

// @symbol _ZN20cMgSmartball_board_c13AnyLineActiveEv
int cMgSmartball_board_c::AnyLineActive() {
    int i;
    for (i = 0; i < 8; i++) {
        if (mLineTimer[i] > 0x168)
            return 1;
    }
    return 0;
}

// @symbol _ZN20cMgSmartball_board_c6UpdateEv
/* Slot 1. Draws the board. Three passes:
 *
 * 1. The eight lines (mLineTimer timers / mLineScored flags). A line is drawn while
 *    its timer is running, or once the manager's 0x4660 flag is up and the
 *    line's own flag is set. The banner sprite is picked by GetOwnerLanguage,
 *    which is why the same five-way chain repeats four times. Each case also
 *    lights that line's three cells in mCellHighlight, and those triples are what
 *    prove the indexing: 0-2 are the columns {i, i+3, i+6}, 3-5 the rows
 *    {3(i-3)..3(i-3)+2}, 6 the diagonal {0,4,8} and 7 the antidiagonal
 *    {2,4,6}.
 *
 * 2. The nine cells. A cell's angle in mCellFlipAngle picks its face sprite (past
 *    0x4000 it has flipped), a lit cell blinks on bit 0x20 of mFanfareTimer, and a
 *    cell mid-flip is sheared with a matrix built from the shared sine table
 *    data_02082214 -- edge-on at +-0x4000 it degenerates to the flat sprite.
 *
 * 3. The nine slot markers along the bottom, in the order the shuffle table
 *    sSmartballCellOrder currently holds.
 *
 * No raw offsets are needed anywhere here: this slot never touches the base's
 * 0x31-0x33 span, so unlike SaveSnapshot and RestoreInitial it reaches
 * everything through real member names. */
void cMgSmartball_board_c::Update() {
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < 8; i++) {
        if (mLineTimer[i] > 0 ||
            (((dScMgSmartball_c *)mpManager)->mPhase != 0 && mLineScored[i] == 1)) {
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
                vec[0] = -cstd::fdiv(0x1000, data_02082214[((u16)(ang + 0x8000) >> 4) * 2 + 1]);
            else
                vec[0] = -cstd::fdiv(0x1000, data_02082214[((u16)ang >> 4) * 2 + 1]);
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

// @symbol _ZN20cMgSmartball_board_c9PaintCellEiPi
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
            p = (char *)G2S::GetBG3ScrPtr();
            t1 = tile | 0xf000;
            MultiStore16(t1, p + row + cx * 2 + kk, layer);
            kk += 2;
            jj++;
        } while (jj <= 0);
        b0 += 0x20;
        b1 += 0x20;
    }

    if (idx == 0 || (&mTriggered)[sSmartballCellOrder[idx - 1]] == 1) {
        base = 0x33f;
    } else {
        base = 0x33d;
    }

    {
        int lay2 = 2;
        int i2;
        int bas2 = base;
        for (i2 = -1; i2 <= 1; i2++) {
            p = (char *)G2S::GetBG3ScrPtr() + ((cy + i2) << 6);
            t2 = (bas2 + 0x20 * i2) | 0xf000;
            MultiStore16(t2, p + cx * 2 - 4, lay2);
        }
    }

    if (idx == 8) {
        int lay3 = 2;
        int i3;
        int bas3 = 0x336;
        for (i3 = -1; i3 <= 0; i3++) {
            p = (char *)G2S::GetBG3ScrPtr() + ((cy + i3) << 6);
            t3 = (bas3 + 0x20 * i3) | 0xf000;
            MultiStore16(t3, p + cx * 2 + 2, lay3);
        }
        return;
    }

    if ((&mTriggered)[sSmartballCellOrder[idx + 1]] == 1) {
        int lay4 = 2;
        int i4;
        int bas4 = 0x33f;
        for (i4 = -1; i4 <= 1; i4++) {
            p = (char *)G2S::GetBG3ScrPtr() + ((cy + i4) << 6);
            t4 = (bas4 + 0x20 * i4) | 0xf000;
            MultiStore16(t4, p + cx * 2 + 2, lay4);
        }
        return;
    }

    {
        int lay5 = 2;
        int i5;
        int bas5 = 0x33e;
        for (i5 = -1; i5 <= 1; i5++) {
            p = (char *)G2S::GetBG3ScrPtr() + ((cy + i5) << 6);
            t5 = (bas5 + 0x20 * i5) | 0xf000;
            MultiStore16(t5, p + cx * 2 + 2, lay5);
        }
    }
}

// @symbol _ZN20cMgSmartball_board_c9ClaimCellEi
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
    if ((&mTriggered)[slot] == 1)
        return;
    (&mTriggered)[slot] = 1;
    v = i * 0x18000 + 0x10000;
    a[0] = v;
    a[1] = 0x78000;
    ((dScMgSmartball_c *)mpManager)->EmitScore((int *)a, 0x64, 0, 1);
    ((dScMgSmartball_c *)mpManager)->CountCell();
    b[0] = v;
    b[1] = 0x78000;
    PaintCell(i, (int *)b);
    Sound::PlayBank2_2D(0x173);
    count = 0;
    if (slot < 3) {
        if ((&mTriggered)[slot + 3] == 1 && (&mTriggered)[slot + 6] == 1) {
            mLineTimer[slot] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot] = 1;
            count += 1;
            mLinesScored++;
        }
    } else if (slot < 6) {
        if ((&mTriggered)[slot - 3] == 1 && (&mTriggered)[slot + 3] == 1) {
            mLineTimer[slot - 3] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot - 3] = 1;
            count += 1;
            mLinesScored++;
        }
    } else {
        if ((&mTriggered)[slot - 6] == 1 && (&mTriggered)[slot - 3] == 1) {
            mLineTimer[slot - 6] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot - 6] = 1;
            count += 1;
            mLinesScored++;
        }
    }
    i = slot % 3;
    if (i == 0) {
        if ((&mTriggered)[slot + 1] == 1 && (&mTriggered)[slot + 2] == 1) {
            mLineTimer[slot / 3 + 3] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot / 3 + 3] = 1;
            count += 1;
            mLinesScored++;
        }
    } else if (i == 1) {
        if ((&mTriggered)[slot - 1] == 1 && (&mTriggered)[slot + 1] == 1) {
            mLineTimer[slot / 3 + 3] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot / 3 + 3] = 1;
            count += 1;
            mLinesScored++;
        }
    } else {
        if ((&mTriggered)[slot - 2] == 1 && (&mTriggered)[slot - 1] == 1) {
            mLineTimer[slot / 3 + 3] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[slot / 3 + 3] = 1;
            count += 1;
            mLinesScored++;
        }
    }
    switch (slot) {
    case 0:
        if ((&mTriggered)[4] == 1 && (&mTriggered)[8] == 1) {
            mLineTimer[6] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[6] = 1;
            count += 1;
            mLinesScored++;
        }
        break;
    case 2:
        if ((&mTriggered)[4] == 1 && (&mTriggered)[6] == 1) {
            mLineTimer[7] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[7] = 1;
            count += 1;
            mLinesScored++;
        }
        break;
    case 4:
        if ((&mTriggered)[0] == 1 && (&mTriggered)[8] == 1) {
            mLineTimer[6] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[6] = 1;
            count += 1;
            mLinesScored++;
        }
        if ((&mTriggered)[2] == 1 && (&mTriggered)[6] == 1) {
            mLineTimer[7] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[7] = 1;
            count += 1;
            mLinesScored++;
        }
        break;
    case 6:
        if ((&mTriggered)[2] == 1 && (&mTriggered)[4] == 1) {
            mLineTimer[7] = 0x21d;
            mFanfareTimer = 0xb5;
            mLineScored[7] = 1;
            count += 1;
            mLinesScored++;
        }
        break;
    case 8:
        if ((&mTriggered)[0] == 1 && (&mTriggered)[4] == 1) {
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
            if ((&mTriggered)[j] == 0) {
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
            ((dScMgSmartball_c *)mpManager)->EmitScore((int *)d, (int)tmp, (int)0, (int)1);
            ((dScMgSmartball_c *)mpManager)->CountLine(1);
            m++;
        } while (m < count);
    }
    if (bingo != 1)
        return;
    e[0] = mCurrent0;
    e[1] = mCurrent1;
    ((dScMgSmartball_c *)mpManager)->EmitScore((int *)e, 0x2710, 0x3c, 1);
    ((dScMgSmartball_c *)mpManager)->CountBingo();
    ((dScMgSmartball_c *)mpManager)->EndRound();
    Sound::PlayBank2_2D(0x174);
}

// @symbol _ZN20cMgSmartball_board_c12SaveSnapshotEv
#pragma push
#pragma opt_strength_reduction off
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family (the
 * base's copy is out-of-line and there is no bl here).
 *
 * Then: spend the countdown at mFanfareTimer and, once a second, replay the jingle
 * selected by mLinesScored; age the eight per-line timers in mLineTimer; scan the
 * shared manager's up-to-13 tracked balls for one sitting in the board's
 * horizontal strip, map its x to one of the nine cells through the shuffle
 * table sSmartballCellOrder and claim that cell; finally spin every claimed
 * cell's angle in mCellFlipAngle up to its 0x8000 stop.
 *
 * The nine cell bytes are reached as (&mTriggered)[k] -- the array begins at the
 * BASE's mTriggered and runs into this class's mCellsTail[6], so no member can name
 * it. See cMgSmartball_board_c.h. */
static inline cMgSmartball_ball_c *GetObjBoard(char *g, int i) {
    return i >= 13 ? 0 : ((dScMgSmartball_c *)g)->mpBalls[i];
}

static inline void SetV2(V2 *p, int x, int y) {
    p->x = x;
    p->y = y;
}

/* Load-bearing for this spelling: without it the function is 5 words too long.
 * Not a property of the function itself -- the same bytes come out with
 * different statement order and no pragma at all. */
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
    for (n = 0; n < ((dScMgSmartball_c *)mpManager)->mBallCount; n++) {
        if (((dScMgSmartball_c *)mpManager)->mDraining != 0)
            break;
        if (GetObjBoard((char *)mpManager, n)->mIsActive == 0)
            continue;
        CopyObjectPos((int *)&t, (int *)GetObjBoard((char *)mpManager, n));
        /* COMPILER STEER, and the only one in this file. `v = t;` is what the
           ROM does and it is what the pre-migration C said -- but mwcc
           scalarizes a whole-struct assignment in C++ where it kept it in
           memory in C, which deletes v's stack slot, its two stores and the
           reload of v.x in the loop below: 0x3a4 bytes instead of 0x3b0.
           Casting the DESTINATION alone restores the memory-to-memory copy;
           casting the source instead does nothing. Measured, not guessed --
           six other spellings (memberwise, copy-init, a reference alias, an
           sret-returning helper, a non-POD V2, a V2 with a base) were all
           tried and all diverge. See launder.h for the family this belongs
           to. */
        *(V2 *)&v = t;
        if (v.y < 0x74000)
            continue;
        if (v.y >= 0x7c000)
            continue;
        for (j = 0, lo = 0xc000, hi = 0x14000; j < 9; j++) {
            int slot = sSmartballCellOrder[j];
            if (v.x >= lo && v.x < hi) {
                if (GetObjBoard((char *)mpManager, n)->ArmExpire(0x14)) {
                    if ((&mTriggered)[slot] == 0) {
                        SetV2(&a, mCurrent0 + ((sSmartballCellOrder[j] % 3 - 1) * 0x18 << 12),
                              mCurrent1 + ((sSmartballCellOrder[j] / 3 - 1) * 0x18 << 12));
                        ((dScMgSmartball_c *)((char *)mpManager))->EmitSparks((int *)&a, 0);
                        SetV2(&b, (j * 0x18 + 0x10) << 12, 0x78000);
                        ((dScMgSmartball_c *)((char *)mpManager))->EmitSparks((int *)&b, 0);
                        func_02012718(0x1be, j * 0x18000 + 0x10000);
                    } else {
                        SetV2(&d, (j * 0x18 + 0x10) << 12, 0x78000);
                        ((dScMgSmartball_c *)((char *)mpManager))->EmitSparks((int *)&d, 1);
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
        if ((&mTriggered)[i] == 1) {
            mCellFlipAngle[i] += 0x400;
            if (mCellFlipAngle[i] >= 0x8000)
                mCellFlipAngle[i] = 0x8000;
        }
    }
}

#pragma pop

// @symbol _ZN20cMgSmartball_board_c14RestoreInitialEv
/* Slot 2. Unlike most of this family's slot 2s this is a real body, not a
 * tail-call veneer -- but it still OPENS with the qualified call to the base,
 * and the qualification is load-bearing exactly as it is in the veneers: an
 * unqualified RestoreInitial() here would dispatch through the vtable and
 * recurse.
 *
 * After that it refills the shared shuffle table sSmartballCellOrder with the
 * identity 0..8, reshuffles it via ShuffleCells, and then zeroes every
 * field this class owns. That zero pass is exhaustive and in address order,
 * which is why it -- not a union of the "obvious" accesses in the other two
 * slots -- is the authority for the layout in cMgSmartball_board_c.h.
 *
 * The nine cell bytes are reached as (&mTriggered)[i] -- pointer arithmetic off
 * the BASE's first cell byte, running through unk_032/unk_033 and on into
 * this class's mCellsTail[6]. No member can name that array: it begins three
 * bytes inside the base and a member cannot straddle the boundary. This is
 * the only spelling that reproduces; five others were measured and every one
 * of them compiled the wrong addressing mode or let strength reduction build
 * an induction variable the ROM does not have. See the header. */
void cMgSmartball_board_c::RestoreInitial() {
    int i;
    cMgSmartball_object_c::RestoreInitial();
    for (i = 0; i < 9; i++)
        sSmartballCellOrder[i] = i;
    ShuffleCells();
    for (i = 0; i < 9; i++) {
        (&mTriggered)[i] = 0;
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

// @symbol _ZN20cMgSmartball_board_c12ShuffleCellsEv
void cMgSmartball_board_c::ShuffleCells() {
    struct W9 tmp;
    int i;
    tmp = sSmartballCellStartOrder;
    for (i = 0; i < 9; i++) {
        sSmartballCellOrder[i] = tmp.w[i] - 1;
    }
}

// @symbol cMgSmartball_board_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
int cMgSmartball_board_c_Construct(char *c, int b, int d, int *s) {
    int tmp[2];
    tmp[0] = s[0];
    tmp[1] = s[1];
    cMgSmartball_object_c_Construct(c, b, d, tmp);
    *(int *)c = (int)&_ZTV20cMgSmartball_board_c[2];
    ((cMgSmartball_object_c *)c)->mRadius = 0x18000;
    return (int)c;
}
}

// @symbol _ZN19cMgSmartball_slot_c12ReelsStoppedEv
int cMgSmartball_slot_c::ReelsStopped() {
    int i;
    for (i = 0; i < 3; i++) {
        if (mReelVel[i].y > 0)
            return 0;
    }
    return 1;
}

// @symbol _ZN19cMgSmartball_slot_c9BeginStopEv
void cMgSmartball_slot_c::BeginStop() {
    /* The stop counts are written through this copy of `this`: spelled
       mReelStopCount[i] directly, the loop is 9 words shorter than the ROM's. */
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
            ((((((unsigned)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) * 3) >> 15) + 3;
    }
}

// @symbol _ZN19cMgSmartball_slot_c9StartSpinEv
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

// @symbol _ZN19cMgSmartball_slot_c10UpdateSpinEv
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void cMgSmartball_slot_c::UpdateSpin() {
    int ib, ic;

    if (mIsSpinning == 1) {
        if (mIsSpeedingUp == 1) {
            int flag = 1;
            int ia;
            for (ia = 0; ia < 3; ia++) {
                int r = RandomIntInternal(&data_0209d4b8);
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
                        int r = RandomIntInternal(&data_0209d4b8);
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
                ((dScMgSmartball_c *)mpManager)->EmitScore((int *)&a, 0x1388, 0, 1);

                ((dScMgSmartball_c *)mpManager)->CountAward(0);

                break;
            }
            case 0x10000: {
                V2 b;
                b.x = mCurrent0;
                b.y = mCurrent1;
                ((dScMgSmartball_c *)mpManager)->EmitScore((int *)&b, 0x1f40, 0, 1);

                ((dScMgSmartball_c *)mpManager)->CountAward(1);

                break;
            }
            case 0x20000: {
                V2 d;
                d.x = mCurrent0;
                d.y = mCurrent1;
                ((dScMgSmartball_c *)mpManager)->EmitScore((int *)&d, 0x2710, 0, 1);

                ((dScMgSmartball_c *)mpManager)->CountAward(2);

                break;
            }
            }
            Sound::PlayBank2_2D(0x1a7);
        }
    }
    mSpinCooldown = 0x3c;
}

#pragma pop

// @symbol _ZN19cMgSmartball_slot_c6UpdateEv
/* Slot 1. First loop draws three copies of one sprite in a row (index i is
 * computed but never read -- only the running x offset matters). Second
 * loop, over the same three mReelOffset[i] pairs, draws a stack of four more
 * sprites per element with descending y. Kept identical to the pre-migration
 * source, including the `if (1) {}` no-op, the
 * (x - 0x18) & 0xFFFFFFFFFFFFFFFF mask, and the goto-based second loop --
 * only the raw offset accesses became real member access. */
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

// @symbol _ZN19cMgSmartball_slot_c12SaveSnapshotEv
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family (the
 * base's copy is out-of-line and there is no bl here). Then runs three
 * independent countdowns: mSpinCooldown always decrements while positive;
 * mSpinsQueued decrements and fires the spin-start helper StartSpin
 * once mIsSpinning is clear and mSpinCooldown has drained; mSpinTimer decrements and
 * fires BeginStop when it reaches exactly 0. Always tail-calls
 * the shared per-frame helper UpdateSpin. */
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

// @symbol _ZN19cMgSmartball_slot_c14RestoreInitialEv
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then
 * exhaustively zeroes every field this class adds, in the ROM's own
 * per-index-then-next-field order for the two 3-element arrays (A[i].a,
 * A[i].b, B[i].a, B[i].b, C[i], D[i] before moving to i+1), followed by
 * the four trailing scalars in address order. This function is the
 * strongest evidence for the field list in the header. */
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

// @symbol cMgSmartball_slot_c_Construct
extern "C" {
void *cMgSmartball_slot_c_Construct(char *c, int a1, int a2, int *src) {
    int tmp[2];
    tmp[0] = src[0];
    tmp[1] = src[1];
    cMgSmartball_object_c_Construct(c, a1, a2, tmp);
    *(void ***)c = (void **)&_ZTV19cMgSmartball_slot_c[2];
    __cxa_vec_ctor(((cMgSmartball_slot_c *)c)->mReelOffset, 3, 8, func_0203d738,
                   NullDestructor_0203d47c);
    __cxa_vec_ctor(((cMgSmartball_slot_c *)c)->mReelVel, 3, 8, func_0203d738,
                   NullDestructor_0203d47c);
    ((cMgSmartball_object_c *)c)->mRadius = 0x28000;
    return c;
}
}

// @symbol _ZN21cMgSmartball_pakkun_c9CheckBallEPi
/* Ball-vs-piranha collision test. While mActionTimer has run out it tests the
 * ball against the plant's two jaw triangles either side of mCurrent; while
 * the plant is mid-action it tests the ring between the two radii. */
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
                r = PointInTriangle((int *)&p, (int *)&t1, (int *)&t2, (int *)&t3);
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
                r = PointInTriangle((int *)&p2, (int *)&t4, (int *)&t5, (int *)&t6);
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

// @symbol _ZN21cMgSmartball_pakkun_c6UpdateEv
/* Slot 1. While mActionTimer is counting down it renders a rotate/scale pair
 * of frames (the shift-by-12 fixed-point matrix built from data_02082214);
 * otherwise it renders one of two idle frames selected by mTriggered, plus a
 * common third frame beneath it. */
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
                scale = cstd::fdiv(0x20000, f38 << 12);
            } else {
                scale = 0x1000;
                if ((f38 - 0x20) % 16 < 4)
                    idx = 0;
            }
            b = (int)((((long long)data_02082214[1] * scale) + 0x800) >> 12);
            a = (int)((((long long)data_02082214[0] * scale) + 0x800) >> 12);
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

// @symbol _ZN21cMgSmartball_pakkun_c12SaveSnapshotEv
#pragma push
#pragma opt_strength_reduction off
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same idiom as every sibling. Then, while
 * mActionTimer is still counting down from a previous trigger, it just
 * spends the timer and returns without scanning. Otherwise it scans the
 * shared manager's up-to-13 tracked objects for one within a fixed-point
 * distance of 0x4000 and, on a hit, arms mActionTimer, flips mTriggered
 * off, fires an effect, deactivates the object it found, and returns. If
 * the whole scan comes up empty, mIdleTimer counts the miss and wraps at
 * 0x3c, toggling mTriggered. */
inline cMgSmartball_ball_c *GetObjPakkun(char *g, int i) {
    return i >= 13 ? 0 : ((dScMgSmartball_c *)g)->mpBalls[i];
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
    for (i = 0; i < ((dScMgSmartball_c *)mpManager)->mBallCount; i++) {
        CopyObjectPos(&s[0], (int *)GetObjPakkun((char *)mpManager, i));
        Vec2_Sub(&s[2], &mCurrent0, &s[0]);
        b = Vec2_Len((Vec2 *)&s[2]) < 0x4000;
        if (b) {
            mActionTimer = 0x61;
            mTriggered = 0;
            ((dScMgSmartball_c *)mpManager)->mpWing->Trigger();
            GetObjPakkun((char *)mpManager, i)->mIsActive = 0;
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

#pragma pop

// @symbol _ZN21cMgSmartball_pakkun_c14RestoreInitialEv
/* Slot 2. A real override, not a tail-call veneer: it calls the base's
 * RestoreInitial() (qualified -- unqualified would dispatch through the
 * vtable and recurse) and then also zeroes mTriggered and this child's own
 * two fields. */
void cMgSmartball_pakkun_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mTriggered = 0;
    mIdleTimer = 0;
    mActionTimer = 0;
}

// @symbol cMgSmartball_pakkun_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
int cMgSmartball_pakkun_c_Construct(char *c, int b, int d, int *s) {
    int tmp[2];
    tmp[0] = s[0];
    tmp[1] = s[1];
    cMgSmartball_object_c_Construct(c, b, d, tmp);
    *(int *)c = (int)&_ZTV21cMgSmartball_pakkun_c[2];
    ((cMgSmartball_object_c *)c)->mRadius = 0x14000;
    return (int)c;
}
}

// @symbol _ZN20cMgSmartball_dokan_c9CheckBallEPi
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

// @symbol _ZN20cMgSmartball_dokan_c6UpdateEv
/* Slot 1. Draws the pipe as five stacked sprites -- one cap, three body
 * segments, one base -- from a three-entry sprite table, all at the same x
 * with y stepping 0x10 units apart in screen space (the >> 12 is the fixed
 * point conversion). The last argument is a flag lifted from the scene. */
void cMgSmartball_dokan_c::Update() {
    int flag = 0;
    if (((dScMgSmartball_c *)mpManager)->mPhase)
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

// @symbol _ZN20cMgSmartball_dokan_c12SaveSnapshotEv
#pragma push
#pragma opt_strength_reduction off
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- the base's is out-of-line at 0x02114724 and there is
 * no bl here, so the original repeated the two statements. Every sibling's
 * slot 0 opens the same way. */
inline cMgSmartball_ball_c *GetObjDokan(char *g, int i) {
    return i >= 13 ? 0 : ((dScMgSmartball_c *)g)->mpBalls[i];
}

void cMgSmartball_dokan_c::SaveSnapshot() {
    int i;
    W s;

    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    for (i = 0; i < ((dScMgSmartball_c *)mpManager)->mBallCount; i++) {
        if (GetObjDokan((char *)mpManager, i)->mIsActive == 0)
            continue;
        if (GetObjDokan((char *)mpManager, i)->mPipeUsed == 1)
            continue;
        CopyObjectPos((int *)&s.v, (int *)GetObjDokan((char *)mpManager, i));
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
        ((dScMgSmartball_c *)mpManager)->EmitScore((int *)&s.e, 0xbb8, 0, 1);
        ((dScMgSmartball_c *)((char *)mpManager))->CountDokan();
        GetObjDokan((char *)mpManager, i)->mPipeUsed = 1;
        Sound::PlayBank2_2D(0x16);
    }
}

#pragma pop

// @symbol _ZN20cMgSmartball_dokan_c14RestoreInitialEv
/* Slot 2. The ROM is three words -- ldr ip, [pc]; bx ip; .word target -- which
 * is mwcc's interworking-safe TAIL CALL, not a linker veneer and not a
 * "this class does not override" vtable entry. A class that simply inherited
 * the slot would have the base's address sitting in the vtable directly; this
 * one has its own symbol, so the original declared the override and its whole
 * body was the qualified call below. The qualification is load-bearing: an
 * unqualified RestoreInitial() here would dispatch through the vtable and
 * recurse. */
void cMgSmartball_dokan_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
}

// @symbol cMgSmartball_dokan_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
int cMgSmartball_dokan_c_Construct(char *c, int b, int d, int *s) {
    int tmp[2];
    tmp[0] = s[0];
    tmp[1] = s[1];
    cMgSmartball_object_c_Construct(c, b, d, tmp);
    *(int *)c = (int)&_ZTV20cMgSmartball_dokan_c[2];
    ((cMgSmartball_object_c *)c)->mRadius = 0x20000;
    return (int)c;
}
}

// @symbol _ZN24cMgSmartball_propeller_c7HitTestEPi
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
    if (PointInTriangle((int *)&s0, (int *)&s1, (int *)&s2, (int *)&s3))
        return 1;

    t0.a = p->a;
    t0.b = p->b;
    t1.a = mVertices[3].x;
    t1.b = mVertices[3].y;
    t2.a = mVertices[1].x;
    t2.b = mVertices[1].y;
    t3.a = mVertices[2].x;
    t3.b = mVertices[2].y;
    if (PointInTriangle((int *)&t0, (int *)&t1, (int *)&t2, (int *)&t3))
        return 1;

    u0.a = p->a;
    u0.b = p->b;
    u1.a = mVertices[4].x;
    u1.b = mVertices[4].y;
    u2.a = mVertices[5].x;
    u2.b = mVertices[5].y;
    u3.a = mVertices[6].x;
    u3.b = mVertices[6].y;
    if (PointInTriangle((int *)&u0, (int *)&u1, (int *)&u2, (int *)&u3))
        return 1;

    v0.a = p->a;
    v0.b = p->b;
    v1.a = mVertices[7].x;
    v1.b = mVertices[7].y;
    v2.a = mVertices[5].x;
    v2.b = mVertices[5].y;
    v3.a = mVertices[6].x;
    v3.b = mVertices[6].y;
    return PointInTriangle((int *)&v0, (int *)&v1, (int *)&v2, (int *)&v3) != 0;
}

/* The base's angle at 0x32 read signed, as RebuildVerts needs it. Update reads
   the same two bytes unsigned and keeps its own cast. */
inline s16 cMgSmartball_propeller_c::GetAngle() const {
    return *(const s16 *)((const char *)this + 0x32);
}

// @symbol _ZN24cMgSmartball_propeller_c12RebuildVertsEv
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

    func_0203d388((V2 *)&mVertices[0], GetAngle());
    func_0203d388((V2 *)&mVertices[1], GetAngle());
    func_0203d388((V2 *)&mVertices[2], GetAngle());
    func_0203d388((V2 *)&mVertices[3], GetAngle());
    func_0203d388((V2 *)&mVertices[4], GetAngle());
    func_0203d388((V2 *)&mVertices[5], GetAngle());
    func_0203d388((V2 *)&mVertices[6], GetAngle());
    func_0203d388((V2 *)&mVertices[7], GetAngle());

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

// @symbol _ZN24cMgSmartball_propeller_c6UpdateEv
/* Slot 1. Looks up a rotation matrix from the base's angle field at 0x32,
 * reading it UNSIGNED (>> 4, lsr not asr) to index the sine table -- one of
 * the three incompatible readings cMgSmartball_object_c.h records for that
 * three-byte region, so it stays a raw cast rather than the base's member
 * name: no single member type is right for all three readers. Draws two
 * OAM sprites (front and back cap) at the base's position (mCurrent0/
 * mCurrent1) using that matrix; touches nothing in this class's own 0x34+
 * region at all. */
void cMgSmartball_propeller_c::Update() {
    int b =
        (short)((((long long)data_02082214[(*(unsigned short *)((char *)this + 0x32) >> 4) * 2 + 1]
                  << 12) +
                 0x800) >>
                12);
    int a;
    int vec[4];

    vec[0] = b;
    a = (short)((((long long)data_02082214[(*(unsigned short *)((char *)this + 0x32) >> 4) * 2]
                  << 12) +
                 0x800) >>
                12);
    vec[3] = b;
    vec[1] = a;
    vec[2] = -a;

    RenderOamBothScreens(sSmartballPropellerSprites[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1,
                         vec);
    RenderOamBothScreens(sSmartballPropellerSprites[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1,
                         2, vec);
}

// @symbol _ZN24cMgSmartball_propeller_c12SaveSnapshotEv
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same idiom as every sibling. If the scene's
 * mRoundEnded flag is set, it returns before touching anything else.
 * Otherwise it steps the rotation speed (mSpinSpeed) by +-8 toward the target
 * (mSpinSpeedTarget), clamping once it crosses, adds the stepped speed onto the
 * base's own angle at 0x32 (a raw cast -- see cMgSmartball_object_c.h
 * on why that field has no single type), and calls the shared helper that
 * recomputes the propeller-blade positions from the new angle.
 *
 * The step writes through a local `short *p` taken from &mSpinSpeed while
 * every comparison re-reads the member. That pointer is load-bearing:
 * written `mSpinSpeed = mSpinSpeed + 8` the function is 0x9c bytes where
 * the ROM has 0xac, because mwcc then keeps the stepped value in a
 * register instead of reloading it. */
void cMgSmartball_propeller_c::SaveSnapshot() {
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (((dScMgSmartball_c *)mpManager)->mRoundEnded != 0)
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
        short *q = (short *)((char *)this + 0x32);
        *q = *q + mSpinSpeed;
    }
    RebuildVerts();
}

// @symbol _ZN24cMgSmartball_propeller_c14RestoreInitialEv
/* Slot 2. A real override, not a tail-call veneer: calls the base's
 * RestoreInitial() (qualified -- unqualified would dispatch through the
 * vtable and recurse), then clears the base's own angle at 0x32 (raw cast,
 * same reasoning as SaveSnapshot/Update -- see cMgSmartball_propeller_c.h),
 * resets the target rotation speed (mSpinSpeedTarget) to its default 0x40, zeroes
 * the current rotation speed (mSpinSpeed), and calls the shared helper that
 * recomputes the propeller-blade positions from the reset state. */
void cMgSmartball_propeller_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    *(short *)((char *)this + 0x32) = 0;
    mSpinSpeedTarget = 0x40;
    mSpinSpeed = 0;
    RebuildVerts();
}

// @symbol cMgSmartball_propeller_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
int cMgSmartball_propeller_c_Construct(char *c, int b, int d, int *s) {
    int tmp[2];
    tmp[0] = s[0];
    tmp[1] = s[1];
    cMgSmartball_object_c_Construct(c, b, d, tmp);
    *(int *)c = (int)&_ZTV24cMgSmartball_propeller_c[2];
    ((cMgSmartball_object_c *)c)->mRadius = 0x20000;
    return (int)c;
}
}

// @symbol _ZN21cMgSmartball_spring_c6UpdateEv
/* Slot 1. Draws one of two sprite pairs depending on a byte flag read
 * through the base's mpManager scene pointer (offset 8, low byte) -- unrelated
 * to this class's own 0x34+ fields, which Update never touches at all. */
void cMgSmartball_spring_c::Update() {
    if ((((dScMgSmartball_c *)mpManager)->param1 & 0xff) == 0) {
        Mg_RenderSpriteBoth(sSmartballSpringSprites[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        Mg_RenderSpriteBoth(sSmartballSpringSprites[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5, -1,
                            2);
    } else {
        Mg_RenderSpriteBoth(sSmartballSpringSpritesAlt[0], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
        Mg_RenderSpriteBoth(sSmartballSpringSpritesAlt[1], mCurrent0 >> 12, (mCurrent1 >> 12) + 5,
                            -1, 2);
    }
}

// @symbol _ZN21cMgSmartball_spring_c12SaveSnapshotEv
#pragma push
#pragma opt_propagation off
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling. Then a three-way
 * state machine gated by two slot-indexed byte lanes (gTouchHeld/gTouchEdge)
 * and driven by the touch x/y lanes (gTouchX/gTouchY, shifted <<12 into fixed point): trigger sets
 * mTriggered and latches mGrabXSaved/mDriveBase/mGrabX/mDriveNow; active recomputes
 * and clamps mDriveNow each call and plays a sound on change; release clears
 * mTriggered, or -- if neither holds -- eases mCurrent1 back toward baseline.
 * See cMgSmartball_spring_c.h for the field-by-field evidence.
 *
 * Three statements in the original read and wrote the SAME struct member
 * through a raw pointer cast twice, solely to keep a compound assignment
 * off it (`int *p = (int *)(c + off); *p = *p - k;`). All three are plain
 * compound assignments below and emit the same. */
void cMgSmartball_spring_c::SaveSnapshot() {
    u8 slot;
    int held;
    int flag;
    int d;

    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (((dScMgSmartball_c *)mpManager)->mRoundEnded != 0)
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

#pragma pop

// @symbol _ZN21cMgSmartball_spring_c14RestoreInitialEv
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then
 * zeroes mSoundHandle -- the ONLY offset this class's RestoreInitial reaches;
 * mGrabXSaved/mDriveBase/mGrabX/mDriveNow carry whatever SaveSnapshot last
 * left there. */
void cMgSmartball_spring_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mSoundHandle = 0;
}

// @symbol cMgSmartball_spring_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
int cMgSmartball_spring_c_Construct(char *c, int b, int d, int *s) {
    int tmp[2];
    tmp[0] = s[0];
    tmp[1] = s[1];
    cMgSmartball_object_c_Construct(c, b, d, tmp);
    *(int *)c = (int)&_ZTV21cMgSmartball_spring_c[2];
    ((cMgSmartball_object_c *)c)->mRadius = 0x20000;
    return (int)c;
}
}

// @symbol _ZN18cMgSmartball_ana_c7HasBallEv
int cMgSmartball_ana_c::HasBall() {
    return mTriggered;
}

// @symbol _ZN18cMgSmartball_ana_c7SwallowEv
void cMgSmartball_ana_c::Swallow() {
    int tmp1[2];
    int tmp2[2];
    mRespawnTimer = 0x3c;
    mTriggered = 1;
    tmp1[0] = mCurrent0;
    tmp1[1] = mCurrent1;
    ((dScMgSmartball_c *)mpManager)->EmitSparks((int *)tmp1, 0);
    tmp2[0] = mCurrent0;
    tmp2[1] = mCurrent1;
    ((dScMgSmartball_c *)mpManager)->EmitScore((int *)tmp2, (int)0x7d0, (int)0, (int)1);
    ((dScMgSmartball_c *)mpManager)->CountAna();
    func_02012718(0xe0, mCurrent0);
}

// @symbol _ZN18cMgSmartball_ana_c6UpdateEv
/* Slot 1. While the BASE's mTriggered flag is set, picks a warning-sprite
 * index from this class's countdown (mRespawnTimer) and draws it under the main
 * sprite; the main sprite is always drawn. mTriggered belongs to the base
 * class, not this one -- see cMgSmartball_ana_c.h. */
void cMgSmartball_ana_c::Update() {
    if (mTriggered == 1) {
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

// @symbol _ZN18cMgSmartball_ana_c12SaveSnapshotEv
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family (the
 * base's copy is out-of-line and there is no bl here). Then decrements this
 * class's own countdown (mRespawnTimer); when it reaches zero it calls the shared
 * helper ServeNextBall and sets the BASE's unk_032 byte to 1 -- the
 * low byte only. 0x031-0x033 is a three-byte region that three children read
 * three incompatible ways; this class's byte-wide view is one of them. See
 * cMgSmartball_object_c.h. */
void cMgSmartball_ana_c::SaveSnapshot() {
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mRespawnTimer <= 0)
        return;
    mRespawnTimer = mRespawnTimer - 1;
    if (mRespawnTimer > 0)
        return;
    ((dScMgSmartball_c *)mpManager)->ServeNextBall();
    unk_032 = 1;
}

// @symbol _ZN18cMgSmartball_ana_c14RestoreInitialEv
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then clears
 * the base's mTriggered flag, the low byte of the base's unk_032, and this
 * class's own countdown.
 *
 * Both go through the base's member names. That became possible when
 * cMgSmartball_board_c showed 0x031-0x033 to be three independent bytes
 * rather than a byte plus a halfword -- this class's byte-wide view now
 * agrees with the base's declaration, so no cast is needed. */
void cMgSmartball_ana_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mTriggered = 0;
    unk_032 = 0;
    mRespawnTimer = 0;
}

// @symbol cMgSmartball_ana_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
int cMgSmartball_ana_c_Construct(char *c, int b, int d, int *s) {
    int tmp[2];
    tmp[0] = s[0];
    tmp[1] = s[1];
    cMgSmartball_object_c_Construct(c, b, d, tmp);
    *(int *)c = (int)&_ZTV18cMgSmartball_ana_c[2];
    ((cMgSmartball_object_c *)c)->mRadius = 0x10000;
    return (int)c;
}
}

// @symbol _ZN21cMgSmartball_kinoko_c6UpdateEv
/* Slot 1. Switches on mVariant (see the header): state 0 draws two base
 * sprites via Mg_RenderSpriteBoth; state 1 computes an ease vector from the
 * base's mRadius (bails out once mRadius < 0x1000) and draws two mirrored
 * sprites via RenderOamBothScreens; there is no case for state 2, so
 * nothing renders. */
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
        d = cstd::fdiv(0x7000, mRadius);
        b = FX_MUL(data_02082214[1], d);
        a = FX_MUL(data_02082214[0], d);
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

// @symbol _ZN21cMgSmartball_kinoko_c12SaveSnapshotEv
#pragma push
#pragma opt_strength_reduction off
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same idiom as every sibling. While mWasHit is set,
 * runs a one-shot transition (an effect/sound trigger, gated on mHitTimer
 * still being 0) and reloads mHitTimer from mVariant; otherwise, while mHitTimer
 * is still counting down, decrements it and, once it drops under 4, scans a
 * fixed actor list for anything within a fixed-point distance of 0x11000,
 * arming mHitTimer back to 4 on a hit. Always ends by easing the base's
 * mRadius toward 0x7000 or 0, depending on mVariant and whether mHitTimer has
 * drained. */
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
            ((dScMgSmartball_c *)mpManager)->EmitScore((int *)&pair, (int)0x12c, (int)0, (int)1);
            ((dScMgSmartball_c *)mpManager)->CountKinoko();
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
            obj = (dScMgSmartball_c *)mpManager;
            i = 0;
            if (obj->mBallCount > 0) {
                do {
                    cMgSmartball_ball_c *n = (i >= 0xd) ? 0 : obj->mpBalls[i];
                    if (n->mIsActive != 0) {
                        void *m = (i >= 0xd) ? (void *)0 : (void *)obj->mpBalls[i];
                        int hit;
                        CopyObjectPos((int *)&bufA, (int *)m);
                        Vec2_Sub((int *)&bufB, &mCurrent0, (int *)&bufA);
                        hit = Vec2_Len((Vec2 *)(void *)&bufB) < 0x11000;
                        if (hit != 0) {
                            mHitTimer = 4;
                            goto done;
                        }
                    }
                    obj = (dScMgSmartball_c *)mpManager;
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

#pragma pop

// @symbol _ZN21cMgSmartball_kinoko_c14RestoreInitialEv
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial, qualified --
 * unqualified would dispatch virtually and recurse), then zeroes mWasHit and
 * mHitTimer. Does NOT touch mVariant -- read exhaustively, this function simply
 * never mentions it. */
void cMgSmartball_kinoko_c::RestoreInitial() {
    cMgSmartball_object_c::RestoreInitial();
    mWasHit = 0;
    mHitTimer = 0;
}

// @symbol cMgSmartball_kinoko_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
char *cMgSmartball_kinoko_c_Construct(char *self, int x, int y, int *src) {
    int v[2];
    v[0] = src[0];
    v[1] = src[1];
    cMgSmartball_object_c_Construct(self, x, y, v);
    *(void **)(self) = (void *)&_ZTV21cMgSmartball_kinoko_c[2];
    ((cMgSmartball_object_c *)self)->mRadius = 0x7000;
    ((cMgSmartball_kinoko_c *)self)->mVariant = 2;
    return self;
}
}

// @symbol _ZN19cMgSmartball_ball_c12PlayHitSoundEiPi
void cMgSmartball_ball_c::PlayHitSound(int a1, int *vel) {
    int spd;

    if (soundTimer > 0) {
        soundTimer = 3;
        return;
    }
    {
        int v[2];
        Vec2_Sub(v, &mVel0, vel);
        spd = Vec2_Len((Vec2 *)v);
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
            ((dScMgSmartball_c *)mpManager)->SpawnDust((int *)pos);
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

// @symbol _ZN19cMgSmartball_ball_c6FreezeEv
void cMgSmartball_ball_c::Freeze() {
    mFrozenPos0 = mCurrent0;
    mFrozenPos1 = mCurrent1;
    mIsFrozen = 1;
    state3b = 1;
}

// @symbol _ZN19cMgSmartball_ball_c6HasHitEv
int cMgSmartball_ball_c::HasHit() {
    return anyHit;
}

// @symbol _ZN19cMgSmartball_ball_c5IsOutEv
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

// @symbol _ZN19cMgSmartball_ball_c9ArmExpireEi
int cMgSmartball_ball_c::ArmExpire(int val) {
    if (mIsActive == 0)
        return 0;
    if (mExpireTimer > 0)
        return 0;
    mExpireTimer = val;
    return 1;
}

// @symbol _ZN19cMgSmartball_ball_c10IsFinishedEv
int cMgSmartball_ball_c::IsFinished() {
    if ((((dScMgSmartball_c *)mpManager)->param1 & 0xff) != 0 && mCurrent0 < 0xe0000 &&
        mCurrent1 >= 0x50000 && mCurrent1 < 0x88000) {
        if (mStuckFrames >= 0x78)
            return 1;
    }
    if (mInPlay == 0)
        return 1;
    return state3b;
}

// @symbol _ZN19cMgSmartball_ball_c5BurstEv
void cMgSmartball_ball_c::Burst() {
    int v[2];
    v[0] = mCurrent0;
    v[1] = mCurrent1;
    ((dScMgSmartball_c *)mpManager)->EmitSparks((int *)v, 0);
}

// @symbol _ZN19cMgSmartball_ball_c9PutInPlayEv
void cMgSmartball_ball_c::PutInPlay() {
    mInPlay = 1;
    Burst();
}

// @symbol _ZN19cMgSmartball_ball_c7HitWingEPi
/* Both reads of the scene's mpWing (0x4780) are member accesses through the
 * manager pointer, not integer arithmetic cast to a pointer. Under 2004/b56 that
 * choice decides the addressing mode: written as `*(int*)(base + 0x4000 + 0x780)`
 * b56 folds the constant, parks 0x4780 in the literal pool and emits a
 * register-offset `ldr`, while the ROM computes `add r2,r2,#0x4000` and loads
 * `[r2, #0x780]`. `ldr` carries only a 12-bit displacement, so 0x4780 cannot be
 * encoded and the address must be split either way; the member access (like the
 * `char*` pointer arithmetic it replaced) lets b56 split it the ROM's way.
 * Both reads must use it -- respelling only one leaves the other sharing.
 */
int cMgSmartball_ball_c::HitWing(int *src) {
    int g = (int)((dScMgSmartball_c *)mpManager)->mpWing;
    if (g == 0)
        return 0;
    struct V2 v;
    v.x = src[0];
    v.y = src[1];
    return ((dScMgSmartball_c *)mpManager)->mpWing->HitTest((int *)&v);
}

// @symbol _ZN19cMgSmartball_ball_c11HitSwitchesEPi

int cMgSmartball_ball_c::HitSwitches(int *arg) {
    int buf[2];
    int i = 0;
    int *zero = 0;
    while (i < ((dScMgSmartball_c *)mpManager)->mPushSwitchCount) {
        int *p;
        dScMgSmartball_c *base;
        buf[0] = arg[0];
        buf[1] = arg[1];
        base = (dScMgSmartball_c *)mpManager;
        p = (i >= base->mPushSwitchCount) ? zero : (int *)base->mpPushSwitches[i];
        if (((cMgSmartball_pushswitch_c *)((char *)p))->CheckBall((int *)buf))
            return 1;
        i++;
    }
    return 0;
}

// @symbol _ZN19cMgSmartball_ball_c10HitPakkunsEPi
extern "C" int cMgSmartball_ball_c::HitPakkuns(int *arg) {
    int buf[2];
    int i = 0;
    int *zero = 0;
    while (i < ((dScMgSmartball_c *)mpManager)->mPakkunCount) {
        int *p;
        dScMgSmartball_c *base;
        buf[0] = arg[0];
        buf[1] = arg[1];
        base = (dScMgSmartball_c *)mpManager;
        p = (i >= base->mPakkunCount) ? zero : (int *)base->mpPakkuns[i];
        if (((cMgSmartball_pakkun_c *)p)->CheckBall((int *)(buf)))
            return 1;
        i++;
    }
    return 0;
}

// @symbol _ZN19cMgSmartball_ball_c9HitDokansEPi

int cMgSmartball_ball_c::HitDokans(int *arg) {
    int buf[2];
    int i = 0;
    int *zero = 0;
    while (i < ((dScMgSmartball_c *)mpManager)->mDokanCount) {
        int *p;
        dScMgSmartball_c *base;
        buf[0] = arg[0];
        buf[1] = arg[1];
        base = (dScMgSmartball_c *)mpManager;
        p = (i >= base->mDokanCount) ? zero : (int *)base->mpDokans[i];
        if (((cMgSmartball_dokan_c *)(p))->CheckBall(buf))
            return 1;
        i++;
    }
    return 0;
}

// @symbol _ZN19cMgSmartball_ball_c13HitPropellersEPi
int cMgSmartball_ball_c::HitPropellers(int *pos) {
    struct S *r7 = (struct S *)pos;
    struct S loc;
    int i;
    if (r7->b >= 0x90000)
        return 0;
    i = 0;
    if (((dScMgSmartball_c *)mpManager)->mPropellerCount > 0) {
        do {
            dScMgSmartball_c *base;
            int x;
            loc.a = r7->a;
            loc.b = r7->b;
            base = (dScMgSmartball_c *)mpManager;
            if (i >= base->mPropellerCount)
                x = 0;
            else
                x = (int)base->mpPropellers[i];
            if (((cMgSmartball_propeller_c *)((char *)x))->HitTest((int *)(&loc)) != 0)
                return 1;
            i++;
        } while (i < ((dScMgSmartball_c *)mpManager)->mPropellerCount);
    }
    return 0;
}

// @symbol _ZN19cMgSmartball_ball_c10HitKinokosEPi
#pragma push
#pragma opt_strength_reduction off
int cMgSmartball_ball_c::HitKinokos(int *pos) {
    int i;
    dScMgSmartball_c *base;
    int n;
    int r7;
    int delta[2];
    int out[2];
    if (pos[1] >= 0x90000)
        return 0;
    base = (dScMgSmartball_c *)mpManager;
    n = base->mKinokoCount;
    i = 0;
    if (n > 0) {
        do {
            if (nearby[i]) {
                r7 = (i >= n ? 0 : base->mpKinokos[i])->HitRadiusSq();
                if (r7 > 0) {
                    delta[0] = pos[0];
                    delta[1] = pos[1];
                    CopyObjectPos(
                        (int *)out,
                        (int *)(i >= ((dScMgSmartball_c *)mpManager)->mKinokoCount
                                    ? 0
                                    : (void *)((dScMgSmartball_c *)mpManager)->mpKinokos[i]));
                    delta[0] = delta[0] - out[0];
                    delta[1] = delta[1] - out[1];
                    if (func_0203d5bc(delta) < (long long)r7) {
                        void *e = i >= ((dScMgSmartball_c *)mpManager)->mKinokoCount
                                      ? 0
                                      : (void *)((dScMgSmartball_c *)mpManager)->mpKinokos[i];
                        ((cMgSmartball_kinoko_c *)e)->mWasHit = 1;
                        return 1;
                    }
                }
            }
            i++;
            base = (dScMgSmartball_c *)mpManager;
            n = base->mKinokoCount;
        } while (i < n);
    }
    return 0;
}

#pragma pop

// @symbol _ZN19cMgSmartball_ball_c10CheckExitsEPi
int cMgSmartball_ball_c::CheckExits(int *pos) {
    struct S *arg = (struct S *)pos;
    struct S p, q0, q1, q2;
    struct S d, e, f, g;
    if (arg->b >= 0)
        return 0;
    if (arg->a >= 0x8000 && arg->a < 0x28000) {
        if (arg->b >= -0x90000 && arg->b < -0x88000) {
            ((dScMgSmartball_c *)mpManager)->KickGate(0);
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
        if (PointInTriangle((int *)&p, (int *)&q0, (int *)&q1, (int *)&q2)) {
            ((dScMgSmartball_c *)mpManager)->KickGate(0);
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
        if (PointInTriangle((int *)&d, (int *)&e, (int *)&f, (int *)&g)) {
            ((dScMgSmartball_c *)mpManager)->KickGate(1);
            return 1;
        }
    }
    return 0;
}

// @symbol _ZN19cMgSmartball_ball_c8AtSpringEPi

int cMgSmartball_ball_c::AtSpring(int *b) {
    int local[2];
    dScMgSmartball_c *base = (dScMgSmartball_c *)mpManager;
    if (mIndex != base->mServeIndex)
        return 0;
    if (b[0] >= 0xdc000) {
        CopyObjectPos(local, (int *)base->mpSpring);
        if (b[1] >= local[1] - 0x20000)
            return 1;
    }
    return 0;
}

// @symbol _ZN19cMgSmartball_ball_c7OnSlopeEPi
int cMgSmartball_ball_c::OnSlope(int *p) {
    int local[2];
    unsigned short v;
    int x, z, diff;

    local[0] = p[0];
    local[1] = p[1];
    v = ((dScMgSmartball_c *)mpManager)->GetTile(local);

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

// @symbol _ZN19cMgSmartball_ball_c12NeedsRespawnEPi
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
    if (PointInTriangle((int *)&p, (int *)&q0, (int *)&q1, (int *)&q2))
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

// @symbol _ZN19cMgSmartball_ball_c11QueueBehindEv
void cMgSmartball_ball_c::QueueBehind() {
    int cur;
    int val;
    dScMgSmartball_c *mgr;
    int vm1;
    int *p;
    int local1[2];
    int local2[2];
    int newpos;
    int pos;

    val = mIndex;
    mgr = (dScMgSmartball_c *)mpManager;
    cur = mgr->mServeIndex;

    if (val == cur) {
        if (mInPlay != 0) {
            SmartballBall_UpdatePhysics(this);
            return;
        }
    }

    vm1 = val - 1;
    if (vm1 == cur) {
        p = (cur >= 0xd) ? (int *)0 : (int *)mgr->mpBalls[cur];
        if (((cMgSmartball_ball_c *)p)->mInPlay != 0)
            goto block1;
    }

    if (val != cur || mInPlay != 0)
        goto block2;

block1:
    p = (cur >= 0xd) ? (int *)0 : (int *)mgr->mpBalls[cur];
    CopyObjectPos(local1, p);
    newpos = local1[0] - 0x10000;
    pos = mCurrent0;
    if (pos < newpos && pos < 0xc8000)
        mCurrent0 = newpos;
    else
        mCurrent0 = mSnapshot0;
    mVel0 = 0;
    mVel1 = 0;

    mCurrent1 = cstd::fdiv(FX_MUL(mCurrent0, 0xa000), 0xd8000) + 0x90000;

    if (mIndex > ((dScMgSmartball_c *)mpManager)->mServeIndex) {
        mQueueGap = (mIndex - ((dScMgSmartball_c *)mpManager)->mServeIndex) * 0xf;
    }
    return;

block2:
    p = (vm1 >= 0xd) ? (int *)0 : (int *)mgr->mpBalls[vm1];
    CopyObjectPos(local2, p);
    mCurrent0 = local2[0] - 0x10000;
    mVel0 = 0;
    mVel1 = 0;

    mCurrent1 = cstd::fdiv(FX_MUL(mCurrent0, 0xa000), 0xd8000) + 0x90000;

    if (mIndex > ((dScMgSmartball_c *)mpManager)->mServeIndex) {
        mQueueGap = (mIndex - ((dScMgSmartball_c *)mpManager)->mServeIndex) * 0xf;
    }
}

// @symbol SmartballBall_UpdatePhysics
#pragma push
#pragma opt_strength_reduction off
/* The equal-arm owner selection below preserves mwccarm's address rematerialization. */
extern "C" { /* .c-derived member: C linkage for the whole block */

void SmartballBall_UpdatePhysics(cMgSmartball_ball_c *self) {
    State2 state;
    V2xz work;
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

    savedVel.x = self->mVel0;
    state.flags[0] = 0;
    savedVel.z = self->mVel1;
    self->targetIndex = -1;
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
        self->hitA[i] = (u8)state.iteration;
        self->hitC[i] = (u8)state.iteration;
        self->hitB[i] = (u8)state.iteration;
        i++;
    } while (i < 0x20);
    self->anyHit = (u8)state.iteration;
    self->specialHit = (u8)state.iteration;
    state.iteration = 0;

    if (self->mCurrent1 >= 0x77000) {
        int clearIndex = state.iteration;
        do {
            self->nearby[clearIndex] = 0;
            clearIndex++;
        } while (clearIndex < 0x19);
    } else {
        int probeIndex = state.iteration;
        do {
            self->nearby[probeIndex] = 0;
            if (probeIndex < ((dScMgSmartball_c *)self->mpManager)->mKinokoCount) {
                dist.x = self->mCurrent0;
                dist.z = self->mCurrent1;
                mgr = ((dScMgSmartball_c *)self->mpManager);
                CopyObjectPos((int *)&pos0, (int *)(probeIndex >= mgr->mKinokoCount
                                                        ? (void *)0
                                                        : mgr->mpKinokos[probeIndex]));
                dist.x -= pos0.x;
                dist.z -= pos0.z;
                if (func_0203d5bc((int *)&dist) <= 0x900000LL) {
                    self->nearby[probeIndex] = 1;
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
        self->hit[i] = (u8)state.zero;
        work.x = state.zero;
        work.z = state.zero;
        work.z += self->mRadius;
        func_0203d388((V2 *)&work, angle);
        dot = func_0203d570((V2 *)&work, (V2 *)(V2xz *)&self->mVel0);
        if (dot >= 0) {
            work.x += self->mCurrent0;
            work.z += self->mCurrent1;
            if ((u8)((dScMgSmartball_c *)self->mpManager)->param1 == 0) {
                pos1 = work;
                if (((cMgSmartball_ball_c *)self)->NeedsRespawn((int *)&pos1) != 0) {
                    self->hit[i] = 1;
                    self->hitA[i] = 1;
                    self->anyHit = 1;
                    blocked = 1;
                    goto final_checks;
                } else {
                    pos2 = work;
                    if (((cMgSmartball_ball_c *)self)->OnSlope((int *)&pos2) != 0) {
                        self->hit[i] = 1;
                        self->hitA[i] = 1;
                        self->anyHit = 1;
                        state.flags[0] = 1;
                        blocked = 1;
                    } else {
                        pos3 = work;
                        if (((cMgSmartball_ball_c *)self)->CheckExits((int *)&pos3) != 0) {
                            self->hit[i] = 1;
                            self->hitA[i] = 1;
                            self->anyHit = 1;
                            self->hitB[i] = 1;
                            state.flags[2] = 1;
                            blocked = 1;
                        }
                    }
                }
            } else {
                pos4 = work;
                if (((cMgSmartball_ball_c *)self)->NeedsRespawn((int *)&pos4) != 0) {
                    self->hit[i] = 1;
                    self->hitA[i] = 1;
                    self->anyHit = 1;
                    blocked = 1;
                } else {
                    pos5 = work;
                    if (((cMgSmartball_ball_c *)self)->CheckExits((int *)&pos5) != 0) {
                        self->hit[i] = 1;
                        self->hitA[i] = 1;
                        self->anyHit = 1;
                        self->hitB[i] = 1;
                        state.flags[2] = 1;
                        blocked = 1;
                        goto final_checks;
                    } else {
                        if (state.iteration == 1) {
                            pos6 = work;
                            if (((cMgSmartball_ball_c *)self)->HitKinokos((int *)&pos6) != 0) {
                                self->hit[i] = 1;
                                self->hitA[i] = 1;
                                self->anyHit = 1;
                                self->specialHit = 1;
                                state.flags[3] = 1;
                                blocked = 1;
                                goto final_checks;
                            }
                        }
                        pos7 = work;
                        if (((cMgSmartball_ball_c *)self)->HitDokans((int *)&pos7) != 0) {
                            self->hit[i] = 1;
                            self->hitA[i] = 1;
                            self->anyHit = 1;
                            blocked = 1;
                        } else {
                            pos8 = work;
                            if (((cMgSmartball_ball_c *)self)->HitPakkuns((int *)&pos8) != 0) {
                                self->hit[i] = 1;
                                self->hitA[i] = 1;
                                self->anyHit = 1;
                                blocked = 1;
                            } else {
                                pos9 = work;
                                if (((cMgSmartball_ball_c *)self)->HitSwitches((int *)&pos9) != 0) {
                                    self->hit[i] = 1;
                                    self->hitA[i] = 1;
                                    self->anyHit = 1;
                                    blocked = 1;
                                } else {
                                    pos10 = work;
                                    if (((cMgSmartball_ball_c *)self)->HitWing((int *)&pos10) !=
                                        0) {
                                        self->hit[i] = 1;
                                        self->hitA[i] = 1;
                                        self->anyHit = 1;
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
            work.x += self->mCurrent0;
            work.z += self->mCurrent1;
        }

    final_checks:
        if (blocked == 0) {
            pos11 = work;
            if (((cMgSmartball_ball_c *)self)->AtSpring((int *)&pos11) != 0) {
                self->hit[i] = 1;
                self->hitA[i] = 1;
                self->anyHit = 1;
                blocked = 1;
                if (angle == 0)
                    state.flags[5] = 1;
            } else {
                pos12 = work;
                if (((cMgSmartball_ball_c *)self)->HitPropellers((int *)&pos12) != 0) {
                    self->hit[i] = 1;
                    self->hitA[i] = 1;
                    self->anyHit = 1;
                    blocked = 1;
                    state.flags[1] = 1;
                }
            }
        }
        angle = (s16)(angle + 0x800);
        i++;
    } while (i < 0x20);

    if (state.flags[6] < 0x11 && self->state3a == 0) {
        entityLoopIndex = 0;
        if (((dScMgSmartball_c *)self->mpManager)->mBallCount > 0) {
            do {
                if (entityLoopIndex != self->mIndex) {
                    other = entityLoopIndex >= 0xd
                                ? state.null34
                                : ((dScMgSmartball_c *)self->mpManager)->mpBalls[entityLoopIndex];
                    if (((cMgSmartball_ball_c *)other)->state3a == 0) {
                        other =
                            entityLoopIndex >= 0xd
                                ? state.null38
                                : ((dScMgSmartball_c *)self->mpManager)->mpBalls[entityLoopIndex];
                        if (((cMgSmartball_object_c *)other)->mIsActive != 0) {
                            other = entityLoopIndex >= 0xd ? (void *)state.null3c
                                                           : ((dScMgSmartball_c *)self->mpManager)
                                                                 ->mpBalls[entityLoopIndex];
                            CopyObjectPos((int *)&entityPos, (int *)other);
                            Vec2_Sub((int *)&entityDelta, (int *)(V2xz *)&self->mCurrent0,
                                     (int *)&entityPos);
                            delta = entityDelta;
                            if (func_0203d5bc((int *)&delta) <= 0x100000LL) {
                                angle = state.angle40;
                                mgr = ((dScMgSmartball_c *)self->mpManager);
                                other = entityLoopIndex >= 0xd ? (void *)angle
                                                               : mgr->mpBalls[entityLoopIndex];
                                CopyObjectPos((int *)&entityPos2, (int *)other);
                                otherPos = entityPos2;
                                if (self->mCurrent1 < 0x90000 || self->mCurrent0 <= otherPos.x) {
                                    i = state.index44;
                                    do {
                                        work.z = self->mRadius;
                                        work.x = 0;
                                        func_0203d388((V2 *)&work, angle);
                                        work.x += self->mCurrent0;
                                        work.z += self->mCurrent1;
                                        Vec2_Sub((int *)&radialDelta, (int *)&work,
                                                 (int *)&otherPos);
                                        delta = radialDelta;
                                        if (func_0203d5bc((int *)&delta) < 0x40000LL) {
                                            self->targetIndex = entityLoopIndex;
                                            self->hit[i] = 1;
                                            self->hitA[i] = 1;
                                            self->hitC[i] = 1;
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
            } while (entityLoopIndex < ((dScMgSmartball_c *)self->mpManager)->mBallCount);
        }
    }

    normal.x = state.zero48;
    normal.z = state.zero48;
    angle = state.zero48;
    i = state.zero48;
    do {
        if (self->hit[i] == 1) {
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
        V2xz *position = (V2xz *)&self->mCurrent0;
        cMgSmartball_ball_c *owner = position ? self : self;
        int *pz = &owner->mCurrent1;
        *(int *)((int)position) -= normal.x;
        *pz -= normal.z;
        Vec2_Sub((int *)&moveDelta, (int *)position, (int *)(V2xz *)&self->mSnapshot0);
    }
    dot = Vec2_Len((Vec2 *)&moveDelta);
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
            if (self->hitB[i] == 1) {
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
            if (self->hitA[i] == 1 && self->hitC[i] == 0) {
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
        func_0203d480((V2 *)&normalized, (V2 *)&response);
        dot = FIX_MUL(self->mVel0, response.x) + FIX_MUL(self->mVel1, response.z);
        if (state.flags[5] == 1) {
            responseScale = FIX_MUL(dot, 0x1800);
        } else if (state.flags[0] == 1) {
            u32 randomScale;
            u32 randomAngle;
            responseScale = FIX_MUL(dot, 0x1400);
            randomScale =
                ((((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0x10) >> 15) << 12);
            responseScale += FIX_MUL_SU(responseScale, randomScale >> 6);
            randomAngle = (u32)RandomIntInternal(&data_0209d4b8);
            func_0203d388((V2 *)&response,
                          (s16)(((((randomAngle >> 16) & 0x7fff) << 12) >> 15) - 0x800));
        } else if (state.flags[3] == 1) {
            u32 randomScale;
            u32 randomAngle;
            self->mVel0 = 0;
            self->mVel1 = 0;
            responseScale = -0x2000;
            randomScale =
                ((((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0x10) >> 15) << 12);
            responseScale += FIX_MUL_US(randomScale >> 5, -0x2000);
            randomAngle = (u32)RandomIntInternal(&data_0209d4b8);
            func_0203d388((V2 *)&response,
                          (s16)(((((randomAngle >> 16) & 0x7fff) << 12) >> 15) - 0x800));
        } else if (state.flags[2] == 1) {
            u32 randomScale;
            responseScale = FIX_MUL(dot, 0x1a00);
            randomScale =
                ((((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0x10) >> 15) << 12);
            responseScale += FIX_MUL_SU(responseScale, randomScale >> 6);
            if (self->mCurrent0 < 0x18000 && response.z > 0) {
                response.x = 0;
                response.z = 0x1000;
            }
        } else {
            responseScale = FIX_MUL(dot, 0x1200);
        }
        velXPtr = (int *)(int)(&self->mVel0);
        *velXPtr -= FIX_MUL(responseScale, response.x);
        velZPtr = (int *)(int)(&self->mVel1);
        *velZPtr -= FIX_MUL(responseScale, response.z);
    }

    if (state.flags[5] == 1) {
        if (self->mCurrent0 < 0xd0000)
            return;
        if (self->mCurrent0 < 0xd8000) {
            self->mCurrent0 = 0xd8000;
            return;
        }
        if (self->mCurrent0 < 0xe8000)
            self->mCurrent0 = 0xe8000;
        CopyObjectPos((int *)&specialPos, (int *)((dScMgSmartball_c *)self->mpManager)->mpSpring);
        self->mCurrent1 = specialPos.z - 0x20001 - self->mRadius;
        GetObjectDelta((void *)&specialDelta0,
                       (char *)((dScMgSmartball_c *)self->mpManager)->mpSpring);
        if (specialDelta0.z >= -0x1000)
            return;
        GetObjectDelta((void *)&specialDelta1,
                       (char *)((dScMgSmartball_c *)self->mpManager)->mpSpring);
        if (self->mVel1 < specialDelta1.z)
            return;
        self->mVel0 = 0;
        GetObjectDelta((void *)&specialDelta2,
                       (char *)((dScMgSmartball_c *)self->mpManager)->mpSpring);
        self->mVel1 = specialDelta2.z;
        if (self->mVel1 < -0x6000)
            self->mVel1 = (self->mVel1 + 0x6000) / 4 - 0x6000;

        if (self->soundTimer == 0) {
            if (self->mVel1 < -0x6000) {
                func_02012718(0x16d, self->mCurrent0);
                func_02012718(0x16e, self->mCurrent0);
            } else {
                volume = (-self->mVel1 << 7) / 0x6000;
                if (volume > 0x7f)
                    volume = 0x7f;
                if (volume < 0)
                    volume = 0;
                func_020126ac(0x16d, 5, volume, 0, func_020126e8(self->mCurrent0));
                if (volume >= 0x40)
                    func_020126ac(0x16e, 5, volume, 0, func_020126e8(self->mCurrent0));
            }
            self->soundTimer = 5;
        }
        if (self->soundPlayed != 0)
            return;
        if (self->mVel1 < -0x6000)
            func_02012718(0x16e, self->mCurrent0);
        self->soundPlayed = 1;
        return;
    }

    if (state.flags[0] == 1) {
        event0.x = savedVel.x;
        event0.z = savedVel.z;
        ((cMgSmartball_ball_c *)self)->PlayHitSound(0x171, (int *)&event0);
    } else if (state.flags[1] == 1) {
        event1.x = savedVel.x;
        event1.z = savedVel.z;
        ((cMgSmartball_ball_c *)self)->PlayHitSound(0x172, (int *)&event1);
    } else if (state.flags[2] == 1) {
        event2.x = savedVel.x;
        event2.z = savedVel.z;
        ((cMgSmartball_ball_c *)self)->PlayHitSound(0x16f, (int *)&event2);
    } else if (state.flags[4] == 1) {
        event3.x = savedVel.x;
        event3.z = savedVel.z;
        ((cMgSmartball_ball_c *)self)->PlayHitSound(0x1a3, (int *)&event3);
    }

    dot = self->mRadius + 0x10000;
    square = FIX_MUL(dot, dot);
    i = 0;
    if (((dScMgSmartball_c *)self->mpManager)->mAnaCount <= 0)
        return;
    do {
        other = i >= ((dScMgSmartball_c *)self->mpManager)->mAnaCount
                    ? (void *)0
                    : ((dScMgSmartball_c *)self->mpManager)->mpAnas[i];
        if (((cMgSmartball_ana_c *)other)->HasBall() == 0) {
            hazardDelta.x = self->mCurrent0;
            hazardDelta.z = self->mCurrent1;
            other = i >= ((dScMgSmartball_c *)self->mpManager)->mAnaCount
                        ? (void *)0
                        : ((dScMgSmartball_c *)self->mpManager)->mpAnas[i];
            CopyObjectPos((int *)&hazardPos, (int *)other);
            hazardDelta.x -= hazardPos.x;
            hazardDelta.z -= hazardPos.z;
            hazardDelta.z *= 2;
            if (func_0203d5bc((int *)&hazardDelta) < square) {
                dot = self->mRadius + 0x10000 - Vec2_Len((Vec2 *)&hazardDelta);
                if (self->mRadius < dot) {
                    other = i >= ((dScMgSmartball_c *)self->mpManager)->mAnaCount
                                ? (void *)0
                                : ((dScMgSmartball_c *)self->mpManager)->mpAnas[i];
                    CopyObjectPos((int *)&hazardPos2, (int *)other);
                    self->mSwallowX = hazardPos2.x;
                    self->mSwallowZ = hazardPos2.z;
                    other = i >= ((dScMgSmartball_c *)self->mpManager)->mAnaCount
                                ? (void *)0
                                : ((dScMgSmartball_c *)self->mpManager)->mpAnas[i];
                    ((cMgSmartball_ana_c *)other)->Swallow();
                    ((cMgSmartball_ball_c *)self)->ArmExpire(0x80);
                    self->state3a = 1;
                    return;
                }
                correction = hazardDelta;
                func_0203d434((V2 *)&correction);
                correction.x = FIX_MUL(correction.x, dot);
                correction.x /= 64;
                correction.z = FIX_MUL(correction.z, dot);
                correction.z /= 64;
                correctionX = correction.x;
                correctionZ = correction.z;
                velXPtr = (int *)(int)(&self->mVel0);
                *velXPtr -= correctionX;
                velZPtr = (int *)(int)(&self->mVel1);
                *velZPtr -= correction.z;
                return;
            }
        }
        i++;
    } while (i < ((dScMgSmartball_c *)self->mpManager)->mAnaCount);
}
}

#pragma pop

// @symbol _ZN19cMgSmartball_ball_c10DrawMarkerEv
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

    if ((((dScMgSmartball_c *)mpManager)->param1 & 0xff) != 0) {
        int x = mCurrent0;
        if (x >= 0x40000 && x < 0xa0000) {
            int y = mCurrent1;
            if (y >= -0xb5000 && y < -0x85000)
                return;
        }
        v = mRadius + 0x15000;
        sq = FX_MUL(v, v);
        for (i = 0; i < ((dScMgSmartball_c *)mpManager)->mAnaCount; i++) {
            if (((cMgSmartball_ana_c *)((
                     unsigned char *)((i >= ((dScMgSmartball_c *)mpManager)->mAnaCount)
                                          ? 0
                                          : ((dScMgSmartball_c *)mpManager)->mpAnas[i])))
                    ->HasBall() == 0) {
                s[0] = mCurrent0;
                s[1] = mCurrent1;
                s[1] += 0x5000;
                CopyObjectPos(&s[4], (i >= ((dScMgSmartball_c *)mpManager)->mAnaCount)
                                         ? 0
                                         : (int *)((dScMgSmartball_c *)mpManager)->mpAnas[i]);
                s[0] = s[0] - s[4];
                s[1] = s[1] - s[5];
                s[1] = s[1] * 2;
                if (func_0203d5bc(&s[0]) < sq)
                    return;
            }
        }
    }

    if ((((dScMgSmartball_c *)mpManager)->param1 & 0xff) == 0)
        RenderOamBothScreens(sSmartballBallSprites[1], (mCurrent0 + 0x800) >> 12,
                             ((mCurrent1 + 0x800) >> 12) + 5, -1, 2, 0);
    else
        RenderOamBothScreens(sSmartballBallSpritesAlt[1], (mCurrent0 + 0x800) >> 12,
                             ((mCurrent1 + 0x800) >> 12) + 5, -1, 2, 0);
}

// @symbol _ZN19cMgSmartball_ball_c6UpdateEv
/* Slot 1. Picks one of two sprites (based on which edge of the level the
 * scene reports) and draws it at the ball's rounded position, with a
 * priority bumped from 1 to 3 while state3a is set. */
void cMgSmartball_ball_c::Update() {
    int r2;
    if (mIsActive == 0)
        return;
    if (mInPlay == 0)
        return;
    r2 = 1;
    if (state3a == 1)
        r2 = 3;
    if ((((dScMgSmartball_c *)mpManager)->param1 & 0xff) == 0) {
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

// @symbol _ZN19cMgSmartball_ball_c12SaveSnapshotEv
/* Slot 0. Does NOT open with the base's SaveSnapshot body inline (unlike
 * every sibling so far) -- this is the one override in the family that
 * replaces the base behaviour instead of repeating it, gating the ball's
 * own countdown/zone/bounce/physics state machine every frame. */
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
    if (mIndex == ((dScMgSmartball_c *)mpManager)->mServeIndex) {
        if (mInPlay != 0)
            mIsWaiting = 0;
    }
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mCurrent0 < 0 || mCurrent0 >= 0x100000 || mCurrent1 < -0xf0000 || mCurrent1 >= 0xc0000) {
        mStuckFrames += 1;
    } else {
        if ((((dScMgSmartball_c *)mpManager)->param1 & 0xff) != 0) {
            Vec2_Sub((int *)&d1, (int *)(V2xz *)&mCurrent0, (int *)(V2xz *)&mLastPos0);
            t = Vec2_Len((Vec2 *)&d1) < 0x8000;
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
        if (PointInTriangle((int *)&pa, (int *)&pb, (int *)&pc, (int *)&pd) != 0)
            goto bounce;
        pe.x = mCurrent0;
        pe.z = mCurrent1;
        pf.x = 0x8000;
        pf.z = 0x92000;
        pg.x = 0xe0000;
        pg.z = 0x96000;
        ph.x = 0xe0000;
        ph.z = 0x9e000;
        if (PointInTriangle((int *)&pe, (int *)&pf, (int *)&pg, (int *)&ph) != 0) {
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
        spd = Vec2_Len((Vec2 *)(V2xz *)&mVel0);
        if (spd >= 0x8000)
            spd = 0x8000;
        if (func_0203d434((V2 *)(V2xz *)&mVel0) != 0)
            func_0203d630((V2 *)(V2xz *)&mVel0, spd);
    }
    mCurrent0 += mVel0;
    mCurrent1 += mVel1;
    if (mIsWaiting == 1) {
        QueueBehind();
    } else {
        SmartballBall_UpdatePhysics(this);
        if (mExitGateOpen == 0) {
            vc.x = 0x20000;
            vc.z = -0x8000;
            func_0203d480((V2 *)&v5c, (V2 *)&vc);
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
    mag = Vec2_Len((Vec2 *)(V2xz *)&mVel0);
    Vec2_Sub((int *)&d2, (int *)(V2xz *)&mCurrent0, (int *)(V2xz *)&mSnapshot0);
    dist = Vec2_Len((Vec2 *)&d2);
    if (specialHit != 0)
        return;
    if (mag < dist)
        return;
    if (mag < 0x800)
        return;
    if (func_0203d434((V2 *)(V2xz *)&mVel0) == 0)
        return;
    func_0203d630((V2 *)(V2xz *)&mVel0, dist);
}

// @symbol _ZN19cMgSmartball_ball_c14RestoreInitialEv
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), latches
 * mCurrent into the mLastPos0/mLastPos1 position pair, zeroes the base's
 * velocity pair, then exhaustively zeroes every field this class adds --
 * this function is the strongest evidence for the field list in the header,
 * since its zero pass is the one place all of them are touched at once. */
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
    if ((((dScMgSmartball_c *)mpManager)->param1 & 0xff) && mIndex >= 9)
        mInPlay = 0;
    else
        mInPlay = 1;
    soundTimer = 0;
    soundPlayed = 0;
    mExitGateOpen = 0;
}

// @symbol cMgSmartball_ball_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
int cMgSmartball_ball_c_Construct(char *c, int b, int d, int *s) {
    int tmp[2];
    tmp[0] = s[0];
    tmp[1] = s[1];
    cMgSmartball_object_c_Construct(c, b, d, tmp);
    *(int *)c = (int)&_ZTV19cMgSmartball_ball_c[2];
    ((cMgSmartball_object_c *)c)->mRadius = 0x8000;
    return (int)c;
}
}

// @symbol _ZN21cMgSmartball_object_c15PointInTriangleEPiS0_S0_S0_
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

// @symbol _ZN21cMgSmartball_kinoko_c11HitRadiusSqEv
int cMgSmartball_kinoko_c::HitRadiusSq() {
    return FX_MUL(mRadius, mRadius);
}

// @symbol _ZN21cMgSmartball_object_c6SetVelEPi
void cMgSmartball_object_c::SetVel(int *velocity) {
    mVel0 = velocity[0];
    mVel1 = velocity[1];
}

// @symbol CopyObjectVel
extern "C" { /* .c-derived member: C linkage for the whole block */
void CopyObjectVel(int *a, int *b) {
    a[0] = ((cMgSmartball_object_c *)b)->mVel0;
    a[1] = ((cMgSmartball_object_c *)b)->mVel1;
}
}

// @symbol GetObjectDelta
extern "C" { /* .c-derived member: C linkage for the whole block */
void GetObjectDelta(void *a, char *b) {
    Vec2_Sub((int *)a, &((cMgSmartball_object_c *)b)->mCurrent0,
             &((cMgSmartball_object_c *)b)->mSnapshot0);
}
}

// @symbol CopyObjectPos
extern "C" { /* .c-derived member: C linkage for the whole block */
void CopyObjectPos(int *a, int *b) {
    a[0] = ((cMgSmartball_object_c *)b)->mCurrent0;
    a[1] = ((cMgSmartball_object_c *)b)->mCurrent1;
}
}

// @symbol _ZN21cMgSmartball_object_c6UpdateEv
/* vtable slot 1. Empty in this class -- the
   0x4-byte body is a bare `bx lr`. Every child overrides it. */
void cMgSmartball_object_c::Update() {}

// @symbol _ZN21cMgSmartball_object_c12SaveSnapshotEv
/* vtable slot 0. Name coined from the body --
   see include/cMgSmartball_object_c.h's own note on why this hierarchy has
   no names to recover. */
void cMgSmartball_object_c::SaveSnapshot() {
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
}

// @symbol _ZN21cMgSmartball_object_c14RestoreInitialEv
/* Name coined from the body --
   see include/cMgSmartball_object_c.h's own note.

   Both reads come before both writes, deliberately. The plain
   `mCurrent0 = mInitial0; mCurrent1 = mInitial1;` spelling costs 4 words:
   the ROM loads the pair and then stores the pair, which is what the two
   temporaries reproduce. SaveSnapshot above needs no such care -- its
   source and destination do not overlap in the same way, and the naive
   spelling emits the same there. */
void cMgSmartball_object_c::RestoreInitial() {
    s32 hi = mInitial1;
    s32 lo = mInitial0;
    mCurrent0 = lo;
    mCurrent1 = hi;
}

// @symbol cMgSmartball_object_c_Construct
extern "C" { /* .c-derived member: C linkage for the whole block */
void cMgSmartball_object_c_Construct(char *c, int a1, int a2, int *a3) {
    int lo, hi;
    *(int *)(c) = (int)&_ZTV21cMgSmartball_object_c[2];
    ((cMgSmartball_object_c *)c)->mpManager = a1;
    ((cMgSmartball_object_c *)c)->mIndex = a2;
    hi = a3[1];
    lo = a3[0];
    ((cMgSmartball_object_c *)c)->mInitial0 = lo;
    ((cMgSmartball_object_c *)c)->mInitial1 = hi;
    hi = a3[1];
    lo = a3[0];
    ((cMgSmartball_object_c *)c)->mCurrent0 = lo;
    ((cMgSmartball_object_c *)c)->mCurrent1 = hi;
    ((cMgSmartball_object_c *)c)->mRadius = 0;
    ((cMgSmartball_object_c *)c)->mVel0 = 0;
    ((cMgSmartball_object_c *)c)->mVel1 = 0;
    ((cMgSmartball_object_c *)c)->mIsActive = 1;
}
}

// @symbol _ZN16dScMgSmartball_c16OnGraphCallback3Ev
/* dScMgSmartball_c::OnGraphCallback3 - slot 25 override */
int dScMgSmartball_c::OnGraphCallback3() {
    void *t = (void *)this;
    return ((dScMgBase_c *)t)->dScMgBase_c::OnGraphCallback3() != 0;
}

// @symbol _ZN16dScMgSmartball_c12SparksActiveEv
int dScMgSmartball_c::SparksActive() {
    char *p = (char *)this;
    int i;
    for (i = 0; i < 0x40; i++) {
        if (((dScMgSmartball_c *)p)->mSparks[0].mActive == 1)
            return 1;
        p += 0x24;
    }
    return 0;
}

// @symbol _ZN16dScMgSmartball_c10EmitSparksEPii
/* ov006 minigame: seeds up to 0x18 particle rows (0x24 bytes each, at c+0x599c)
 * with random headings, first over the free rows in order and then at random
 * rows until the count is reached. The random value is masked only after the
 * first store: the ROM stores the zero, then shifts. Masking before the store
 * lets the scheduler hoist the shift above the store and the second store
 * drifts past the next call's argument setup. */
void dScMgSmartball_c::EmitSparks(int *pair, int a3) {
    char *c = (char *)this;
    register int count;
    register int i;
    char *p;
    int *pang;
    unsigned int r;
    char *q;
    int *pf8;
    int *pfc;
    int *pf4;
    int *pf0;
    int ten;
    int one;
    int z_a;
    int z_b;
    int z2;
    int ten2;
    int one2;

    count = 0;
    p = c;
    pang = ((dScMgSmartball_c *)c)->mSparks[0].mPos;
    ten = 0x10;
    one = 1;
    z_a = count;
    z_b = count;
    for (i = 0; i < 0x40; p += 0x24, pang += 9, i++) {
        if (((dScMgSmartball_c *)p)->mSparks[0].mActive == 1)
            continue;
        ((dScMgSmartball_c *)p)->mSparks[0].mOrigin[0] = pair[0];
        ((dScMgSmartball_c *)p)->mSparks[0].mOrigin[1] = pair[1];
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        ((dScMgSmartball_c *)p)->mSparks[0].mPos[0] = z_a;
        r = (r >> 16) & 0x7fff;
        r = r << 16;
        ((dScMgSmartball_c *)p)->mSparks[0].mPos[1] = (int)(r >> 15);
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        func_0203d388((V2 *)pang, (short)(unsigned short)(r * 2));
        /* mSparks[0].mPos/mOrigin and mActive below: the member spelling moves bytes. */
        *(int *)(p + 0x59a8) <<= 1;
        *(int *)(p + 0x59a4) += *(int *)(p + 0x599c);
        *(int *)(p + 0x59a8) += *(int *)(p + 0x59a0);
        ((dScMgSmartball_c *)p)->mSparks[0].mVel[0] = z_b;
        ((dScMgSmartball_c *)p)->mSparks[0].mVel[1] = ten;
        *(unsigned char *)(p + 0x59bc) = (unsigned char)one;
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        ((dScMgSmartball_c *)p)->mSparks[0].mFrame = (int)((r << 4) >> 15);
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        ((dScMgSmartball_c *)p)->mSparks[0].mBursts = (int)((r * 3) >> 15) + 5;
        count++;
        ((dScMgSmartball_c *)p)->mSparks[0].mAlt = (unsigned char)a3;
        if (count >= 0x18)
            return;
    }
    if (count >= 0x18)
        return;

    z2 = 0;
    ten2 = 0x10;
    one2 = 1;
    do {
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        q = c + ((int)((r << 6) >> 15) * 0x24);
        pf0 = &((dScMgSmartball_c *)q)->mSparks[0].mOrigin[0];
        *pf0 = pair[0];
        pf4 = &((dScMgSmartball_c *)q)->mSparks[0].mOrigin[1];
        *pf4 = pair[1];
        r = RandomIntInternal(&data_0209d4b8);
        pf8 = &((dScMgSmartball_c *)q)->mSparks[0].mPos[0];
        *pf8 = z2;
        pfc = &((dScMgSmartball_c *)q)->mSparks[0].mPos[1];
        *pfc = (int)(((((unsigned int)r >> 16) & 0x7fff) * 0xc000) >> 15);
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        func_0203d388((V2 *)pf8, (short)(unsigned short)(r * 2));
        *pfc <<= 1;
        *pf8 += *pf0;
        *pfc += *pf4;
        ((dScMgSmartball_c *)q)->mSparks[0].mVel[0] = z2;
        ((dScMgSmartball_c *)q)->mSparks[0].mVel[1] = ten2;
        ((dScMgSmartball_c *)q)->mSparks[0].mActive = (unsigned char)one2;
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        ((dScMgSmartball_c *)q)->mSparks[0].mFrame = (int)((r << 4) >> 15);
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        ((dScMgSmartball_c *)q)->mSparks[0].mBursts = (int)((r * 3) >> 15) + 5;
        count++;
        ((dScMgSmartball_c *)q)->mSparks[0].mAlt = (unsigned char)a3;
    } while (count < 0x18);
}

// @symbol _ZN16dScMgSmartball_c12RenderSparksEv
void dScMgSmartball_c::RenderSparks() {
    char *self = (char *)this;
    dScMgSmartball_c *scene = (dScMgSmartball_c *)self;
    int i;
    char *p = self;
    for (i = 0; i < 0x40; i++) {
        if (((dScMgSmartball_c *)p)->mSparks[0].mActive != 0) {
            if (((int)scene->param1 & 0xff) == 0) {
                int idx = ((dScMgSmartball_c *)p)->mSparks[0].mFrame / 4 + 8;
                int b = ((dScMgSmartball_c *)p)->mSparks[0].mAlt;
                if (b != 0)
                    idx += 4;
                Mg_RenderSpriteBoth(sSmartballSparkSprites[idx],
                                    ((dScMgSmartball_c *)p)->mSparks[0].mPos[0] >> 12,
                                    ((dScMgSmartball_c *)p)->mSparks[0].mPos[1] >> 12, -1, 0);
            } else {
                int idx = ((dScMgSmartball_c *)p)->mSparks[0].mFrame / 4 + 4;
                int b = ((dScMgSmartball_c *)p)->mSparks[0].mAlt;
                if (b != 0)
                    idx += 4;
                Mg_RenderSpriteBoth(sSmartballSparkSpritesAlt[idx],
                                    ((dScMgSmartball_c *)p)->mSparks[0].mPos[0] >> 12,
                                    ((dScMgSmartball_c *)p)->mSparks[0].mPos[1] >> 12, -1, 0);
            }
        }
        p += 0x24;
    }
}

// @symbol _ZN16dScMgSmartball_c12UpdateSparksEv
void dScMgSmartball_c::UpdateSparks() {
    char *o = (char *)this;
    int i;
    char *b;
    char *v;
    char *p;
    Fix12i d;

    b = o;
    v = o + 0x59ac;
    p = o + 0x59a4;
    i = 0;
    do {
        if (((dScMgSmartball_c *)b)->mSparks[0].mActive) {
            L(b + 0x59a4) += ((dScMgSmartball_c *)b)->mSparks[0].mVel[0];
            L(b + 0x59a8) += ((dScMgSmartball_c *)b)->mSparks[0].mVel[1];
            d = Vec2_Len((Vec2 *)(const Vector3 *)v) * 7 / 8;
            if (func_0203d434((V2 *)(Fix12i *)v))
                func_0203d630((V2 *)(int *)v, (int)d);
            N(b + 0x59b4)++;
            if (((dScMgSmartball_c *)b)->mSparks[0].mFrame >= 0x10) {
                /* mFrame = 0, mActive = 0, mVel = {0, 0x10}: the member spelling moves bytes. */
                *(int *)(b + 0x59b4) = 0;
                N(b + 0x59b8)--;
                if (((dScMgSmartball_c *)b)->mSparks[0].mBursts <= 0) {
                    *(unsigned char *)(b + 0x59bc) = 0;
                } else {
                    d = RandomIntInternal(&data_0209d4b8);
                    ((dScMgSmartball_c *)b)->mSparks[0].mPos[0] = 0;
                    ((dScMgSmartball_c *)b)->mSparks[0].mPos[1] = (u16)((u32)d >> 16 & 0x7fff) * 2;
                    func_0203d388(
                        (V2 *)(int *)p,
                        (short)(u16)(((u32)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 2));
                    M(b + 0x59a8) *= 2;
                    M(b + 0x59a4) += ((dScMgSmartball_c *)b)->mSparks[0].mOrigin[0];
                    M(b + 0x59a8) += ((dScMgSmartball_c *)b)->mSparks[0].mOrigin[1];
                    *(int *)(b + 0x59ac) = 0;
                    *(int *)(b + 0x59b0) = 0x10;
                }
            }
        }
        b += 0x24;
        v += 0x24;
        p += 0x24;
        i++;
    } while (i < 0x40);
}

// @symbol _ZN16dScMgSmartball_c11ResetSparksEv
void dScMgSmartball_c::ResetSparks() {
    int i;
    for (i = 0; i < 0x40; i++) {
        mSparks[i].mActive = 0;
        mSparks[i].mBursts = 0;
        mSparks[i].mFrame = 0;
    }
}

// @symbol _ZN16dScMgSmartball_c8EndRoundEv
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

// @symbol _ZN16dScMgSmartball_c13ServeNextBallEv
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
        CopyObjectPos(t, (int *)get(this, i));
        func_02012718(0x1a5, t[0]);
        return;
    }
}

// @symbol _ZN16dScMgSmartball_c8CountOutEv
// The ROM materializes &mOutTally via a pool-loaded constant + add
// (ldr r1,[pc]; add r1,self,r1) rather than splitting the large offset.
void dScMgSmartball_c::CountOut() {
    mOutTally++;
}

// @symbol _ZN16dScMgSmartball_c10CountAwardEi
void dScMgSmartball_c::CountAward(int i) {
    mAwardTally[i]++;
}

// @symbol _ZN16dScMgSmartball_c10CountDokanEv
void dScMgSmartball_c::CountDokan() {
    mDokanTally++;
}

// @symbol _ZN16dScMgSmartball_c8CountAnaEv
void dScMgSmartball_c::CountAna() {
    mAnaTally++;
}

// @symbol _ZN16dScMgSmartball_c11CountKinokoEv
void dScMgSmartball_c::CountKinoko() {
    mKinokoTally++;
}

// @symbol _ZN16dScMgSmartball_c10CountBingoEv
void dScMgSmartball_c::CountBingo() {
    mBingoTally++;
}

// @symbol _ZN16dScMgSmartball_c9CountLineEi
int dScMgSmartball_c::CountLine(int i) {
    int *arr = mLineTally;
    int v = arr[i - 1] + 1;
    arr[i - 1] = v;
    return v;
}

// @symbol _ZN16dScMgSmartball_c9CountCellEv
void dScMgSmartball_c::CountCell() {
    mCellTally++;
}

// @symbol _ZN16dScMgSmartball_c7GetTileEPi
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

// @symbol _ZN16dScMgSmartball_c10UpdateDustEv
#pragma push
#pragma opt_strength_reduction off
void dScMgSmartball_c::UpdateDust() {
    char *self = (char *)this;
    int i;
    char *obj = (char *)mDustVel;
    for (i = 0; i < 0x10; i++) {
        unsigned char *flag = (unsigned char *)(int)((dScMgSmartball_c *)(self + i))->mDustActive;
        if (*flag != 0) {
            *(int *)((dScMgSmartball_c *)(int)(self + i * 8))->mDustPos +=
                *(int *)((dScMgSmartball_c *)(self + i * 8))->mDustVel;
            *(int *)&((dScMgSmartball_c *)(int)(self + i * 8))->mDustPos[4] +=
                *(int *)&((dScMgSmartball_c *)(self + i * 8))->mDustVel[4];
            {
                int m = Vec2_Len((Vec2 *)obj) * 6 / 8;
                if (func_0203d434((V2 *)obj)) {
                    func_0203d630((V2 *)obj, m);
                }
            }
            {
                int *counter = (int *)(int)((dScMgSmartball_c *)(self + i * 4))->mDustAge;
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

// @symbol _ZN16dScMgSmartball_c9SpawnDustEPi
/* SpawnDust -- dScMgSmartball_c, ov006 0x02115248, 0x238 bytes.
 *
 * Spawns up to four dust puffs: for each of the four passes it takes the first
 * free slot of sixteen, draws two angles from the shared RNG, and fills that
 * slot's mDustPos and mDustVel from the sine table, the position biased by the
 * caller's origin, then marks the slot taken.
 *
 * TWO THINGS ARE LOAD BEARING AND BOTH WERE MEASURED.
 *
 * 1. TVec2 HAS AN EMPTY DESTRUCTOR.  `v` is never address-taken, so a plain POD
 *    pair is scalarised into registers and the frame comes out sub sp,#0x1c; the
 *    non-POD copy forces a stack home and the cartridge's sub sp,#0x24.  That is
 *    the only difference between an otherwise byte-identical body and this one.
 *
 * 2. `sa` IS A SEPARATE s64 STATEMENT, NOT AN INITIALISER.  sinA feeds two
 *    64-bit products, times 0x8000 here and times 0x4000 twelve lines down.
 *    Widening it once into its own local, ASSIGNED as a statement ahead of the
 *    first product, makes 2004/b56 compute the sign word once into a short-lived
 *    scratch and derive both high halves from it back to back (asr ...,#0x1f then
 *    lsl #0xf and lsl #0xe adjacent, the scratch dying immediately into the
 *    #0x800 rounding constant).  Every other spelling defers the second shift
 *    about twenty words into the y block and misses the whole window:
 *    `s64 sa = sinA;` at the declaration, using sa for the 0x8000 product as
 *    well, hoisting either product into a named int, reordering the stores, the
 *    six FX operand and helper forms, and five optimisation pragmas were all
 *    measured and all leave the same 27-word window.
 *
 * The flag and the two arrays are spelled through the stepped base (c + j,
 * c + j * 8), not indexed from `this`: &mDustActive[j], mDustPos[j * 8] and
 * mDustVel[j * 8 + 4] each move bytes, and a struct-array view of either array
 * changes the frame and the loop's addressing and does not reproduce.
 */
extern "C" void dScMgSmartball_c::SpawnDust(int *origin) {
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
                int rnd1 = RandomIntInternal(&data_0209d4b8);
                int rnd2 = RandomIntInternal(&data_0209d4b8);
                int angB = ((((unsigned int)rnd2 >> 16) & 0x7fff) * 0x8000) >> 16;
                int angA = ((((unsigned int)rnd1 >> 16) & 0x7fff) * 0x20000) >> 16;
                s16 sinB = data_02082214[(angB >> 4) * 2 + 1];
                s16 sinA = data_02082214[(angA >> 4) * 2 + 1];
                s16 cosA;
                s64 sa;
                TVec2 v;
                sa = (s64)sinA;
                v.x = FX(sinB, FX(sinA, 0x8000));
                *(int *)((dScMgSmartball_c *)(c + j * 8))->mDustPos = v.x;
                cosA = data_02082214[(angA >> 4) * 2];
                v.y = FX(sinB, FX(cosA, 0x8000));
                *(int *)&((dScMgSmartball_c *)(c + j * 8))->mDustPos[4] = v.y;
                *(int *)((dScMgSmartball_c *)(c + j * 8))->mDustPos += origin[0];
                *(int *)&((dScMgSmartball_c *)(c + j * 8))->mDustPos[4] += origin[1];
                *(int *)((dScMgSmartball_c *)(c + j * 8))->mDustVel = FX(sinB, FX(sa, 0x4000));
                *(int *)&((dScMgSmartball_c *)(c + j * 8))->mDustVel[4] =
                    FX(sinB, FX(cosA, 0x4000));
                mDustAge[j] = 0;
                *flag = 1;
            }
            break;
        }
    }
}

// @symbol _ZN16dScMgSmartball_c12UpdateScoresEv
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void dScMgSmartball_c::UpdateScores() {
    char *o = (char *)this;
    dScMgSmartball_c *self = (dScMgSmartball_c *)o;
    int i;
    for (i = 0; i < 5; i++) {
        int *A = ((dScMgSmartball_c *)(int)(o + i * 4))->mPopupScore;
        int *E = ((dScMgSmartball_c *)(int)(o + i * 4))->mPopupDelay;
        int *D0 = (int *)((dScMgSmartball_c *)(int)(o + i * 8))->mPopupPos;
        int *D1 = (int *)&((dScMgSmartball_c *)(int)(o + i * 8))->mPopupPos[4];
        int *B = ((dScMgSmartball_c *)(int)(o + i * 4))->mPopupMultiplier;
        int *C = ((dScMgSmartball_c *)(int)(o + i * 4))->mPopupAge;
        if (*A > 0) {
            if (*E > 0) {
                *E -= 1;
                if (*E <= 0) {
                    int tmp[2];
                    tmp[0] = *D0;
                    tmp[1] = *D1;
                    EmitScore((int *)tmp, *A, 0, *B);
                    continue;
                }
            }
            *C += 1;
            if (((int)self->param1 & 0xff) == 0)
                *D1 -= 0x1000;
            else
                *D1 -= 0x800;
            if (*C > 0x3c) {
                *A = 0;
                *C = 0;
            }
        }
    }
}

#pragma pop

// @symbol _ZN16dScMgSmartball_c9EmitScoreEPiiii
extern "C" void dScMgSmartball_c::EmitScore(int *src, int v2, int v3, int v5) {
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

// @symbol _ZN16dScMgSmartball_c8KickGateEi
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
    idx = (int)(((long long)idx));
    mGateLampTimer[idx] = 0x14;
}

// @symbol _ZN16dScMgSmartball_c13IsTableAtRestEv
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

// @symbol _ZN16dScMgSmartball_c15ResolveBallPairEiiPhS0_
#pragma push
#pragma opt_common_subs off
void dScMgSmartball_c::ResolveBallPair(int firstIndex, int secondIndex, u8 *firstObject,
                                       u8 *secondObject) {
    // Keep the intermediate pair materialized across the angle calculation.
    // Named ball-pointer locals change register allocation; use direct casts.
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
    CopyObjectPos(&firstPos.x, (int *)firstObject);
    CopyObjectPos(&secondPos.x, (int *)secondObject);
    Vec2_Sub((int *)&positionDelta, (int *)&firstPos, (int *)&secondPos);
    delta.x = positionDelta.x;
    delta.y = positionDelta.y;
    collisionAngle = cstd::atan2(delta.y, delta.x);
    if (((cMgSmartball_ball_c *)firstObject)->HasHit() != 0) {
        firstVelocity.x = 0;
        firstVelocity.y = 0;
    } else {
        CopyObjectVel((int *)&firstSavedVelocity, (int *)firstObject);
        firstVelocity = firstSavedVelocity;
    }
    if (((cMgSmartball_ball_c *)secondObject)->HasHit() != 0) {
        secondVelocity.x = 0;
        secondVelocity.y = 0;
    } else {
        CopyObjectVel((int *)&secondSavedVelocity, (int *)secondObject);
        secondVelocity = secondSavedVelocity;
    }
    // Rotate into the line between the balls, exchange its velocity component,
    // then rotate back. The perpendicular components stay with their balls.
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
// @symbol _ZN16dScMgSmartball_c12CollideBallsEv
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void dScMgSmartball_c::CollideBalls() {
    char *p = (char *)this;
    int ballCount = mBallCount;
    int firstIndex, secondIndex;
    for (firstIndex = 0; firstIndex < ballCount; firstIndex++) {
        for (secondIndex = 0; secondIndex < ballCount; secondIndex++) {
            /* Stepped base, element 0: mpBalls[firstIndex] moves bytes. */
            int firstBall =
                (firstIndex >= 13)
                    ? 0
                    : *(int *)(int)&((dScMgSmartball_c *)(p + firstIndex * 4))->mpBalls[0];
            int secondBall = (secondIndex >= 13) ? 0 : (int)mpBalls[secondIndex];
            ResolveBallPair(firstIndex, secondIndex, (u8 *)firstBall, (u8 *)secondBall);
            ballCount = mBallCount;
        }
    }
}

#pragma pop

// @symbol _ZN16dScMgSmartball_c12SpawnObjectsEv
#pragma push
#pragma opt_strength_reduction off
/* SpawnObjects: minigame-board teardown+rebuild. 7 inlined-destructor loops
 * over object slot arrays (double vtable store + Memory::operator_delete2 +
 * slot=0), 3 single-slot deletes, language-selected asset loads
 * (func_02057d00 + DecompressLZ16 to VRAM/tilemaps), per-tile-type scan
 * loops over the two 0x300-entry tilemaps mBg1Map/mBg2Map collecting
 * spawn positions ((i%32)<<15, mapA: ((i/32)*8-0xf0)<<12 /
 * mapB: (i/32)<<15), spawn loops (operator new + per-type init, V2 passed via
 * pointer to a caller-local copy), a virtual-init pass (vtbl[2](this) on
 * every spawned object), bulk zeroing.
 *
 * Two pool words the disasm text left ambiguous: the +0x59c scan mask is
 * 0x3ff; the tile ID for the mpDokans object type is 0x3d2 (not 0x2d2/0x1d2).
 *
 * Load-bearing spellings (in order of impact):
 *  - Spawn callees take V2* + explicit caller-local copy (V2 tmp = pos[j];
 *    f(p,c,j,&tmp)). By-value V2 in C mode splits across r3/[sp] with
 *    dynamic sp adjustment, which pins a frame pointer the ROM lacks.
 *    operator new result flows through p into the slot store (no separate r var).
 *  - u64-mask laundering on the two mpKinokos spawn slot stores -> pooled
 *    0x46bc + str r0,[r1,rK]! writeback (also the 9th live value that pulls
 *    fp into the push set; frame 0x124 with 9 saved regs).
 *  - BOTH vtable stores in every teardown site are volatile ints: forces
 *    the per-loop pool reload of the shared base vtable (0213ed10) that
 *    mwccarm otherwise CSEs function-wide, and pins the preamble emission
 *    order (mov #0 before the pool loads).
 *  - Counter increments written as inline volatile RMWs
 *    ((*(volatile int *)&mKinokoCount)++): rematerializes the pooled counter
 *    address per loop instead of CSEing it across the loop pair.
 *  - In the 12 counted scan loops the pos[k] stores are volatile and the
 *    final shift is folded into the y-store (y kept un-shifted): this emits
 *    str x / add rFresh / lsl into dead-k / str. The 6 k-local
 *    (k++) scan loops keep plain stores with fully-shifted y locals.
 *  - Every loop counter is block-scoped (for (int i = ...)): stabilizes the
 *    per-loop register set (i=r4, const=r5, zero=r6, vtblB=r7, vtblA=sb,
 *    slot=sl) instead of rotating.
 *  - Virtual init calls pass this: ((void(**)(void*))(*(int*)p))[2](p)
 *    keeps the object in r0 and the vptr chain in r1.
 *  - Second mpKinokos spawn loop copies the V2 via int temps loaded y-then-x
 *    (int ty = pos[n].y; int tx = pos[n].x;) to keep its load order.
 *  - #pragma opt_strength_reduction off (as in the draft) keeps every loop
 *    un-strength-reduced; does not break the div/mod-by-32 shift idioms.
 */
void dScMgSmartball_c::SpawnObjects() {
    char *c = (char *)this;
    dScMgSmartball_c *self = (dScMgSmartball_c *)c;
    V2 pos[25];

    Mg_SubmitScore(0);

    for (int i = 0; i < 0xd; i++) {
        int **slot = (int **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpBalls));
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV19cMgSmartball_ball_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 8; i++) {
        int **slot = (int **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpAnas));
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV18cMgSmartball_ana_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 0x19; i++) {
        int **slot = (int **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpKinokos));
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_kinoko_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 3; i++) {
        int **slot = (int **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpPropellers));
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV24cMgSmartball_propeller_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 6; i++) {
        int **slot = (int **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpDokans));
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV20cMgSmartball_dokan_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 3; i++) {
        int **slot = (int **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpPakkuns));
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_pakkun_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 2; i++) {
        int **slot = (int **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpPushSwitches));
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV25cMgSmartball_pushswitch_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }

    {
        int *p = (int *)mpSlot;
        if (p != 0) {
            if (p != 0) {
                *p = (int)&_ZTV19cMgSmartball_slot_c[2];
                __cxa_vec_cleanup(((cMgSmartball_slot_c *)p)->mReelVel, 3, 8,
                                  NullDestructor_0203d47c);
                __cxa_vec_cleanup(((cMgSmartball_slot_c *)p)->mReelOffset, 3, 8,
                                  NullDestructor_0203d47c);
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            mpSlot = 0;
        }
    }
    {
        int *p = (int *)mpBoard;
        if (p != 0) {
            if (p != 0) {
                *p = (int)&_ZTV20cMgSmartball_board_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            mpBoard = 0;
        }
    }
    {
        int *p = (int *)mpWing;
        if (p != 0) {
            if (p != 0) {
                *p = (int)&_ZTV19cMgSmartball_wing_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            mpWing = 0;
        }
    }

    Mg_SetTopScreenYOffset(0x30);

    if (GetOwnerLanguage() == 5) {
        func_02057d00(sSmartballPathBuf, sSmartballLayoutNcgFmtSpanish, (int)self->mLevel & 1);
    } else if (GetOwnerLanguage() == 4) {
        func_02057d00(sSmartballPathBuf, sSmartballLayoutNcgFmtItalian, (int)self->mLevel & 1);
    } else if (GetOwnerLanguage() == 3) {
        func_02057d00(sSmartballPathBuf, sSmartballLayoutNcgFmtGerman, (int)self->mLevel & 1);
    } else if (GetOwnerLanguage() == 2) {
        func_02057d00(sSmartballPathBuf, sSmartballLayoutNcgFmtFrench, (int)self->mLevel & 1);
    } else if (((int)self->param1 & 0xff) == 0) {
        func_02057d00(sSmartballPathBuf, sSmartballLayoutNcgFmtE, (int)self->mLevel & 1);
    } else {
        func_02057d00(sSmartballPathBuf, sSmartballLayoutNcgFmt, (int)self->mLevel & 1);
    }
    {
        void *d = Ov004_LoadFileByPath(sSmartballPathBuf);
        DecompressLZ16(d, (void *)0x6400000);
        DecompressLZ16(d, (void *)0x6600000);
        Ov004_Deallocate(d);
    }
    func_02057d00(sSmartballPathBuf, sSmartballBg1NscFmt, (int)self->mLevel & 1);
    {
        void *d = Ov004_LoadFileByPath(sSmartballPathBuf);
        DecompressLZ16(d, mBg1Map);
        Ov004_Deallocate(d);
    }
    func_02057d00(sSmartballPathBuf, sSmartballBg2NscFmt, (int)self->mLevel & 1);
    {
        void *d = Ov004_LoadFileByPath(sSmartballPathBuf);
        DecompressLZ16(d, mBg2Map);
        Ov004_Deallocate(d);
    }
    if (((int)self->param1 & 0xff) == 0) {
        void *d = Ov004_LoadFileByPath(sSmartballLay0Bg1FrameNscPath);
        DecompressLZ16(d, G2::GetBG2ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath(sSmartballLay0Bg2FrameNscPath);
        DecompressLZ16(d, G2S::GetBG2ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath(sSmartballLay0Bg1NscPath);
        DecompressLZ16(d, G2::GetBG3ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath(sSmartballLay0Bg2NscPath);
        DecompressLZ16(d, G2S::GetBG3ScrPtr());
        Ov004_Deallocate(d);
    } else {
        void *d = Ov004_LoadFileByPath(sSmartballLay1Bg1FrameNscPath);
        DecompressLZ16(d, G2::GetBG2ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath(sSmartballLay1Bg2FrameNscPath);
        DecompressLZ16(d, G2S::GetBG2ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath(sSmartballLay1Bg1NscPath);
        DecompressLZ16(d, G2::GetBG3ScrPtr());
        Ov004_Deallocate(d);
        d = Ov004_LoadFileByPath(sSmartballLay1Bg2NscPath);
        DecompressLZ16(d, G2S::GetBG3ScrPtr());
        Ov004_Deallocate(d);
    }

    mBallCount = 0;
    mAnaCount = 0;
    mKinokoCount = 0;
    mDokanCount = 0;
    mPakkunCount = 0;
    mPropellerCount = 0;
    mPushSwitchCount = 0;

    __cxa_vec_ctor(pos, 0x19, 8, func_0203d738, NullDestructor_0203d47c);

    {
        int mask = 0x3ff;
        for (int i = 0; i < 0x300; i++) {
            int k = mKinokoCount;
            if (k >= 0x19)
                break;
            if ((mBg1Map[i] & mask) == 0x2e8) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xc;
                (*(volatile int *)&mKinokoCount)++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = mKinokoCount;
            if (k >= 0x19)
                break;
            if ((mBg2Map[i] & mask) == 0x2e8) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xf;
                (*(volatile int *)&mKinokoCount)++;
            }
        }
    }
    {
        int n = mKinokoCount;
        int j;
        for (j = 0; j < n; j++) {
            void *p = ::operator new(0x40);
            if (p != 0) {
                V2 tmp = pos[j];
                p = cMgSmartball_kinoko_c_Construct((char *)p, (int)c, (int)j, (int *)&tmp);
            }
            {
                void **slot =
                    (void **)(((long long)(int)((dScMgSmartball_c *)(c + j * 4))->mpKinokos));
                *slot = p;
                if (*slot != 0)
                    ((cMgSmartball_kinoko_c *)*slot)->mVariant = 0;
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
                    *(volatile int *)&pos[k].x = x;
                    *(volatile int *)&pos[k].y = y << 0xc;
                    (*(volatile int *)&mKinokoCount)++;
                }
            }
            for (int i = 0; i < 0x300; i++) {
                int k = mKinokoCount;
                if (k >= 0x19)
                    break;
                if ((mBg2Map[i] & mask) == 0x328) {
                    int x = (i % 0x20) << 0xf;
                    int y = i / 0x20;
                    *(volatile int *)&pos[k].x = x;
                    *(volatile int *)&pos[k].y = y << 0xf;
                    (*(volatile int *)&mKinokoCount)++;
                }
            }
        }
        for (; n < mKinokoCount; n++) {
            void *p = ::operator new(0x40);
            if (p != 0) {
                V2 tmp;
                int ty = pos[n].y;
                int tx = pos[n].x;
                tmp.x = tx;
                tmp.y = ty;
                p = cMgSmartball_kinoko_c_Construct((char *)p, (int)c, n, (int *)&tmp);
            }
            {
                void **slot =
                    (void **)(((long long)(int)((dScMgSmartball_c *)(c + n * 4))->mpKinokos));
                *slot = p;
                if (*slot != 0)
                    ((cMgSmartball_kinoko_c *)*slot)->mVariant = 1;
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
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xc;
                (*(volatile int *)&mAnaCount)++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = mAnaCount;
            if (k >= 8)
                break;
            if ((mBg2Map[i] & mask) == tile) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xf;
                (*(volatile int *)&mAnaCount)++;
            }
        }
        {
            int n = mAnaCount;
            int j;
            for (j = 0; j < n; j++) {
                void *p = ::operator new(0x38);
                if (p != 0) {
                    V2 tmp = pos[j];
                    p = (void *)cMgSmartball_ana_c_Construct((char *)p, (int)c, j, (int *)&tmp);
                }
                mpAnas[j] = (cMgSmartball_ana_c *)p;
                n = mAnaCount;
            }
        }
    }

    if (((int)self->param1 & 0xff) == 0) {
        mBallCount = 0xc;
    } else {
        mBallCount = 0xd;
    }
    {
        int j;
        for (j = 0; j < mBallCount; j++) {
            void *p;
            pos[j].x = ((1 - j) << 0x10) + 0xc0000;
            pos[j].y = cstd::fdiv(FX_MUL(pos[j].x, 0xa000), 0xd8000) + 0x90000;
            p = ::operator new(0x12c);
            if (p != 0) {
                V2 tmp = pos[j];
                p = (void *)cMgSmartball_ball_c_Construct((char *)p, (int)c, j, (int *)&tmp);
            }
            mpBalls[j] = (cMgSmartball_ball_c *)p;
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
                (*(volatile int *)&mDokanCount)++;
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
                (*(volatile int *)&mDokanCount)++;
            }
        }
        {
            int n = mDokanCount;
            int j;
            for (j = 0; j < n; j++) {
                void *p = ::operator new(0x34);
                if (p != 0) {
                    V2 tmp = pos[j];
                    p = (void *)cMgSmartball_dokan_c_Construct((char *)p, (int)c, j, (int *)&tmp);
                }
                mpDokans[j] = (cMgSmartball_dokan_c *)p;
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
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xc;
                (*(volatile int *)&mPakkunCount)++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = mPakkunCount;
            if (k >= 3)
                break;
            if ((mBg2Map[i] & mask) == tile) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xf;
                (*(volatile int *)&mPakkunCount)++;
            }
        }
        {
            int n = mPakkunCount;
            int j;
            for (j = 0; j < n; j++) {
                void *p = ::operator new(0x3c);
                if (p != 0) {
                    V2 tmp = pos[j];
                    p = (void *)cMgSmartball_pakkun_c_Construct((char *)p, (int)c, j, (int *)&tmp);
                }
                mpPakkuns[j] = (cMgSmartball_pakkun_c *)p;
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
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xc;
                (*(volatile int *)&mPropellerCount)++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = mPropellerCount;
            if (k >= 3)
                break;
            if ((mBg2Map[i] & mask) == 0x84) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xf;
                (*(volatile int *)&mPropellerCount)++;
            }
        }
        {
            int n = mPropellerCount;
            int j;
            for (j = 0; j < n; j++) {
                void *p = ::operator new(0x78);
                if (p != 0) {
                    V2 tmp = pos[j];
                    p = (void *)cMgSmartball_propeller_c_Construct((char *)p, (int)c, j,
                                                                   (int *)&tmp);
                }
                mpPropellers[j] = (cMgSmartball_propeller_c *)p;
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
            void *p = ::operator new(0x88);
            if (p != 0) {
                V2 tmp = pos[0];
                p = cMgSmartball_slot_c_Construct((char *)p, (int)c, (int)0, (int *)&tmp);
            }
            mpSlot = (cMgSmartball_slot_c *)p;
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
            void *p = ::operator new(0x9c);
            if (p != 0) {
                V2 tmp = pos[4];
                p = (void *)cMgSmartball_board_c_Construct((char *)p, (int)c, 0, (int *)&tmp);
            }
            mpBoard = (cMgSmartball_board_c *)p;
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
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xc;
                (*(volatile int *)&mPushSwitchCount)++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = mPushSwitchCount;
            if (k >= 2)
                break;
            if ((mBg2Map[i] & mask) == 0x50) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xf;
                (*(volatile int *)&mPushSwitchCount)++;
            }
        }
        {
            int n = mPushSwitchCount;
            int j;
            for (j = 0; j < n; j++) {
                void *p = ::operator new(0x38);
                if (p != 0) {
                    V2 tmp = pos[j];
                    p = (void *)cMgSmartball_pushswitch_c_Construct((char *)p, (int)c, j,
                                                                    (int *)&tmp);
                }
                mpPushSwitches[j] = (cMgSmartball_pushswitch_c *)p;
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
            void *p = ::operator new(0x88);
            if (p != 0) {
                V2 tmp = pos[0];
                p = cMgSmartball_wing_c_Construct((char *)p, (int)c, (int)0, (int *)&tmp);
            }
            mpWing = (cMgSmartball_wing_c *)p;
        }
    }

    for (int i = 0; i < mAnaCount; i++) {
        void *p = mpAnas[i];
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < mKinokoCount; i++) {
        void *p = mpKinokos[i];
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < mBallCount; i++) {
        void *p = mpBalls[i];
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    {
        void *p = mpSpring;
        ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < mPropellerCount; i++) {
        void *p = mpPropellers[i];
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < mDokanCount; i++) {
        void *p = mpDokans[i];
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < mPakkunCount; i++) {
        void *p = mpPakkuns[i];
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < mPushSwitchCount; i++) {
        void *p = mpPushSwitches[i];
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    {
        void *p = mpSlot;
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    {
        void *p = mpBoard;
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    {
        void *p = mpWing;
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
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
        *(int *)((dScMgSmartball_c *)(c + i * 8))->mPopupPos = 0;
        *(int *)&((dScMgSmartball_c *)(c + i * 8))->mPopupPos[4] = 0;
        mPopupDelay[i] = 0;
    }
    for (int i = 0; i < 0x10; i++) {
        mDustActive[i] = 0;
        mDustAge[i] = 0;
        *(int *)((dScMgSmartball_c *)(c + i * 8))->mDustPos = 0;
        *(int *)&((dScMgSmartball_c *)(c + i * 8))->mDustPos[4] = 0;
        *(int *)((dScMgSmartball_c *)(c + i * 8))->mDustVel = 0;
        *(int *)&((dScMgSmartball_c *)(c + i * 8))->mDustVel[4] = 0;
    }
    mPhase = 0;
    mResultSlideX = 0x100;
    SetSubBg0Offset(mResultSlideX, 0);
    ResetSparks();
    __cxa_vec_cleanup(pos, 0x19, 8, NullDestructor_0203d47c);
}

#pragma pop

// @symbol _ZN16dScMgSmartball_c6RenderEv
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
// Spill-order levers (each pins temps to stack slots in order):
//  - plain (non-u64) pcnt for loop C -> pcnt@sp+0x10
//  - block-scope pcnt2 for loop G -> val@sp+0x14
//  - int *slot[2] with early slot[0]=0, slot[1]=loopG pcnt, slot[0]=loop2 pmode
//    forces spill order pmode@0x18 before pcnt2@0x1c
// Loop2 digit-count still uses inner u64 mask form for pooled ldr form.
//
/* dScMgSmartball_c::Render -- vtable slot 9: the third of the three slots
 * where this class's table differs from dScMgBase_c's.
 *
 * The result screen's itemised score, one row per bonus category, gated on how far
 * mResultTimer, the reveal counter, has run; then the two flipper lamps, then every live
 * display object's slot-1 virtual, in the ROM's order.
 *
 * Only the receiver changed: the body below has `this` where the void*
 * parameter was; everything else here -- the two pragmas, the spill-order
 * levers above, the `slot[2]` array whose two entries are written in that
 * particular order -- is load-bearing. */
/* Both pragmas are load-bearing, not tidying: without them the function
 * emits 0x1170 bytes instead of 0x10c0. Do not remove. */
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

        /* Block B: row 0 */
        if (mCellTally > 0) {
            Mg_RenderSpriteBoth(data_ov006_02138ae0[0], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0x64, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth((void *)*(int *)(sSmartballTextSpritesByLanguage[i] + 0x54), 0x90,
                                xpos, -1, -1);
            Mg_DrawNumber(0xa0, xpos, mCellTally, 0, 0, 1, 0);
            Mg_DrawNumber(0xe0, xpos, mCellTally * 0x64, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Loop C: rows 1..4 */
        limit = col * 0x14;
        for (row = 0; row < 4; row++) {
            if (mResultTimer >= limit && mLineTally[row] > 0) {
                Mg_RenderSpriteBoth(data_ov006_02138ae0[row + 1], 0x30, xpos, -1, -1);
                val = sSmartballCellScores[row];
                Mg_DrawNumber(0x80, xpos, val, 0, 0, 1, 0);
                i = GetGameLanguage();
                Mg_RenderSpriteBoth((void *)*(int *)(sSmartballTextSpritesByLanguage[i] + 0x54),
                                    0x90, xpos, -1, -1);
                /* Stepped base, element 0: &mLineTally[row] moves bytes. */
                pcnt = ((dScMgSmartball_c *)(g + row * 4))->mLineTally;
                Mg_DrawNumber(0xa0, xpos, *pcnt, 0, 0, 0, 0);
                Mg_DrawNumber(0xe0, xpos, val * *pcnt, 0, 0, 1, 0);
                xpos += 0x18;
                limit += 0x14;
                col++;
            }
        }

        /* Block D */
        if (mKinokoTally > 0 && mResultTimer >= col * 0x14) {
            Mg_RenderSpriteBoth(sSmartballKinokoSprites[0], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0x12c, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth((void *)*(int *)(sSmartballTextSpritesByLanguage[i] + 0x54), 0x90,
                                xpos, -1, -1);
            Mg_DrawNumber(0xa0, xpos, mKinokoTally, 0, 0, 0, 0);
            Mg_DrawNumber(0xe0, xpos, mKinokoTally * 0x12c, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Block E */
        if (mDokanTally > 0 && mResultTimer >= col * 0x14) {
            Mg_RenderSpriteBoth(data_ov006_02137a78[0], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0xbb8, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth((void *)*(int *)(sSmartballTextSpritesByLanguage[i] + 0x54), 0x90,
                                xpos, -1, -1);
            Mg_DrawNumber(0xa0, xpos, mDokanTally, 0, 0, 0, 0);
            Mg_DrawNumber(0xe0, xpos, mDokanTally * 0xbb8, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Block F */
        if (mAnaTally > 0 && mResultTimer >= col * 0x14) {
            Mg_RenderSpriteBoth(data_ov006_02137a6c[0], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0x7d0, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth((void *)*(int *)(sSmartballTextSpritesByLanguage[i] + 0x54), 0x90,
                                xpos, -1, -1);
            Mg_DrawNumber(0xa0, xpos, mAnaTally, 0, 0, 0, 0);
            Mg_DrawNumber(0xe0, xpos, mAnaTally * 0x7d0, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Loop G: rows with per-row multiplier */
        limG = col * 0x14;
        for (j = 0; j < 3; j++) {
            /* Stepped base, element 0: &mAwardTally[j] moves bytes. */
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
                Mg_RenderSpriteBoth((void *)*(int *)(sSmartballTextSpritesByLanguage[i] + 0x54),
                                    0x90, xpos, -1, -1);
                Mg_DrawNumber(0xa0, xpos, *slot[1], 0, 0, 0, 0);
                Mg_DrawNumber(0xe0, xpos, mult * *slot[1], 0, 0, 1, 0);
                xpos += 0x18;
                limG += 0x14;
                col++;
            }
        }

        /* Block H */
        if (mBingoTally > 0 && mResultTimer >= col * 0x14) {
            Mg_RenderSpriteBoth(data_ov006_02138ae0[5], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0x2710, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth((void *)*(int *)(sSmartballTextSpritesByLanguage[i] + 0x54), 0x90,
                                xpos, -1, -1);
            Mg_DrawNumber(0xa0, xpos, mBingoTally, 0, 0, 0, 0);
            Mg_DrawNumber(0xe0, xpos, mBingoTally * 0x2710, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Block I */
        if (mOutTally > 0 && mResultTimer >= col * 0x14) {
            Mg_RenderSpriteBoth(sSmartballBallSprites[0], 0x30, xpos, -1, -1);
            Mg_DrawNumber(0x80, xpos, 0x7d0, 0, 0, 1, 0);
            i = GetGameLanguage();
            Mg_RenderSpriteBoth((void *)*(int *)(sSmartballTextSpritesByLanguage[i] + 0x54), 0x90,
                                xpos, -1, -1);
            Mg_DrawNumber(0xa0, xpos, mOutTally, 0, 0, 0, 0);
            Mg_DrawNumber(0xe0, xpos, mOutTally * 0x7d0, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Block J: timer */
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

        /* Block K */
        if (mPhase == 1 && mResultTimer >= 0x12c) {
            if (col >= 7)
                Mg_DrawBlinkSprite(g, 0x80, 0xa8);
            else if (col == 6)
                Mg_DrawBlinkSprite(g, 0xe0, 0xa8);
            else
                Mg_DrawBlinkSprite(g, 0xe0, 0xa0);
        }
    }

    /* merge */
    if ((param1 & 0xff) != 0 || mDraining == 0)
        ((dScMgSmartball_c *)(g))->RenderSparks();

    /* Section 2 */
    Mg_DrawTopRightCounter(Mg_GetSubmittedScore(), 6);
    Mg_DrawTopLeftCounter((void *)Mg_GetTopRecord());

    /* Loop 2: item list (sb reused as index) */
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
                    /* mPopupScore[sb] again, through an int address: the
                       member spellings, and re-reading *pcnt, all move bytes. */
                    u = (unsigned int)*(int *)((int)g + sb * 4 + 0x478c);
                    if (u != 0) {
                        do {
                            u /= 10;
                            nd++;
                        } while (u != 0);
                    }
                    i = GetGameLanguage();
                    Mg_RenderSpriteBoth((void *)*(int *)(sSmartballTextSpritesByLanguage[i] + 0x54),
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
                    int y = *(int *)(((dScMgSmartball_c *)(g + sb * 8))->mPopupPos + 4);
                    int x = *(int *)((dScMgSmartball_c *)(g + sb * 8))->mPopupPos;
                    Mg_DrawNumber(x >> 12, y >> 12, cnt, -1, 0, 0, 0);
                }
            }
        }
    }

    /* Loop 3: 16 slots */
    {
        int slot;
        int two = 2, neg = -1, zero = 0;
        for (slot = 0; slot < 0x10; slot++) {
            if (mDustActive[slot] == 1) {
                int idx = mDustAge[slot] / 5;
                if (idx > 3)
                    idx = two;
                Mg_RenderSpriteBoth(
                    data_ov006_02137a60[idx],
                    *(int *)((dScMgSmartball_c *)(g + slot * 8))->mDustPos >> 12,
                    *(int *)(((dScMgSmartball_c *)(g + slot * 8))->mDustPos + 4) >> 12, neg, zero);
            }
        }
    }

    /* Block M */
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

    /* Block N */
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

    /* Dispatch loops */
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

// @symbol _ZN16dScMgSmartball_c8BehaviorEv
#pragma push
#pragma opt_strength_reduction off
/* dScMgSmartball_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the three slots where this class's table
 * differs from dScMgBase_c's. The old file's naming comment agreed, and
 * here it is right.
 *
 * State 0 runs the table: every live object's slot-0 virtual is called through its
 * own vptr, one array at a time (balls, mushrooms, holes, propellers, pipes,
 * piranhas, push switches, then the three singletons), then the physics, the score
 * and the ball-lost check. State 1 is the game-over hold, state 2 the hand-off out.
 * The opening `0xc4 == 0` block is dScMgBase_c's shared counter idiom -- the same
 * three fields Flower's and MCarlo's own Behaviors arm, which is why
 * include/dScMgBase_c.h names 0xc0/0xc3/0xc4.
 *
 * THE `(long long)(int)` CASTS ARE LAUNDERS. They force the address through a wider
 * temporary so mwccarm re-materialises it instead of strength-reducing the loop, and
 * the pragma below is there for the same reason: neither is a spelling that can be
 * simplified without changing bytes.
 *
 * WAS A C99 FILE, so the declarations move inside `extern "C"`; in C++ they would
 * mangle and resolve to nothing. The `extern` on the four variables is load-bearing
 * -- without it a declaration inside `extern "C" {}` is a definition and collides
 * with the delinked gap object. */
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
            o = *(char **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpBalls));
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mKinokoCount; i++) {
            o = *(char **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpKinokos));
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mAnaCount; i++) {
            o = *(char **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpAnas));
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mPropellerCount; i++) {
            o = *(char **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpPropellers));
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mDokanCount; i++) {
            o = *(char **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpDokans));
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mPakkunCount; i++) {
            o = *(char **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpPakkuns));
            (**(VFunc **)o)(o);
        }
        for (i = 0; i < mPushSwitchCount; i++) {
            o = *(char **)(((long long)(int)((dScMgSmartball_c *)(c + i * 4))->mpPushSwitches));
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

        ((dScMgSmartball_c *)(c))->UpdateScores();
        ((dScMgSmartball_c *)(c))->UpdateDust();
        ((dScMgSmartball_c *)(c))->CollideBalls();

        if (((dScMgSmartball_c *)(c))->IsTableAtRest() != 0) {
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
                        ((cMgSmartball_ball_c *)((i >= 13) ? 0 : (char *)mpBalls[i]))
                                ->ArmExpire(1) != 0) {
                        ((cMgSmartball_ball_c *)((int *)((i >= 13) ? 0 : (char *)mpBalls[i])))
                            ->Burst();
                        CopyObjectPos(vec, (int *)((i >= 13) ? 0 : (char *)mpBalls[i]));
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
        ((dScMgSmartball_c *)(c))->UpdateScores();
        mPromptEnabled = 0;
        if (mResultTimer == 0) {
            data_0209d454[0] |= 1;
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
        ((dScMgSmartball_c *)(c))->UpdateScores();
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

// @symbol _ZN16dScMgSmartball_c10StartRoundEi
#include "G2x.h"
#include "types.h"
#include "dScMgSmartball_c.h"
/* vtable slot 18 -- an override of
   dScMgBase_c::StartRound(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
extern "C" {
void dScMgSmartball_c::StartRound(int /* arg */) {
    SpawnObjects();
    G2x::SetBlendAlpha((volatile unsigned short *)0x4000050, 0, 0x18, 4, 0xa);
    G2x::SetBlendAlpha((volatile unsigned short *)0x4001050, 0, 0x18, 4, 0xa);
}
}

// @symbol _ZN16dScMgSmartball_c15InitSubScreenBgEv
/* dScMgSmartball_c::InitSubScreenBg - slot 31, this class's own override.

   Not a Kill: see the slot-31 block in include/dScMgBase_c.h. Same shape as
   dScMgAmida_c's -- BG1CNT base bits 4 -- differing only in the language
   table it indexes. */
int dScMgSmartball_c::InitSubScreenBg() {
    int f;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 4;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~0x40;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    SetSubBg1Offset(0, 0);
    data_0209d454[0] &= ~2;
    f = GetGameLanguage();
    LoadCompressedFileAt(sSmartballOptionNcgByLanguage[f], (void *)G2S::GetBG1CharPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_OPTION_NSC_BIN, G2S::GetBG1ScrPtr());
}

// @symbol _ZN16dScMgSmartball_c13InitResourcesEv
#pragma push
#pragma opt_propagation off
/* dScMgSmartball_c::InitResources() -- vtable slot 0, the scene
 * setup for the Smartball (Bounce and Pounce) minigame. Clamps the saved
 * high score, brings up the two screens' BG layers, decompresses one of two
 * tile/palette sets depending on the mode byte at +8, zeroes the seven
 * per-object state arrays, builds the scene's one heap object, and finishes
 * by forcing every listed OAM sprite to blend mode 1 with priority 0.
 *
 * Two shapes below are load-bearing, not style:
 *   - the saved score is narrowed with `(u8)(param & 0xFF)`, and BOTH halves
 *     of that are needed: the ROM masks the byte twice (`ands r0,r0,#0xff`
 *     for the `if`, then `and r0,r0,#0xff` before the store), and the cast
 *     alone or the mask alone lets mwccarm fold the pair into one, costing
 *     four bytes. This replaces the u64 cast the first draft used for the
 *     same effect: the plain narrowing conversion emits the same pair,
 *     so no launder is needed here at all.
 *   - the last OAM block reads its old attr2 from sSmartballPakkunSprites[2],
 *     NOT from [2]+1 that it then writes. The ROM does the same, so the
 *     original macro expansion there was fed the wrong pointer.
 *
 * The offsets still spelled raw off `c` all fall inside dScMgSmartball_c.h's
 * pad_4660, which that header records as real access it has not modelled
 * yet: the seven per-object slot arrays at 0x4688..0x4770,
 * their three singletons at 0x4684/0x4778..0x4780, and the high score at 0xbc.
 * Reading their widths is a separate pass; nothing here shadows a type.
 */
inline u32 OamWordVal(OamAttr *arg0) {
    return *((u32 *)arg0);
}

inline OamAttr *OamNext(OamAttr *arg0) {
    return arg0 + 1;
}

inline u32 *OamWord(OamAttr *arg0) {
    return (u32 *)arg0;
}

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
    *((volatile u16 *)0x4001008) = ((*((volatile u16 *)0x4001008)) & 0x43) | 0x520c;
    *((volatile u16 *)0x4001008) &= ~0x40;
    *((volatile s32 *)0x4001010) = 0x100;
    *((volatile u16 *)0x4001008) &= ~3;
    f = Ov004_LoadFileByPath(&sSmartballResultBgNcgPath);
    DecompressLZ16(f, G2S::GetBG0CharPtr());
    Ov004_Deallocate(f);
    {
        volatile u16 v;
        MultiStore16((u16)(v = 0), G2S::GetBG0ScrPtr(), 0x1000);
    }
    f = Ov004_LoadFileByPath(&sSmartballResultBgNscPath);
    DecompressLZ16(f, G2S::GetBG0ScrPtr());
    Ov004_Deallocate(f);
    f = Ov004_LoadFileByPath(&sSmartballResultNclPath);
    CP15::FlushAndInvalidateDataCache((u32)f, 0x20);
    GXS::LoadBGPltt(((char *)f) + 2, 2, 0x1e);
    Ov004_Deallocate(f);
    *((volatile u16 *)0x400000c) = (((*((volatile u16 *)0x400000c)) & 0xFFFF) & 0x43) | 0x1410;
    *((volatile u16 *)0x400000c) &= ~0x40;
    *((volatile s32 *)0x4000018) = 0;
    *((volatile u16 *)0x400000c) = ((*((volatile u16 *)0x400000c)) & (~3)) | 1;
    *((volatile u16 *)0x400100c) = ((*((volatile u16 *)0x400100c)) & 0x43) | 0x1410;
    *((volatile u16 *)0x400100c) &= ~0x40;
    *((volatile s32 *)0x4001018) = 0;
    *((volatile u16 *)0x400100c) = ((*((volatile u16 *)0x400100c)) & (~3)) | 1;
    *((volatile u16 *)0x400000e) = ((*((volatile u16 *)0x400000e)) & 0x43) | 0x1618;
    *((volatile u16 *)0x400000e) &= ~0x40;
    *((volatile s32 *)0x400001c) = 0;
    *((volatile u16 *)0x400000e) = ((*((volatile u16 *)0x400000e)) & (~3)) | 2;
    *((volatile u16 *)0x400100e) = ((*((volatile u16 *)0x400100e)) & 0x43) | 0x1618;
    *((volatile u16 *)0x400100e) &= ~0x40;
    *((volatile s32 *)0x400101c) = 0;
    *((volatile u16 *)0x400100e) = ((*((volatile u16 *)0x400100e)) & (~3)) | 2;
    if (((u8)(*((s32 *)(c + 8)))) == 0) {
        f = Ov004_LoadFileByPath(&sSmartballLay0Bg1FrameNcgPath);
        DecompressLZ16(f, G2::GetBG2CharPtr());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath(&sSmartballLay0Bg2FrameNcgPath);
        DecompressLZ16(f, G2S::GetBG2CharPtr());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath(&sSmartballLay0Bg1NcgPath);
        DecompressLZ16(f, (void *)func_02054d88());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath(&sSmartballLay0Bg2NcgPath);
        DecompressLZ16(f, G2S::GetBG3CharPtr());
        Ov004_Deallocate(f);
    } else {
        f = Ov004_LoadFileByPath(&sSmartballLay1Bg1FrameNcgPath);
        DecompressLZ16(f, G2::GetBG2CharPtr());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath(&sSmartballLay1Bg2FrameNcgPath);
        DecompressLZ16(f, G2S::GetBG2CharPtr());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath(&sSmartballLay1Bg1NcgPath);
        DecompressLZ16(f, (void *)func_02054d88());
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath(&sSmartballLay1Bg2NcgPath);
        DecompressLZ16(f, G2S::GetBG3CharPtr());
        Ov004_Deallocate(f);
    }
    if (((u8)(*((s32 *)(c + 8)))) == 0) {
        f = Ov004_LoadFileByPath(&sSmartballLay0Bg1NclPath);
        CP15::FlushAndInvalidateDataCache((u32)f, 0x1a0);
        GX::LoadBGPltt(f, 0x60, 0x1a0);
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath(&sSmartballLay0Bg2NclPath);
        CP15::FlushAndInvalidateDataCache((u32)f, 0x1a0);
        GXS::LoadBGPltt(f, 0x60, 0x1a0);
        Ov004_Deallocate(f);
    } else {
        f = Ov004_LoadFileByPath(&sSmartballLay1Bg1NclPath);
        CP15::FlushAndInvalidateDataCache((u32)f, 0x1a0);
        GX::LoadBGPltt(f, 0x60, 0x1a0);
        Ov004_Deallocate(f);
        f = Ov004_LoadFileByPath(&sSmartballLay1Bg2NclPath);
        CP15::FlushAndInvalidateDataCache((u32)f, 0x1a0);
        GXS::LoadBGPltt(f, 0x60, 0x1a0);
        Ov004_Deallocate(f);
    }
    data_0209d45c &= ~1;
    data_0209d45c |= 0xc;
    data_0209d454[0] |= 0xd;
    f = Ov004_LoadFileByPath(&sSmartballNclPath);
    CP15::FlushAndInvalidateDataCache((u32)f, 0x100);
    GX::LoadOBJPltt(f, 0, 0x100);
    GXS::LoadOBJPltt(f, 0, 0x100);
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
        cMgSmartballPair r1;
        void *o;
        r1.v0 = 0xec000;
        r1.v1 = 0xa0000;
        o = ::operator new(0x48);
        if (o != 0) {
            cMgSmartballPair r2 = r1;
            o = (void *)cMgSmartball_spring_c_Construct((char *)o, (int)c, 0, &r2.v0);
        }
        mpSpring = (cMgSmartball_spring_c *)o;
    }
    mpSlot = 0;
    mpBoard = 0;
    mpWing = 0;
    G2x::SetBlendAlpha((volatile u16 *)0x4000050, 0, 0x18, 4, 0xa);
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 0, 0x18, 4, 0xa);
    do {
        OamAttr *_p = sSmartballBallSprites[1];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (u16)((_p->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    do {
        OamAttr *_p = sSmartballBallSpritesAlt[1];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (u16)((_p->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    do {
        OamAttr *_p = sSmartballPushSwitchSprites[2];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (u16)((_p->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    do {
        OamAttr *_p = sSmartballPropellerSprites[1];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (u16)((_p->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    do {
        OamAttr *_p = sSmartballKinokoSprites[1];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (u16)((_p->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    do {
        OamAttr *_p = sSmartballKinokoTurnSprites[1];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (u16)((_p->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    do {
        OamAttr *_p = sSmartballSpringSprites[1];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = (OamWordVal(_p) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (u16)((_p->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    do {
        OamAttr *_p = sSmartballSpringSpritesAlt[1];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = (OamWordVal(_p) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (u16)((_p->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    do {
        OamAttr *_p = sSmartballPakkunSprites[2];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = (OamWordVal(_p) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (_p->attr2 & (~0xf000)) | (_n << 0xc);
    } while (0);
    do {
        OamAttr *_p = sSmartballPakkunSprites[2] + 1;
        u16 _a = sSmartballPakkunSprites[2]->attr2;
        u32 _n;
        *OamWord(sSmartballPakkunSprites[2] + 1) =
            (OamWordVal(OamNext(sSmartballPakkunSprites[2])) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        OamNext(sSmartballPakkunSprites[2])->attr2 =
            (u16)((OamNext(sSmartballPakkunSprites[2])->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    do {
        OamAttr *_p = sSmartballWingSprites[1];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = (OamWordVal(_p) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (u16)((_p->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    do {
        OamAttr *_p = sSmartballWingBodySprites[1];
        u16 _a = _p->attr2;
        u32 _n;
        *OamWord(_p) = (OamWordVal(_p) & (~0xc00)) | 0x400;
        _n = (_a & 0xf000) >> 0xc;
        _p->attr2 = (u16)((_p->attr2 & (~0xf000)) | (_n << 0xc));
    } while (0);
    ((dScMgSmartball_c *)(c))->SpawnObjects();
    return 1;
}

#pragma pop

// @symbol _ZN16dScMgSmartball_c21AfterCleanupResourcesEj
#pragma push
#pragma opt_strength_reduction off
/* dScMgSmartball_c::AfterCleanupResources -- vtable slot 5.
 *
 * Attributed by the ROM's vtable: one of the three slots where this class's table
 * differs from dScMgBase_c's. The old file's naming comment agreed, and
 * here it is right.
 *
 * Tears the pinball table down object by object -- balls, mushrooms, holes, the
 * spring, propellers, pipes, piranhas, push switches, then the three singletons.
 * Each pointer slot is walked, the object's vptr rewound to its base's before the
 * free (which is a destructor fully inlined into two vtable stores, the same shape
 * include/dScMgBase_c.h describes for TouchIcon_c), and the slot nulled.
 *
 * THE PARAMETER IS u32, NOT int. The pre-migration spelling was `int mode` and it
 * emitted the same, because the only use is a compare against 2 -- but the virtual being
 * overridden is dScMgBase_c's `void AfterCleanupResources(u32)`, so that is the
 * signature. The chain at the end is a QUALIFIED call, which compiles to the direct
 * `bl` the ROM has; unqualified it would become a virtual dispatch through this
 * very slot and recurse.
 *
 * WAS A C99 FILE. Its three declarations are gone rather than moved: decl_common.h
 * already declares __cxa_vec_cleanup and NullDestructor_0203d47c with C linkage, and
 * the base chain is a real call now. The pragma is load-bearing, not tidying. */
void dScMgSmartball_c::AfterCleanupResources(u32 vfSuccess) {
    char *c = (char *)this;

    if (vfSuccess != 2)
        return;

    for (int i = 0; i < 0xd; i++) {
        int **slot = (int **)((dScMgSmartball_c *)(c + i * 4))->mpBalls;
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV19cMgSmartball_ball_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    mBallCount = 0;

    for (int i = 0; i < 0x19; i++) {
        int **slot = (int **)((dScMgSmartball_c *)(c + i * 4))->mpKinokos;
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_kinoko_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    mKinokoCount = 0;

    for (int i = 0; i < 8; i++) {
        int **slot = (int **)((dScMgSmartball_c *)(c + i * 4))->mpAnas;
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV18cMgSmartball_ana_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    mAnaCount = 0;

    {
        int *p = (int *)mpSpring;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_spring_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            mpSpring = 0;
        }
    }

    for (int i = 0; i < 3; i++) {
        int **slot = (int **)((dScMgSmartball_c *)(c + i * 4))->mpPropellers;
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV24cMgSmartball_propeller_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }

    for (int i = 0; i < 6; i++) {
        int **slot = (int **)((dScMgSmartball_c *)(c + i * 4))->mpDokans;
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV20cMgSmartball_dokan_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    mDokanCount = 0;

    for (int i = 0; i < 3; i++) {
        int **slot = (int **)((dScMgSmartball_c *)(c + i * 4))->mpPakkuns;
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_pakkun_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    mPakkunCount = 0;

    for (int i = 0; i < 2; i++) {
        int **slot = (int **)((dScMgSmartball_c *)(c + i * 4))->mpPushSwitches;
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)&_ZTV25cMgSmartball_pushswitch_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            *slot = 0;
        }
    }
    mPushSwitchCount = 0;

    {
        int *p = (int *)mpSlot;
        if (p != 0) {
            if (p != 0) {
                *p = (int)&_ZTV19cMgSmartball_slot_c[2];
                __cxa_vec_cleanup(((cMgSmartball_slot_c *)p)->mReelVel, 3, 8,
                                  NullDestructor_0203d47c);
                __cxa_vec_cleanup(((cMgSmartball_slot_c *)p)->mReelOffset, 3, 8,
                                  NullDestructor_0203d47c);
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            mpSlot = 0;
        }
    }
    {
        int *p = (int *)mpBoard;
        if (p != 0) {
            if (p != 0) {
                *p = (int)&_ZTV20cMgSmartball_board_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            mpBoard = 0;
        }
    }
    {
        int *p = (int *)mpWing;
        if (p != 0) {
            if (p != 0) {
                *p = (int)&_ZTV19cMgSmartball_wing_c[2];
                *(int volatile *)p = (int)&_ZTV21cMgSmartball_object_c[2];
                Memory::operator_delete2(p);
            }
            mpWing = 0;
        }
    }

    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

#pragma pop

/* The scene factory and its empty particle-element constructor share this unit. */
extern "C" void SmartballObj_ElemCtor(void);

/* Inline, so the factory below holds it: dScMgBase_c's C2 and this class's
   vtable are the compiler's, then the four arrays the destructor tears down. */
inline dScMgSmartball_c::dScMgSmartball_c() {
    __cxa_vec_ctor(mPopupPos, 5, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
    __cxa_vec_ctor(mDustPos, 0x10, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
    __cxa_vec_ctor(mDustVel, 0x10, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
    __cxa_vec_ctor(mSparks, 0x40, 0x24, (void *)SmartballObj_ElemCtor,
                   (void *)SmartballObj_ElemDtor);
}

// @symbol dScMgSmartball_c_classInit
/* Allocates and constructs the scene; returns null if allocation fails.
 * Historical alias: MgBingoBallSlotsShot_Spawn. */
extern "C" void *dScMgSmartball_c_classInit(void) {
    return new dScMgSmartball_c;
}

// @symbol SmartballObj_ElemCtor
extern "C" void SmartballObj_ElemCtor(void) {}