//cpp
/* ==========================================================================
 * daPkn_c -- the piranha plant, ov084 0x0212eaf0..0x02130174.
 *
 * THE NAME IS THE CARTRIDGE'S OWN.  ov084 0x02130bec holds the bytes
 * "7daPkn_c\0" -- the length-prefixed mangled type name -- _ZTI7daPkn_c at
 * 0x02130bf8 points its +4 word back at that string, and the vtable's -4
 * header word at 0x02130c24 points back at the _ZTI.  The class was carried
 * here under the coined name PiranhaPlant, which occurs in none of the 106
 * extracted images; that spelling is gone.  The reconstructed factory
 * daPkn_c_classInit (historical alias PiranhaPlant_Spawn) builds it for the
 * PAKUN registry profile at 0x02130c04, whose id is 0xfa.
 *
 * BASE: dEnemyBase_c, proven by the destructor at 0x0212eaf0 -- it stores this
 * class's vtable, destroys six member subobjects in reverse declaration order,
 * and tails into _ZN12dEnemyBase_cD2Ev.  Chain:
 * daPkn_c -> dEnemyBase_c -> dActor_c -> dBase_c -> fBase_c.  The vtable is 31
 * words, which is dEnemyBase_c's length exactly: this class adds no new
 * virtuals, it only overrides eight of the inherited slots, and all eight sit
 * inside this text run.
 *
 * THE VTABLE EXTENT IS EXACT, WHICH IS UNUSUAL.  _ZTV7daPkn_c at 0x02130c28 is
 * the LAST datum in ov084's .data section, so its length is fixed by the end
 * of the section rather than by whatever symbol happens to follow.  The tree
 * has a known _ZTV extent defect that reads both long and short depending on
 * what follows a vtable; this class is immune to it, and the 31-slot figure
 * above is a section boundary, not an inference.
 *
 * ROUTE: TEXT-ONLY.  ov084 delinks no .data at all -- every entry in
 * config/arm9/overlays/ov084/delinks.txt is a .text range -- so this TU
 * contributes code and nothing else.  It still COMPILES a vtable and an RTTI
 * group, because the key function lives here (see below); those sections are
 * licensed as compiler_only_output with disposition deadstrip-data in
 * config/tu_manifest.d/ov084/daPkn_c.json and are dropped at link time.  The
 * cartridge's own copies stay ROM-supplied.
 *
 * THE DESTRUCTOR IS INLINE, AND THAT IS MEASURED.  The cartridge has D1 at
 * 0x0212eaf0 BELOW D0 at 0x0212eb48 and carries no D2 anywhere.  That is what
 * mwccarm 2004/b56 emits for an inline in-class destructor; the out-of-line
 * form emits D2/D0/D1, in the wrong order and with a homeless D2.  So
 * include/daPkn_c.h carries the empty in-class body and this file writes no
 * destructor at all: both ROM bodies are compiler output from the typed member
 * list.  With the destructor inline, daPkn_c::OnAimedAtWithEgg becomes the
 * first out-of-line virtual the class declares -- the key function -- so this
 * TU is where the compiler puts _ZTV7daPkn_c, _ZTI7daPkn_c and _ZTS7daPkn_c.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S.  mwccarm emits one .text section
 * per function in the reverse of source order, so the highest-address ROM
 * function (daPkn_c_classInit, 0x02130110) is written FIRST here and the
 * lowest (func_ov084_0212ebb4) LAST.  The two destructor bodies are compiler
 * output and land below everything written, which is where the ROM has them.
 * Do not reorder.
 *
 * THE STATE MACHINE.  mState (+0x458) selects one of nine bodies through the
 * pointer-to-member table __sinit_ov084_02130654 builds into .bss at
 * 0x02130e80.  daPkn_c::Behavior dispatches it.  Those nine .data records are
 * resolved BY SYMBOL NAME by dsd, not by address, so the free functions they
 * name keep their func_ov084_* spellings: renaming one is a same-commit edit
 * to config/arm9/overlays/ov084/symbols.txt, and a mangled TU sitting beside a
 * stale name there links every one of those words as 0.  Naming them is the
 * next improvement and is deliberately not attempted here.
 *
 * DECLARATIONS.  There is ONE file-scope extern "C" region, and it carries only
 * what the C++-named members need: a member function may not sit inside a
 * block-scope linkage specification, so those bodies cannot declare anything
 * themselves.  Every free member below is wrapped in its own extern "C" block
 * and declares what it alone needs, which is how two members of this TU can
 * hold different views of one symbol.
 *
 * include/decl_common.h is deliberately NOT included.  It declares five of this
 * TU's own members with signatures that contradict the definitions here
 * (func_ov084_0212ef00 as void where it returns int, func_ov084_0212ec60 over
 * void* where the body works a char*), and it restates _ZTV7daPkn_c, which
 * include/daPkn_c.h now owns.  The decl_* headers this file does include carry
 * nothing this file redefines.
 *
 * Assembled from these 24 legacy one-function sources (ROM address order):
 *   [0]  0x0212eaf0  _ZN7daPkn_cD1Ev.cpp                 -> compiler output
 *   [1]  0x0212eb48  _ZN7daPkn_cD0Ev.cpp                 -> compiler output
 *   [2]  0x0212ebb4  func_ov084_0212ebb4.c
 *   [3]  0x0212ec04  func_ov084_0212ec04.c
 *   [4]  0x0212ec58  _ZN7daPkn_c16OnAimedAtWithEggEv.cpp
 *   [5]  0x0212ec60  func_ov084_0212ec60.c
 *   [6]  0x0212ef00  func_ov084_0212ef00.c
 *   [7]  0x0212f1d0  func_ov084_0212f1d0.c
 *   [8]  0x0212f204  func_ov084_0212f204.c
 *   [9]  0x0212f298  func_ov084_0212f298.c
 *   [10] 0x0212f2dc  func_ov084_0212f2dc.c
 *   [11] 0x0212f33c  func_ov084_0212f33c.cpp
 *   [12] 0x0212f460  func_ov084_0212f460.cpp
 *   [13] 0x0212f588  func_ov084_0212f588.cpp
 *   [14] 0x0212f630  func_ov084_0212f630.cpp
 *   [15] 0x0212f6d8  func_ov084_0212f6d8.c
 *   [16] 0x0212fa7c  func_ov084_0212fa7c.c
 *   [17] 0x0212fc10  func_ov084_0212fc10.c
 *   [18] 0x0212fc84  _ZN7daPkn_c16CleanupResourcesEv.cpp
 *   [19] 0x0212fcd8  _ZN7daPkn_c16OnPendingDestroyEv.cpp
 *   [20] 0x0212fcdc  _ZN7daPkn_c6RenderEv.cpp
 *   [21] 0x0212fd4c  _ZN7daPkn_c8BehaviorEv.cpp
 *   [22] 0x0212feb4  _ZN7daPkn_c13InitResourcesEv.cpp
 *   [23] 0x02130110  d_a_pkn.c                            (daPkn_c_classInit)
 * ======================================================================== */

