//cpp
/* Lethal Lava Land coin puzzle, ov064 0x02118b50..0x02119330.
 *
 * Two classes, one translation unit: their methods are interleaved in the
 * ROM, so one file has to emit both. daObjFl_Coin_c is the manager
 * (FL_COIN, 0xd8). daObjFl_Puzzle_c is a piece (FL_PUZZLE, 0x33c).
 * Typeinfo: _ZTI14daObjFl_Coin_c at 0x0211bf98, _ZTI16daObjFl_Puzzle_c at
 * 0x0211bfa4. The run ends with the piece's touch helper
 * func_ov064_0211929c, its collision callback func_ov064_021192bc and the
 * two registry factories; both g_profile rows live outside this file.
 *
 * Source order is the reverse of the ROM. mwccarm 2004/b56 emits one .text
 * section per function in reverse source order, so the highest address is
 * written first. Do not reorder. The destructor pairs are the exception:
 * each class's destructor is inline, and the two forcing calls at the
 * bottom of this file pull D1 out before D0, which is the cartridge order.
 *
 * How the puzzle runs. A piece starts in PIECE_STATE_INIT, which binds it to
 * the FL_COIN manager (actor 0x4f) and then hands it to PIECE_STATE_WAIT.
 * A step ends on the tick where mStepTimer reads 0x18 (24); the timer only
 * counts while the manager is gone or has no live coins. Then the next byte
 * of the piece's per-type script becomes mState: WAIT, or one of four moves
 * that shake the piece for 20 ticks (0x14) and then slide it 120 units per
 * tick along -X, +X, -Z or +Z for 4 ticks. A script ends in -1; a piece taking the last entry
 * (the byte after it is -1) rewinds the script and ORs
 * FLCOIN_FLAG_SCRIPT_WRAPPED into the manager's flags. A piece the player has
 * touched (mHadClsn, set by the collision callback func_ov064_0211929c on
 * actor 0xbf, PLAYER) keeps writing FLCOIN_FLAG_PLAYER_TOUCHED over them. The
 * manager moves to phase 1 on a frame where the flags are exactly both bits and
 * the player is within 1000 units, and then each WAIT-state piece spawns one
 * coin (actor 0x120) bound back to the manager.
 *
 * Leftover: func_ov064_02118da0 keeps its `(int)this + 0x336` byte increment
 * and its Vec-typed read of the manager's position; spelling either as a plain
 * member changes the bytes. Behavior's IsClsnInRange call keeps its mangled
 * free-function form (Fix12 by value). Model::LoadFile and
 * dBgW_KcMbg::SetFile stay ABI-exact free declarations; SetFile's by-value
 * Fix12<int> grows the call when spelled as the real method. unk_32c is only
 * zeroed in this file, the two callback helpers keep their address names
 * and stay free extern "C" functions, and the sound played at the end of a shake (bank-3 id 0xe7) is unnamed. The
 * two daWater_Hakidasi_c slots past this span are not piece methods.
 */
#include "daObjFl_Puzzle_c.h"
#include "daObjFl_Coin_c.h"
#include "common.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "daCoin_c.h"

/* State table at 0x0211c904. Each entry is a pointer-to-member that Behavior
 * calls. The targets are the piece methods below. */
typedef void (daObjFl_Puzzle_c::*PMF)();
struct Entry { PMF pmf; };

/* Actor IDs (symbols/actor_debug_names.tsv). */
enum {
    ACTOR_ID_FL_COIN = 0x4f,   /* the manager, daObjFl_Coin_c */
    ACTOR_ID_COIN = 0x120      /* daCoin_c */
};

/* InitResources' legacy file completed CLPS_Block as one word. No header
 * defines the body; the call passes it by reference. */
struct CLPS_Block { int x; };

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the models, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct FlCoinModelFilePtr : SharedFilePtr {
    u32 words[2];
    FlCoinModelFilePtr(u32 fileID);
    ~FlCoinModelFilePtr();
};
struct FlCoinCollisionFilePtr : SharedFilePtr {
    u32 words[2];
    FlCoinCollisionFilePtr(u32 fileID);
    ~FlCoinCollisionFilePtr();
};

extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int n, const struct Vector3 *v);
extern void Matrix4x3_FromTranslation(struct Matrix4x3* m, int x, int y, int z);
extern Entry data_ov064_0211c904[];
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* c, Fix12i a, Fix12i b);
BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, BMD_File*, int, int);
KCL_File* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, KCL_File*, const Matrix4x3&, int fix, short s, CLPS_Block&);
void func_020393c4(int* p, int v);
extern SharedFilePtr *data_ov064_0211adc8[];
extern FlCoinCollisionFilePtr data_ov064_0211c800;
extern CLPS_Block data_ov064_0211baac;
extern Matrix4x3 data_020a0e68;
void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void* self, const Matrix4x3&, s16);
/* Per-type step scripts, indexed by mType (ov064 .data). */
extern int data_ov064_0211c198[];
/* The collision callback piece InitResources installs, and the touch helper
 * it forwards to; both are defined just below. */
void func_ov064_0211929c(daObjFl_Puzzle_c *self, dActor_c *other);
void func_ov064_021192bc(void *collider, daObjFl_Puzzle_c *self, dActor_c *other);
}

// @symbol daObjFl_Puzzle_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjFl_Puzzle_c through RTTI,
 * allocation size, vtable identity, and the FL_PUZZLE registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The implementation was earlier named BowserPuzzlePiece.
 * Historical alias: BowserPuzzlePiece_Spawn.
 *
 * `new daObjFl_Puzzle_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x33c), dBgActor_c's base constructor, then the
 * vptr store. */
extern "C" daObjFl_Puzzle_c *daObjFl_Puzzle_c_classInit(void)
{
    return new daObjFl_Puzzle_c;
}

// @symbol daObjFl_Coin_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjFl_Coin_c through RTTI,
 * allocation size, vtable identity, and the FL_COIN registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The implementation was earlier named BowserPuzzleManager.
 * Historical alias: BowserPuzzleManager_Spawn.
 *
 * `new daObjFl_Coin_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0xd8), dActor_c's base constructor, then the
 * vptr store. */
extern "C" daObjFl_Coin_c *daObjFl_Coin_c_classInit(void)
{
    return new daObjFl_Coin_c;
}

/* The collision callback piece InitResources installs in mMeshCollider's
 * slot. The slot passes three arguments; the touch helper wants the last
 * two. long_calls keeps the pooled absolute tail call. */
#pragma push
#pragma long_calls on
// @symbol func_ov064_021192bc
extern "C" void func_ov064_021192bc(void *collider, daObjFl_Puzzle_c *self, dActor_c *other)
{
    func_ov064_0211929c(self, other);
}
#pragma pop

// @symbol func_ov064_0211929c
/* The player (actor 0xbf) touched the piece: raise mHadClsn. */
extern "C" void func_ov064_0211929c(daObjFl_Puzzle_c *self, dActor_c *other)
{
    u8 isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        self->mHadClsn = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN14daObjFl_Coin_c13InitResourcesEv, 0x02119284, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjFl_Coin_c13InitResourcesEv

/* recovered: renamed to Class_Method, RTTI class fields named */
// recovered name: daObjFl_Coin_c_InitResources
/* recovered: renamed to Class_Method */
/* daObjFl_Coin_c::InitResources - recovered from vtable slot identity */
/* Clears the piece-flag byte, the phase and the live-coin count. */
s32 daObjFl_Coin_c::InitResources() {
    mPhase = 0;
    mPieceFlags = 0;
    mLiveCoinCount = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN16daObjFl_Puzzle_c13InitResourcesEv, 0x021191a8, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* Picks the model and script from param1's low nibble (mType), loads the
 * model and the shared collision file, puts both matrices at the piece's
 * position, registers func_ov064_021192bc with the collider through
 * func_020393c4, and starts in PIECE_STATE_INIT with the step marked running
 * and the coin spawn still allowed. */
int daObjFl_Puzzle_c::InitResources()
{
    mType = param1 & 0xf;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel,
        _ZN5Model8LoadFileER13SharedFilePtr(*data_ov064_0211adc8[mType]), 1, -1);
    func_ov064_02119010();
    func_ov064_02118fa4();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov064_0211c800),
        mClsnMat, 0x1000, mAngleY, data_ov064_0211baac);
    func_020393c4((int*)((char*)&mMeshCollider), (int)&func_ov064_021192bc);
    mStepScript = data_ov064_0211c198[mType];
    mStepIndex = 0;
    unk_32c = 0;
    mStepTimer = 0;
    mHadClsn = 0;
    mStepActive = 1;
    mState = PIECE_STATE_INIT;
    mCanSpawnCoin = 1;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN14daObjFl_Coin_c8BehaviorEv, 0x0211915c, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjFl_Coin_c8BehaviorEv
// recovered name: daObjFl_Coin_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjFl_Coin_c::Behavior - recovered from vtable slot identity */
/* Phase 0 waits for a frame where the flags are exactly both bits and the
 * player is within 1000 units (0x3e8000), then moves to phase 1 and stays there. */