#include "daPkn_c.h"
#include "types.h"
#include "common.h"
#include "decl_Animation.h"
#include "SharedFilePtr.h"

/* The six shared files this actor claims.  include/SharedFilePtr.h declares no
   fields on purpose -- the files that shadow it disagree about the layout -- so
   this TU takes its own two-word view and casts to SharedFilePtr wherever it
   calls a method.  Every member below that reads a loaded handle reads it out
   of `.file`, which is the second word, so one spelling serves all of them. */
struct PknSharedFile { int id; void *file; };
extern PknSharedFile data_ov084_02130dfc;   /* the plant's own model */
extern PknSharedFile data_ov002_0210da38;   /* the pipe, shared out of ov002 */
extern PknSharedFile data_ov084_02130df4;   /* idle animation */
extern PknSharedFile data_ov084_02130e0c;   /* recoil animation */
extern PknSharedFile data_ov084_02130e14;   /* lunge animation */
extern PknSharedFile data_ov084_02130e24;   /* death animation */

/* Six animation handles, held indirectly: the table is pointers, not objects. */
extern SharedFilePtr *data_ov084_021302f4[];

/* func_ov084_0212ec60's own view of the shared scratch matrix and of the
   Vector3 fields it walks.  The tags are uniquified because the file scope this
   body now shares with the real headers already has a Vector3 and a Matrix4x3,
   and the byte match is in this spelling. */
struct PknVec3 { int x, y, z; };
struct PknMtx43 { int w[12]; };

/* daPkn_c::Behavior calls the current state through a pointer-to-member held in
   .bss at 0x02130e80.  Nothing here names a symbol; the class exists only to
   give the member pointer a type. */
struct PknStateCls { virtual void dummy(); };
typedef void (PknStateCls::*PknStatePMF)();

#define AT(p, off) ((void *)(int)((char *)(p) + (off)))

/* --------------------------------------------------------------------------
 * The one file-scope extern "C" region.  Everything here is reached from a
 * C++-named member, which cannot declare it in its own body.
 * ------------------------------------------------------------------------ */
extern "C" {

/* -- this TU's own members, forward-declared because mwcc lays .text down in
      reverse source order and every one of these calls is a forward reference. */
void  func_ov084_0212ebb4(void *c);
void  func_ov084_0212ec60(char *c);
int   func_ov084_0212ef00(char *self);
int   func_ov084_0212f1d0(char *c);
void  func_ov084_0212f204(char *r4);
void  func_ov084_0212f33c(void *self);
void  func_ov084_0212f460(void *self);

/* -- the state table __sinit_ov084_02130654 fills in -- */
extern PknStatePMF data_ov084_02130e80[];

/* -- other modules -- */
void  LoadBlueCoinModel(void *c);
void  UnloadBlueCoinModel(void *c);
int   func_0201267c(int a, void *b);
void  func_02012694(u32 id, void *pos);
void  func_020105cc(void *thiz, u32 flags);

void *_ZN8dActor_c10FindWithIDEj(u32 id);
char *_ZN8dActor_c13ClosestPlayerEv(void);
int   _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void *self, void *clsn, void *player);
void  _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void *self, void *clsn);
void  _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const Vector3 *c, const void *d, int e, int f);

void  _ZN6Player16IncMegaKillCountEv(void *player);
void  _ZN6Player6BounceE5Fix12IiE(void *player, int fix);
int   _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *player, const void *pos, u32 a, int fix, u32 b, u32 c, u32 d);

int   _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(void *self, void *clsn, void *anim, u32 flags);
void  _ZN5dCc_c5ClearEv(void *thiz);
void  _ZN5dCc_c6UpdateEv(void *thiz);
void  _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *thiz, void *pos);

int   _ZN9Animation8FinishedEv(void *a);

/* SetAnim and the three Init calls keep mangled spellings with SCALAR slots:
   they carry Fix12<int> BY VALUE, which mwccarm passes differently at the call
   site, so spelling the true types breaks the byte match. */
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *anim, int flags, int speed, u32 startFrame);
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *a, int r, int h, u32 e, u32 g);
void  _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *a, const Vector3 *v, int r, int h, u32 e, u32 g);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *a, int b, int c, void *d, void *e);

/* -- shared tables -- */
extern s16 data_02082214[];              /* the sin/cos table, two shorts a step */
extern int data_ov084_0213030c[];        /* per-frame horizontal fire offset */
extern int data_ov084_02130334[];        /* per-frame vertical fire offset */
extern u8  data_ov084_021302ec[];        /* the five bones the head angle sums */
extern PknMtx43 data_020a0e68;           /* the shared scratch matrix */

void  _Z14ApproachLinearRsss(s16 *val, s16 target, s16 step);
int   Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
void  Vec3_Asr(PknVec3 *d, PknVec3 *s, int sh);
void  Vec3_LslInPlace(PknVec3 *v, int sh);
void  SubVec3(PknVec3 *a, PknVec3 *b, PknVec3 *c);
void  Matrix4x3_FromTranslation(PknMtx43 *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationY(PknMtx43 *m, s16 ang);
void  MulMat4x3Mat4x3(void *a, void *b, void *c);

void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 handle, u32 id, int x, int y, int z, const void *f, void *cb);
void  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
int   _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, int t, int flag);
int   _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 b, u32 c, void *pos, u32 d);

}

/* ==========================================================================
 * ROM ordinal 23 -- daPkn_c_classInit, 0x02130110, size 0x64.
 *
 * NOT counted by the promotion queue's shard_count, which segments on symbol
 * NAME and does not recognise `daPkn_c_classInit` as a member of this class
 * (issue #2436).  It is one: it is the only code that builds a daPkn_c, its
 * allocation size 0x47c IS this class's sizeof, and it stores this class's
 * vptr.
 *
 * The store is `_ZTV7daPkn_c + 2`.  config/arm9/overlays/ov084/symbols.txt binds
 * _ZTV7daPkn_c to the public ADDRESS POINT at 0x02130c28; mwcc's own emitted
 * symbol addresses the vtable OBJECT two words lower, and this TU emits the
 * vtable, so the +2 (int-indexed, eight bytes) is what agrees with the addend-8
 * vptr stores the compiler puts in the destructor.
 * ======================================================================== */

extern "C" {

// @symbol daPkn_c_classInit
int *daPkn_c_classInit(void)
{
    extern void *_ZN7fBase_cnwEj(unsigned int size);
    extern void  _ZN12dEnemyBase_cC2Ev(void *thiz);
    extern void  _ZN9ModelAnimC1Ev(char *thiz);
    extern void  _ZN5ModelC1Ev(char *thiz);
    extern void  _ZN10dBgCh_ActrC1Ev(char *thiz);
    extern void  _ZN7dCcAc_cC1Ev(char *thiz);
    extern void  _ZN10dCcAcPos_cC1Ev(char *thiz);

    int *p = (int *)_ZN7fBase_cnwEj(1148);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)(_ZTV7daPkn_c + 2);
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN5ModelC1Ev((char *)p + 0x174);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1c4);
        _ZN7dCcAc_cC1Ev((char *)p + 0x380);
        _ZN7dCcAc_cC1Ev((char *)p + 0x3b4);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x3e8);
    }
    return p;
}

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN7daPkn_c13InitResourcesEv, 0x0212feb4, size 0x25c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * This was still `extern "C" int _ZN7daPkn_c13InitResourcesEv(char* c)`
 * working raw offsets -- a file renamed .cpp without ever being migrated. It is
 * a real method now and every offset is a field.
 *
 * Unlike FirePiranhaPlantBig's, this one DOES check its loads: a failed SetFile
 * on either model returns 0 rather than carrying on.
 *
 * The tail computes where the plant's fire comes from: 0xe0 along the facing
 * angle out of the shared sin/cos table at data_02082214, and 0x37800 above the
 * spawn position.
 *
 * SetAnim and the three Init calls keep extern "C" declarations with scalar
 * slots: they carry Fix12<int> BY VALUE, which mwccarm passes differently at
 * the call site, so spelling the true types breaks the byte match -- see
 * notes/mwccarm-codegen.md 6az.
 */