s32 daObjFl_Coin_c::Behavior() {
    switch (mPhase) {
    case 0:
        if (mPieceFlags == (FLCOIN_FLAG_PLAYER_TOUCHED | FLCOIN_FLAG_SCRIPT_WRAPPED)) {
            if (DistToCPlayer() < 0x3e8000) {   /* player within 1000 units */
                mPhase++;
            }
        }
        break;
    case 1:
        break;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN16daObjFl_Puzzle_c8BehaviorEv, 0x021190b0, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c8BehaviorEv
/* Per frame: advance the step script if no step is running, run the current
 * state's handler from the table at 0x0211c904, tick the step timer (when the
 * manager is gone or has no live coins), rebuild the model matrix, and if
 * IsClsnInRange(0, 0) returns nonzero rebuild the collision matrix and update
 * the collider. */
int daObjFl_Puzzle_c::Behavior() {
    func_ov064_02118ee4();
    (this->*data_ov064_0211c904[mState].pmf)();
    daObjFl_Coin_c* mgr = 0;
    unsigned int id = mCoinMgrId;
    if (id != 0)
        mgr = (daObjFl_Coin_c*)dActor_c::FindWithID(id);
    /* The step timer only runs while the manager is gone or has no live coins. */
    if (mgr == 0 || mgr->mLiveCoinCount == 0) {
        u16* ctr = &mStepTimer;
        *ctr = *ctr + 1;
    }
    func_ov064_02119010();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        func_ov064_02118fa4();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN16daObjFl_Puzzle_c6RenderEv, 0x02119088, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c6RenderEv
/* recovered: named members + shared header, real C++ method */
/* Draws the model. */
int daObjFl_Puzzle_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN16daObjFl_Puzzle_c16CleanupResourcesEv, 0x0211904c, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* Disables the collider and releases this type's model file and the shared
 * collision file. */
int daObjFl_Puzzle_c::CleanupResources()
{
    unsigned char idx;
    mMeshCollider.Disable();
    idx = mType;
    data_ov064_0211adc8[idx]->Release();
    data_ov064_0211c800.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov064_02119010, 0x02119010, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02119010Ev
/* Model matrix translation: the position (plus the shake offset on Y), each
 * axis shifted right by 3. */
void daObjFl_Puzzle_c::func_ov064_02119010() {
    int x = mPosX >> 3;
    int y = (mPosY + mShakeOffsetY) >> 3;
    int z = mPosZ >> 3;
    Matrix4x3_FromTranslation(&mModel.mat4x3, x, y, z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov064_02118fa4, 0x02118fa4, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02118fa4Ev
/* Builds a translation at the position (shake offset added to Y) in the scratch
 * matrix at 0x020a0e68, copies it into mClsnMat, and hands it to the moving
 * mesh collider along with the Y angle. */
void daObjFl_Puzzle_c::func_ov064_02118fa4() {
    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY + mShakeOffsetY, mPosZ);
    mClsnMat = data_020a0e68;
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(&mMeshCollider, mClsnMat, mAngleY);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov064_02118ee4, 0x02118ee4, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02118ee4Ev
/* Step script driver. While mHadClsn is set, writes FLCOIN_FLAG_PLAYER_TOUCHED into the
 * manager's flags. If no step is running, takes the next script byte as mState,
 * and when the byte after it is -1 rewinds the script and ORs
 * FLCOIN_FLAG_SCRIPT_WRAPPED into the manager's flags. Then marks a step as
 * running and restarts the step timer. */
void daObjFl_Puzzle_c::func_ov064_02118ee4()
{
  daObjFl_Coin_c *a;
  int p_addr;
  if (mHadClsn) {
    if (mCoinMgrId) {
      a = (daObjFl_Coin_c *)dActor_c::FindWithID(mCoinMgrId);
      if (a)
        a->mPieceFlags = FLCOIN_FLAG_PLAYER_TOUCHED;
    }
  }
  if (mStepActive)
    return;
  p_addr = (u32)&mStepIndex;
  {
    u8 idx = mStepIndex;
    s8 *tab = (s8 *)mStepScript;
    u8 *p = (u8 *)((u64)p_addr);
    int m1 = ~0;
    mState = tab[idx];
    *p = *p + 1;
    if (((s8 *)mStepScript)[mStepIndex] == (s8)m1) {
      mStepIndex = 0;
      if (mCoinMgrId) {
        a = (daObjFl_Coin_c *)dActor_c::FindWithID(mCoinMgrId);
        if (a) {
          u8 *f = &a->mPieceFlags;
          *f |= FLCOIN_FLAG_SCRIPT_WRAPPED;
        }
      }
    }
  }
  mStepActive = 1;
  mStepTimer = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov064_02118e24, 0x02118e24, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02118e24Eiii
/* Shared move handler. a1/a2 are the X/Z distance per tick (fix12) and a3 the
 * number of sliding ticks. Ticks 0..0x13 only shake (Y offset 0 on even ticks,
 * -0x6000 = -6 units on odd). Tick 0x14 clears the offset and plays sound
 * 0xe7 at the camera-space position; the piece slides on ticks 0x14 up to but
 * not including a3 + 0x14, and the first tick at or past that returns to
 * PIECE_STATE_WAIT and ends the step. */
void daObjFl_Puzzle_c::func_ov064_02118e24(int a1, int a2, int a3)
{
    unsigned int st = mStepTimer;

    if (st < 0x14) {
        if (st & 1) {
            mShakeOffsetY = -0x6000;
        } else {
            mShakeOffsetY = 0;
        }
        return;
    }

    if (st == 0x14) {
        mShakeOffsetY = 0;
        _ZN5Sound9PlayBank3EjRK7Vector3(0xe7, (const struct Vector3 *)&mCamSpacePosX);
    }

    if ((int)mStepTimer >= a3 + 0x14) {
        mState = PIECE_STATE_WAIT;
        mStepActive = 0;
        return;
    }

    {
        int *px = &mPosX;
        int *pz = &mPosZ;
        *px += a1;
        *pz += a2;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov064_02118da0, 0x02118da0, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02118da0Ev
/* PIECE_STATE_INIT. Finds the FL_COIN manager; if there is one, remembers its
 * unique ID, sets the manager's Y to this piece's Y (X and Z are read and
 * written back unchanged), ends the step and advances mState to WAIT. With no
 * manager the piece marks itself for destruction. */
typedef struct { int x, y, z; } Vec;
extern "C" {
char* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);
void _ZN7fBase_c18MarkForDestructionEv(void*);
}
void daObjFl_Puzzle_c::func_ov064_02118da0(){
  volatile int pad[4];
  daObjFl_Coin_c* a = (daObjFl_Coin_c*)_ZN8dActor_c15FindWithActorIDEjPS_(ACTOR_ID_FL_COIN, 0);
  (void)&pad;
  if(a!=0){
    Vec* p = (Vec*)&a->mPosX;
    mCoinMgrId = a->uniqueID;
    int z = p->z;
    int x = p->x;
    int y = mPosY;
    a->mPosX = x;
    a->mPosY = y;
    a->mPosZ = z;
    mStepActive = 0;
    *(unsigned char*)(((int)this + 0x336)) = *(unsigned char*)(((int)this + 0x336)) + 1;   /* mState++ */
    return;
  }
  _ZN7fBase_c18MarkForDestructionEv(this);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov064_02118d3c, 0x02118d3c, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02118d3cEv
/* PIECE_STATE_WAIT. Once, when the manager reaches phase 1, spawns this piece's
 * coin. Ends the step once the timer has reached 0x18 (24). */
void daObjFl_Puzzle_c::func_ov064_02118d3c(){
  if (mCanSpawnCoin != 0) {
    unsigned int id = mCoinMgrId;
    if (id != 0) {
      daObjFl_Coin_c *a = (daObjFl_Coin_c *)dActor_c::FindWithID(id);
      if (a != 0 && a->mPhase == 1) {
        mCanSpawnCoin = 0;
        func_ov064_02118c48();
      }
    }
  }
  if (mStepTimer >= 0x18)
    mStepActive = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov064_02118d20, 0x02118d20, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02118d20Ev
/* PIECE_STATE_MOVE_NEG_X: 120 units (0x78000) per tick toward -X, 4 ticks. */
void daObjFl_Puzzle_c::func_ov064_02118d20()
{
    func_ov064_02118e24(-0x78000, 0, 4);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov064_02118d08, 0x02118d08, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02118d08Ev
/* PIECE_STATE_MOVE_POS_X: 120 units per tick toward +X, 4 ticks. */
void daObjFl_Puzzle_c::func_ov064_02118d08() {
    func_ov064_02118e24(0x78000, 0, 4);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov064_02118cec, 0x02118cec, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02118cecEv
/* PIECE_STATE_MOVE_NEG_Z: 120 units per tick toward -Z, 4 ticks. */
void daObjFl_Puzzle_c::func_ov064_02118cec()
{
    func_ov064_02118e24(0, -0x78000, 4);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov064_02118cd4, 0x02118cd4, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02118cd4Ev
/* PIECE_STATE_MOVE_POS_Z: 120 units per tick toward +Z, 4 ticks. */
void daObjFl_Puzzle_c::func_ov064_02118cd4() {
    func_ov064_02118e24(0, 0x78000, 4);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov064_02118c48, 0x02118c48, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_c19func_ov064_02118c48Ev
/* Spawns a COIN (0x120, spawn param 2) at this piece's position with the
 * piece's mAreaId and death-table ID -1. If it spawned and this piece
 * has a manager ID, finds the manager, stores its unique ID in the coin's
 * mPuzzleManagerID and bumps the manager's live-coin count. */
void daObjFl_Puzzle_c::func_ov064_02118c48()
{
    dActor_c* spawned = dActor_c::Spawn(ACTOR_ID_COIN, 2, *(const Vector3*)&mPosX, 0, mAreaId, -1);
    daObjFl_Coin_c* found;
    if (spawned == 0)
        return;
    if (mCoinMgrId == 0)
        return;
    found = (daObjFl_Coin_c*)dActor_c::FindWithID(mCoinMgrId);
    if (found == 0)
        return;
    ((daCoin_c*)spawned)->mPuzzleManagerID = found->uniqueID;
    found->mLiveCoinCount += 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN16daObjFl_Puzzle_cD0Ev, 0x02118b94, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_cD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#ifdef _MSC_VER
/* MSVC needs this flat D0 entry. Call the actual class-body destructor
 * qualified so dispatch is direct, then use the class-specific deallocator.
 * The inline body includes member/base teardown; no separate flat D1 provider
 * is supplied by this branch. The mwccarm definition below is unchanged. */
extern "C" daObjFl_Puzzle_c *_ZN16daObjFl_Puzzle_cD0Ev(daObjFl_Puzzle_c *thiz)
{
    thiz->daObjFl_Puzzle_c::~daObjFl_Puzzle_c();          /* direct member/base teardown */
    daObjFl_Puzzle_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void daObjFl_Puzzle_c_EmitDeletingDestructor(daObjFl_Puzzle_c *piece)
{
    delete piece;
}
#endif

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN16daObjFl_Puzzle_cD1Ev, 0x02118b50, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_Puzzle_cD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. */
void daObjFl_Puzzle_c_EmitDestructor(daObjFl_Puzzle_c *piece)
{
    piece->~daObjFl_Puzzle_c();
}

/* Not called. The coin destructor is inline in the header; these two calls
 * are what ask mwccarm for the out-of-line D1 and D0. */
void daObjFl_Coin_c_EmitDeletingDestructor(daObjFl_Coin_c *coin)
{
    delete coin;
}

void daObjFl_Coin_c_EmitDestructor(daObjFl_Coin_c *coin)
{
    coin->~daObjFl_Coin_c();
}

// @symbol __sinit_daObjFl_Coin_c.cpp
/* The retail initializer constructs the fourteen model handles in file ID
 * order, then the collision handle, then fills the six-entry state table. */
FlCoinModelFilePtr data_ov064_0211c810(0x5ff);
FlCoinModelFilePtr data_ov064_0211c818(0x601);
FlCoinModelFilePtr data_ov064_0211c7e8(0x602);
FlCoinModelFilePtr data_ov064_0211c840(0x603);
FlCoinModelFilePtr data_ov064_0211c820(0x604);
FlCoinModelFilePtr data_ov064_0211c830(0x605);
FlCoinModelFilePtr data_ov064_0211c7e0(0x606);
FlCoinModelFilePtr data_ov064_0211c808(0x607);
FlCoinModelFilePtr data_ov064_0211c7d8(0x608);
FlCoinModelFilePtr data_ov064_0211c7f0(0x609);
FlCoinModelFilePtr data_ov064_0211c828(0x60a);
FlCoinModelFilePtr data_ov064_0211c7f8(0x60b);
FlCoinModelFilePtr data_ov064_0211c838(0x60c);
FlCoinModelFilePtr data_ov064_0211c848(0x60d);
FlCoinCollisionFilePtr data_ov064_0211c800(0x600);

extern "C" {
extern Entry data_ov064_0211bf68;
extern Entry data_ov064_0211bf70;
extern Entry data_ov064_0211bf78;
extern Entry data_ov064_0211bf80;
extern Entry data_ov064_0211bf88;
extern Entry data_ov064_0211bf90;
}
Entry data_ov064_0211c904[6] = {
    data_ov064_0211bf80, data_ov064_0211bf68, data_ov064_0211bf90,
    data_ov064_0211bf70, data_ov064_0211bf88, data_ov064_0211bf78,
};