int daPkn_c::InitResources()
{
    int i;
    Vector3 v;
    for (i = 0; i < 6; i++)
        Animation::LoadFile(*data_ov084_021302f4[i]);
    LoadBlueCoinModel(this);
    Model::LoadFile(*(SharedFilePtr *)&data_ov084_02130dfc);
    Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210da38);
    if (mModelAnim.SetFile((BMD_File *)data_ov084_02130dfc.file, 1, -1) == 0)
        return 0;
    if (mModel.SetFile((BMD_File *)data_ov002_0210da38.file, 1, -1) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c1, this, 0x32000, 0x64000, 0x200004, 0x423e0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c2, this, 0x82000, 0x64000, 2, 0x423e0);
    v.x = 0; v.y = 0; v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0x46000, 0x64000, 0x200002, 0);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mState = 0;
    mInitAngleY = mPrevAngleY;
    unk_464 = 0x7fffffff;
    unk_460 = 0;
    mClsnEnabled = 0;
    unk_45d = 1;
    mStateTimer = 0;
    unk_108 = 3;
    unk_46c = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
    mSpawnPos.x = mPosX;
    mSpawnPos.y = mPosY;
    mSpawnPos.z = mPosZ;
    {
        s16 *tbl = data_02082214;
        /* The add sits INSIDE the integer cast, which is load-bearing here:
           not interchangeable with `&mSpawnPos`. */
        Vector3* home = (Vector3*)(((int)this + 0x440));
        mHomePos = *home;
        /* The shift must be LOGICAL so the angle wraps -- on the signed s16 it
           would index the wrong table entry for negative angles. */
        unsigned short angh = *(unsigned short*)&mAngleY;
        int ang = angh >> 4;
        int y0 = mPosY;
        int z0 = mPosZ;
        int cosv = tbl[(ang << 1) + 1];
        int sinv = tbl[ang << 1];
        int z = cosv * 0xe0 + z0;
        int y = y0 + 0x37800;
        int x = sinv * 0xe0 + mPosX;
        mFirePos.x = x;
        mFirePos.y = y;
        mFirePos.z = z;
    }
    unk_474 = 0;
    mParticleHandle = unk_474;
    unk_478 = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130df4.file, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN7daPkn_c8BehaviorEv, 0x0212fd4c, size 0x168 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPkn_c::Behavior()
{
    int r;
    int old;
    int cur;
    r = _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(((char*)this), &mWithMeshClsn, &mModelAnim, 1);
    if (r != 0) {
        if (r == 2) {
            unk_108 = 0;
            mState = 7;
            mScaleX = 0;
            mScaleY = 0;
            mScaleZ = 0;
            mPosX = mHomePos.x;
            mPosY = mHomePos.y;
            mPosZ = mHomePos.z;
        }
        return 1;
    }
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char*)this), &mdCcAc_c1);
    mModelAnim.Advance();
    func_ov084_0212f204(((char*)this));
    old = mState;
    (((PknStateCls *)((char *)this))->*data_ov084_02130e80[old])();
    {
        unsigned short* p100 = (unsigned short*)((char*)&mStateTimer);
        *p100 = (unsigned short)(*p100 + 1);
    }
    cur = mState;
    if (old != cur) {
        if (cur == 5) {
            int* pb0 = (int*)((char*)&mFlags);
            *pb0 = *pb0 & ~0x10000000;
        }
        mStateTimer = 0;
        unk_478 = 0;
    }
    func_ov084_0212ec60(((char*)this));
    _ZN5dCc_c5ClearEv((char*)&mdCcAc_c1);
    _ZN5dCc_c5ClearEv((char*)&mdCcAc_c2);
    _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
    if (mClsnEnabled != 0) {
        _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c1);
        _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c2);
        if (mState == 2) {
            _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(&mdCcAcPos_c, &mSpawnPos);
            _ZN5dCc_c6UpdateEv((char*)&mdCcAcPos_c);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN7daPkn_c6RenderEv, 0x0212fcdc, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * The six-slot `struct Obj` this file used to cast both models to was their own
 * vtable, and the slot it called is Render -- ModelAnim's for the plant, Model's
 * for the pipe it sits in.
 *
 * `mModelAnim.file` is a BCA_File * where the raw field was an int, so the
 * comparison against the second word of data_ov084_02130df4 needs the cast the
 * offset spelling did not.
 */
int daPkn_c::Render()
{
    if (mScaleX == 0)
        return 1;
    mModelAnim.Render((Vector3 *)&mScaleX);
    if ((int)mModelAnim.file == (int)data_ov084_02130df4.file)
        mModel.Render(&mPipeScale);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN7daPkn_c16OnPendingDestroyEv, 0x0212fcd8, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daPkn_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN7daPkn_c16CleanupResourcesEv, 0x0212fc84, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
int daPkn_c::CleanupResources()
{
  int i;
  ((SharedFilePtr *)&data_ov084_02130dfc)->Release();
  for(i=0;i<6;i++){
    data_ov084_021302f4[i]->Release();
  }
  UnloadBlueCoinModel(((void*)this));
  ((SharedFilePtr *)&data_ov002_0210da38)->Release();
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov084_0212fc10, 0x0212fc10, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212fc10
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov084_0212fc10(char *c)
{
    *(int *)(c + 0x80) = 0x1000;
    *(int *)(c + 0x84) = 0x1000;
    *(int *)(c + 0x88) = 0x1000;
    *(unsigned char *)(c + 0x45c) = 1;
    *(int *)(c + 0xb0) |= 0x10000000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130df4.file, 0, 0x1000, 0);
    if (*(int *)(c + 0x464) < 0x4b0000)
        *(int *)(c + 0x458) = 1;
    func_ov084_0212ef00(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov084_0212fa7c, 0x0212fa7c, size 0x194 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212fa7c
// recovered name: FirePiranhaPlantBig_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
/* recovered: renamed to Class_Method */
/* daFPkn_c::Kill - recovered from vtable slot identity */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov084_0212fa7c(char *c) {
    *(unsigned char *)(c + 0x45c) = 1;
    if (_ZN9Animation8FinishedEv(c + 0x160) || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130df4.file, 0, 0x1000, 0);
    }
    {
        int thr = (*(int *)(c + 0x46c) != 0) ? 0x12c000 : 0x190000;
        if (*(int *)(c + 0x464) < thr && func_ov084_0212f1d0(c)) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130e0c.file, 0x40000000, 0x1000, 0);
            *(int *)(c + 0x458) = 3;
            func_02012694(0x175, c + 0x74);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xfe, *(int *)(c + 0x434), *(int *)(c + 0x438), *(int *)(c + 0x43c));
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xff, *(int *)(c + 0x434), *(int *)(c + 0x438), *(int *)(c + 0x43c));
            return;
        }
    }
    *(int *)(c + 0x478) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(int *)(c + 0x478), 3, 0x174, c + 0x74, 0);
    if (*(int *)(c + 0x464) < 0x352000) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0, 0x7f, 0x1451, 0);
        *(unsigned char *)(c + 0x45d) = 0;
    } else if (*(unsigned char *)(c + 0x45d) == 0) {
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0x1451, 0))
            *(unsigned char *)(c + 0x45d) = 1;
    }
    func_ov084_0212ef00(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov084_0212f6d8, 0x0212f6d8, size 0x3a4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f6d8
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov084_0212f6d8(char *c)
{
    void *actor;
    u16 type;
    int isPlayer;
    int isNine;
    u32 flags;
    struct Vector3 pos1;
    struct Vector3 pos2;

    *(u8 *)(c + 0x45c) = 1;

    if (_ZNK9Animation12WillHitFrameEi(c + 0x160, 0x10)
        || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x20)
        || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x34)
        || _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x4b)) {
        func_0201267c(0xc0, c + 0x74);
    }

    _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x468), 0x800);
    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);

    if (_ZN9Animation8FinishedEv(c + 0x160) != 0) {
        int thr;
        if (*(int *)(c + 0x46c) != 0)
            thr = 0x12c000;
        else
            thr = 0x190000;
        thr = thr + 0x64000;
        if (*(int *)(c + 0x464) > thr)
            *(int *)(c + 0x458) = 4;
        else
            *(int *)(c + 0x168) = 0;
    }

    /* cylinder 0 (id @ +0x3a4, flags @ +0x3a0) */
    if (*(u32 *)(c + 0x3a4) != 0) {
        actor = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x3a4));
        if (actor != 0) {
            type = *(u16 *)((char *)actor + 0xc);
            isPlayer = (int)(type == 0xbf);
            if (isPlayer != 0) {
                flags = *(u32 *)(c + 0x3a0);
                if (flags & 0x10) {
                    _ZN6Player16IncMegaKillCountEv(actor);
                    func_02012694(0x1d, c + 0x74);
                    func_ov084_0212ebb4(c);
                    return;
                }
                if (*(u8 *)((char *)actor + 0x6f9) != 0) {
                    func_ov084_0212ebb4(c);
                    return;
                }
                if (*(u8 *)((char *)actor + 0x6fb) == 0) {
                    pos1.x = *(int *)(c + 0x5c);
                    pos1.y = *(int *)(c + 0x60);
                    pos1.z = *(int *)(c + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos1, 3, 0xc000, 1, 0, 1);
                    return;
                }
                if ((flags & 0x40000) == 0)
                    return;
                func_ov084_0212ebb4(c);
                return;
            }
            if ((*(u32 *)(c + 0x3a0) & 0x2000) != 0) {
                isNine = (int)(type == 9);
                if (isNine != 0) {
                    func_ov084_0212ebb4(c);
                    return;
                }
            }
        }
    }

    /* cylinder 1 (id @ +0x3d8, flags @ +0x3d4; mega bit read from +0x3a0 per ROM) */
    if (*(u32 *)(c + 0x3d8) != 0) {
        actor = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x3d8));
        if (actor != 0) {
            type = *(u16 *)((char *)actor + 0xc);
            isPlayer = (int)(type == 0xbf);
            if (isPlayer != 0) {
                if ((*(u32 *)(c + 0x3a0) & 0x10) != 0) {
                    _ZN6Player16IncMegaKillCountEv(actor);
                    func_02012694(0x1d, c + 0x74);
                    func_ov084_0212ebb4(c);
                    return;
                }
                if (*(u8 *)((char *)actor + 0x6f9) != 0) {
                    func_ov084_0212ebb4(c);
                    return;
                }
                if ((*(u32 *)(c + 0x3d4) & 0x40000) == 0)
                    return;
                func_ov084_0212ebb4(c);
                return;
            }
            if ((*(u32 *)(c + 0x3d4) & 0x2000) != 0) {
                isNine = (int)(type == 9);
                if (isNine != 0) {
                    func_ov084_0212ebb4(c);
                    return;
                }
            }
        }
    }

    /* cylinder 2 (id @ +0x40c, flags @ +0x408) — player only */
    if (*(u32 *)(c + 0x40c) == 0)
        return;
    actor = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x40c));
    if (actor == 0)
        return;
    type = *(u16 *)((char *)actor + 0xc);
    isPlayer = (int)(type == 0xbf);
    if (isPlayer == 0)
        return;
    if ((*(u32 *)(c + 0x408) & 0x10) != 0) {
        _ZN6Player16IncMegaKillCountEv(actor);
        func_02012694(0x1d, c + 0x74);
        func_ov084_0212ebb4(c);
        return;
    }
    if (*(u8 *)((char *)actor + 0x6f9) != 0)
        return;
    pos2.x = *(int *)(c + 0x5c);
    pos2.y = *(int *)(c + 0x60);
    pos2.z = *(int *)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos2, 3, 0xc000, 1, 0, 1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov084_0212f630, 0x0212f630, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f630
extern "C" void func_ov084_0212f630(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0xcb33, 0);
    if (func_ov084_0212ef00((char *)c) != 0) return;
    if (*(unsigned short *)(c + 0x100) <= 0xb) return;
    if (!_ZN9Animation8FinishedEv(c + 0x160)) return;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130e14.file, 0x40000000, 0x1000, 0);
    *(int *)(c + 0x458) = 2;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov084_0212f588, 0x0212f588, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f588
extern "C" void func_ov084_0212f588(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    *(unsigned char *)(c + 0x45c) = 1;
    if (_ZN9Animation8FinishedEv(c + 0x160)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130e0c.file, 0x40000000, 0x1000, 0);
        *(int *)(c + 0x458) = 1;
    } else {
        int thresh = (*(int *)(c + 0x46c) != 0) ? 0x12c000 : 0x190000;
        if (*(int *)(c + 0x464) < thresh) {
            if (func_ov084_0212f1d0((char *)c) != 0) {
                *(int *)(c + 0x168) = 0;
                *(int *)(c + 0x458) = 2;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov084_0212f460, 0x0212f460, size 0x128 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f460
void func_ov084_0212f460(void *self)
{
    char *c = (char *)self;
    int space[3];
    int zero;

    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0xcb33, 0);
    /* `fp` is assigned BEFORE `zero`, and the test reads through it. Under 2004/b56
       the callee-saved registers are handed out in assignment order, so this is what
       puts fp in r7 and zero in r6 as the ROM has them; with `zero = 0` first they come
       out swapped and nothing else moves. The ROM emits `mov r6,#0; strb` before
       `add r7,r4,#0x100` regardless -- the store is independent, so it schedules ahead
       of the address computation that feeds the compare. */
    u16 *fp = (u16 *)(c + 0x100);
    zero = 0;
    *(unsigned char *)(c + 0x45c) = zero;
    if (*fp < 0xa) {
        int x = *(int *)(c + 0x5c);
        s16 *sintbl = data_02082214;
        *(volatile int *)&space[0] = x;
        int y = *(int *)(c + 0x60);
        int *xtbl = data_ov084_0213030c;
        *(volatile int *)&space[1] = y;
        int z = *(int *)(c + 0x64);
        int *ytbl = data_ov084_02130334;
        *(volatile int *)&space[2] = z;

        x = xtbl[*fp] * sintbl[(*(u16 *)(c + 0x8e) >> 4) * 2] + x;
        *(volatile int *)&space[0] = x;
        z = xtbl[*fp] * sintbl[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1] + z;
        *(volatile int *)&space[2] = z;
        y = y + (ytbl[*fp] << 12);
        *(volatile int *)&space[1] = y;

        u32 slot = *(u32 *)(c + 0x470);
        int px = space[0];
        int py = space[1];
        *(void **)(c + 0x470) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            slot, 0xfb, px, py, z, 0, 0);
    }
    if (_ZN9Animation8FinishedEv(c + 0x160) == 0)
        return;
    *(int *)(c + 0x458) = 6;
    *(int *)(c + 0x474) = 0;
    *(void **)(c + 0x470) = *(void **)(c + 0x474);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov084_0212f33c, 0x0212f33c, size 0x124 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f33c
/* recovered: shared common types */
void func_ov084_0212f33c(void *self)
{
    char *c = (char*)self;
    int r4;
    struct Vector3 v;

    *(unsigned char*)(c + 0x45c) = 0;
    r4 = *(int*)(c + 0x80);
    if (*(unsigned short*)(c + 0x100) == 0) {
        func_02012694(0x11f, c + 0x74);
        r4 = 0x1000;
    }

    *(void**)(c + 0x470) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(unsigned int*)(c + 0x470), 0xfc, *(int*)(c + 0x5c),
        *(int*)(c + 0x60) + 0x1e000, *(int*)(c + 0x64), 0, 0);
    *(void**)(c + 0x474) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(unsigned int*)(c + 0x474), 0xfd, *(int*)(c + 0x5c),
        *(int*)(c + 0x60) + 0x1e000, *(int*)(c + 0x64), 0, 0);

    if (r4 > 0) {
        r4 = r4 - 0xa3;
    } else {
        r4 = 0;
        if (*(unsigned char*)(c + 0x108) != 0) {
            int y = *(int*)(c + 0x60);
            int z = *(int*)(c + 0x64);
            v = (struct Vector3){*(int*)(c + 0x5c), y + 0x78000, z};
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x122, r4, &v, (void*)r4, *(signed char*)(c + 0xcc), -1);
            *(unsigned char*)(c + 0x108) = r4;
        }
        *(int*)(c + 0x458) = 7;
    }
    *(int*)(c + 0x80) = r4;
    *(int*)(c + 0x84) = r4;
    *(int*)(c + 0x88) = r4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov084_0212f2dc, 0x0212f2dc, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f2dc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov084_0212f2dc(char *c){
    *(unsigned char*)(c + 0x45c) = 0;
    if(*(int*)(c + 0x464) <= 0x4b0000) return;
    *(int*)(c + 0x458) = 8;
    *(int*)(c + 0x474) = 0;
    *(int*)(c + 0x470) = *(int*)(c + 0x474);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130e0c.file, 0, 0x1000, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov084_0212f298, 0x0212f298, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f298
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov084_0212f298(char *c)
{
    int v;
    *(unsigned char *)(c + 0x45c) = 0;
    v = *(int *)(c + 0x80);
    if (*(unsigned short *)(c + 0x100) == 0)
        v = 0x4cc;
    if (v < 0x1000)
    {
        v += 0x52;
    }
    else
    {
        *(int *)(c + 0x458) = 0;
        v = 0x1000;
    }
    *(int *)(c + 0x80) = v;
    *(int *)(c + 0x84) = v;
    *(int *)(c + 0x88) = v;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov084_0212f204, 0x0212f204, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f204
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov084_0212f204(char* r4){
  struct Vector3 v;
  *(char**)(r4 + 0x460) = _ZN8dActor_c13ClosestPlayerEv();
  {
    char* p = *(char**)(r4 + 0x460);
    if (p != 0) {
      struct Vector3* pp = (struct Vector3*)(((int)p + 0x5c));
      v.x = pp->x;
      v.y = pp->y;
      v.z = pp->z;
      *(int*)(r4 + 0x464) = Vec3_Dist((struct Vector3*)(r4 + 0x5c), &v);
      *(short*)(r4 + 0x468) = Vec3_HorzAngle((struct Vector3*)(r4 + 0x5c), &v);
      *(int*)(r4 + 0x46c) = *(unsigned char*)(*(char**)(r4 + 0x460) + 0x6de);
    } else {
      *(int*)(r4 + 0x464) = 0x7fffffff;
      *(short*)(r4 + 0x468) = *(short*)(r4 + 0x8e);
    }
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov084_0212f1d0, 0x0212f1d0, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212f1d0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov084_0212f1d0(char *c) {
    char *p = *(char**)(c + 0x460);
    if (p == 0) return 0;
    if (*(int*)(p + 0xa8) > 0xa000) return 1;
    return *(int*)(p + 0x98) > 0xa000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov084_0212ef00, 0x0212ef00, size 0x2d0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ef00
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov084_0212ef00(char* self)
{
    void *actor;
    u32 flags;
    u32 raw;
    u16 type;
    u32 id;

    id = *(u32*)(self + 0x3a4);
    if (id == 0) goto second;

    actor = _ZN8dActor_c10FindWithIDEj(id);
    if (actor == 0) goto second;

    type = *(u16*)((char*)actor + 0xc);
    {
        int t = (int)(type == 0xbf);
        if (t == 0) goto checkA2;
    }

    raw = *(u32*)(self + 0x3a0);
    flags = raw & 0x26ff0;
    if (flags == 0) goto jumpA;

    if (flags & 0x10) {
        _ZN6Player16IncMegaKillCountEv(actor);
        func_02012694(0x1d, self + 0x74);
    }
    func_020105cc(self, flags);
    func_ov084_0212ebb4(self);
    goto successA;

jumpA:
    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(self, self + 0x380, actor) != 0) {
        _ZN6Player6BounceE5Fix12IiE(actor, 0x28000);
        func_ov084_0212ebb4(self);
        goto successA;
    }

    if (*(u32*)(self + 0x3a0) & 0x40000) {
        func_ov084_0212ebb4(self);
        goto successA;
    }

    {
        int v[3];
        v[0] = *(int*)(self + 0x5c);
        v[1] = *(int*)(self + 0x60);
        v[2] = *(int*)(self + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, v, 3, 0xc000, 1, 0, 1);
    }

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x110, data_ov084_02130e0c.file, 0, 0x1000, 0);
    _ZN9Animation8SetFlagsEi(self + 0x160, 0x40000000);
    *(int*)(self + 0x458) = 3;
    func_02012694(0x175, self + 0x74);
    return 0;

successA:
    return 1;

checkA2:
    if (!(*(u32*)(self + 0x3a0) & 0x2000)) goto second;
    {
        int u = (int)(type == 9);
        if (u == 0) goto second;
    }
    func_ov084_0212ebb4(self);
    return 1;

second:
    id = *(u32*)(self + 0x3d8);
    if (id == 0) goto fail;

    actor = _ZN8dActor_c10FindWithIDEj(id);
    if (actor == 0) goto fail;

    type = *(u16*)((char*)actor + 0xc);
    {
        int t = (int)(type == 0xbf);
        if (t == 0) goto checkB2;
    }

    raw = *(u32*)(self + 0x3d4);
    flags = raw & 0x26ff0;
    if (flags == 0) goto jumpB;

    if (flags & 0x10) {
        _ZN6Player16IncMegaKillCountEv(actor);
        func_02012694(0x1d, self + 0x74);
    }
    func_020105cc(self, flags);
    func_ov084_0212ebb4(self);
    return 1;

jumpB:
    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(self, self + 0x380, actor) != 0) {
        _ZN6Player6BounceE5Fix12IiE(actor, 0x28000);
        func_ov084_0212ebb4(self);
        return 1;
    }

    if (*(u32*)(self + 0x3d4) & 0x40000) {
        func_ov084_0212ebb4(self);
        return 1;
    }

    goto fail;

checkB2:
    if (!(*(u32*)(self + 0x3d4) & 0x2000)) goto fail;
    {
        int u = (int)(type == 9);
        if (u == 0) goto fail;
    }
    func_ov084_0212ebb4(self);
    return 1;

fail:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov084_0212ec60, 0x0212ec60, size 0x2a0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ec60
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov084_0212ec60(char* c)
{
    volatile s16 ang[3];
    struct { PknMtx43 saved; PknVec3 tv; PknVec3 v; } L;
    int lr;
    int r3;

    Vec3_Asr(&L.v, (PknVec3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, L.v.x, L.v.y, L.v.z);
    L.saved = data_020a0e68;
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
    *(PknMtx43*)(c + 0x12c) = data_020a0e68;

    ang[0] = 0;
    ang[1] = 0;
    ang[2] = 0;
    for (lr = 0; lr < 5; lr++) {
        ang[1] = (s16)(ang[1] + *(s16*)(*(char**)(c + 0x120) + data_ov084_021302ec[lr] * 0x34 + 0x1c));
    }

    *(s32*)(c + 0x440) = 0;
    *(s32*)(c + 0x444) = 0;
    *(s32*)(c + 0x448) = 0;
    data_020a0e68 = L.saved;
    MulMat4x3Mat4x3((char*)*(void**)(c + 0x124) + 0x120, &data_020a0e68, &data_020a0e68);
    *(s32*)(c + 0x440) = data_020a0e68.w[9];
    *(s32*)(c + 0x444) = data_020a0e68.w[10];
    *(s32*)(c + 0x448) = data_020a0e68.w[11];
    Vec3_LslInPlace((PknVec3*)(c + 0x440), 3);
    SubVec3((PknVec3*)(c + 0x440), (PknVec3*)(c + 0x5c), (PknVec3*)(c + 0x440));

    *(s32*)AT(c, 0x444) -= data_02082214[((u16)ang[0] >> 4) << 1] * (s16)0x14 + 0x32000;
    *(s32*)AT(c, 0x448) += 0x32000;

    L.tv.x = *(s32*)(c + 0x5c);
    L.tv.y = *(s32*)(c + 0x60);
    L.tv.z = *(s32*)(c + 0x64);

    {
        int rr = (int)((u32)(*(s32*)(c + 0x168) << 4) >> 0x10);
        int d = rr - 0x28;
        if (d < 0)
            d = -d;
        r3 = ((d << 12) / 10) + 0x400;
        if (r3 < 0x800)
            r3 = 0x800;
        *(s32*)(c + 0x428) = r3;
        *(s32*)(c + 0x42c) = r3;
        *(s32*)(c + 0x430) = r3;
    }

    L.tv.x = data_02082214[(*(u16*)(c + 0x8e) >> 4) << 1] * (s16)0xe0 + L.tv.x;
    r3 = r3 - 0xc00;
    L.tv.z = data_02082214[((*(u16*)(c + 0x8e) >> 4) << 1) + 1] * (s16)0xe0 + L.tv.z;
    L.tv.y = L.tv.y + (r3 * 0x18 + 0x38000);

    *(s32*)(c + 0x434) = L.tv.x;
    *(s32*)(c + 0x438) = L.tv.y;
    *(s32*)(c + 0x43c) = L.tv.z;

    Matrix4x3_FromTranslation((PknMtx43*)(c + 0x190), L.tv.x >> 3, L.tv.y >> 3, L.tv.z >> 3);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN7daPkn_c16OnAimedAtWithEggEv, 0x0212ec58, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daPkn_c16OnAimedAtWithEggEv
// recovered name: PiranhaPlant_OnAimedAtWithEgg
/* recovered: renamed to Class_Method */
/* daPkn_c::OnAimedAtWithEgg - recovered from vtable slot identity */
s32 daPkn_c::OnAimedAtWithEgg() {
    return 286720;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov084_0212ec04, 0x0212ec04, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ec04
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov084_0212ec04(char* c, int arg) {
    *(int*)(c + 0x458) = 2;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130e14.file, 0x40000000, 0x1000, 0);
    *(unsigned int*)(c + 0x168) = ((unsigned int)(arg << 0x10)) >> 4;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov084_0212ebb4, 0x0212ebb4, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov084_0212ebb4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov084_0212ebb4(void *c)
{
    func_0201267c(0xc1, (char*)c + 0x74);
    *(int*)((char*)c + 0x458) = 5;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x110, data_ov084_02130e24.file, 0x40000000, 0x1000, 0);
}
}
