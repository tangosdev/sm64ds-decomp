//cpp
/* Big brick blocks: one class, six actor IDs (0xf, 0x10, 0x11, 0x12,
 * 0x13, 0x2e) -- item blocks, the star block, the switch-activated
 * block. Kill is a switchboard on actorID: coins, stars, particles,
 * then destroy, except the 0x13 block which the event bit re-enables.
 * The TU is ov002 0x020b35a0..0x020b415c, nineteen functions; it ends
 * with the five per-profile registry factories, each `new daObjBlockL_c()`.
 *
 * leftovers:
 * - Particle::System::NewSimple, dActor_c::SpawnCoins,
 *   dBgActor_c::IsClsnInRangeOnScreen and dBgW_KcMbg::SetFile stay
 *   mangled extern-C calls: each takes Fix12<int> by value
 *   (notes/mwccarm-codegen.md 6az).
 * - the BigBrickBlockFileRow / ClpsRow resource tables stay file-local:
 *   no shared header owns those 0xc-stride rows yet.
 * - linkedActor + 0x3b0 is an unnamed byte on the mega character
 *   (actor 0x121); no header names it.
 * - func_02039394 / func_020393a4 are small stores into dBgW. This TU
 *   calls them; naming belongs to dBgW.
 * - data_0209f2f8 reads as the level ID and data_0209f220 as a star
 *   flag (same pattern in the daObjBk_Botaosi_c TU); neither has a
 *   shared home.
 * - IsStarCollectedInCurLevel is a shared coined helper; naming
 *   belongs at its definition.
 */

#include "daObjBlockL_c.h"
#include "Player.h"
#include "dActor_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "daObjSwitch_c.h"
#include "Model.h"
#include "Event.h"
#include "daObjBlockItemTag_c.h"

/* File-local resource rows (see the header note above). */
struct BigBrickBlockFileRow {
    SharedFilePtr* file;
    u8 pad[8];
};

/* shadow struct 'ClpsRow' */
struct ClpsRow {
    CLPS_Block *block;
    u8 pad[8];
};

/* POD spelling of Vector3's 0x0c bytes: with a user-declared constructor on
   Vector3, copy-init lowers memberwise; the ROM copy is the block move, so the
   local copy in Render goes through this. */
struct RawVector3 {
    int x, y, z;
};

/* Model handles construct through func_02017acc and destroy through
 * func_02017ab4. Collision handles construct through func_02017b4c and
 * destroy through SharedFilePtr_Destruct_Clsn. The manifest aliases those
 * undefined members onto the ROM symbols. */
struct BlockModelFile : SharedFilePtr {
    u32 words[2];

    BlockModelFile(u32 fileID);
    ~BlockModelFile();
};

struct daObjBlockLClsnFile : SharedFilePtr {
    u32 words[2];

    daObjBlockLClsnFile(u32 fileID);
    ~daObjBlockLClsnFile();
};

extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_( u32 id, Fix12i x, Fix12i y, Fix12i z);
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs( void *self, const Vector3 &pos, u32 count, s32 speed, s16 delay);
extern BigBrickBlockFileRow data_ov002_02108ab0[];
extern BigBrickBlockFileRow data_ov002_02108ab4[];
extern Vector3 data_ov002_021089e0;
void func_020393a4(dBgW_KcMbg *p, int v);
void func_02039394(dBgW_KcMbg *p, int v);
void _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
extern ClpsRow data_ov002_02108ab8[];         /* CLPS blocks,       0xc stride */
extern s8 data_0209f2f8;              /* current level */
extern u8 data_0209f220;
int IsStarCollectedInCurLevel(int star);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mat, int scale, s16 angY,
CLPS_Block *clps);
}

/* One class, six actor IDs (see the banner), so most members branch on
 * actorID. The ROM materializes each comparison into a register and then
 * tests that (cmp/moveq #1/movne #0/cmp #0/b); this int-returning predicate
 * is what asks for that shape -- a bare `if (actorID == 0x11)` folds the test
 * into the branch and compiles shorter. */
static inline int IsActorID(dActor_c *actor, int id) {
    return actor->actorID == id;
}

/* Emission order is ROM order: functions must stay ROM-ascending.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN13daObjBlockL_cD1Ev
// @symbol _ZN13daObjBlockL_cD0Ev
/* ONE definition, TWO ROM functions. Two vtable stores and three destructor
 * calls, every one a consequence of `struct daObjBlockL_c : dBgActor_c`: its
 * own vptr, then dBgActor_c's -- inlined, because dBgActor_c's destructor is
 * defined in its class body -- then dBgActor_c's Model and dBgW_KcMbg, then
 * dActor_c. This class adds no member with a destructor of its own. D0
 * additionally returns the object to the actor heap through the inline
 * operator delete, which is why nothing below mentions a heap. With deferred
 * code generation off the two variants land in the cartridge's order, D1
 * first at 0x020b35a0 and D0 at 0x020b35e4.
 */
daObjBlockL_c::~daObjBlockL_c()
{
}
// @symbol _ZN13daObjBlockL_c17NotifyLinkedActorEv
/* Called from Kill. Follows mLinkedActor: clears a byte on the mega
   character (actor 0x121), or arms mActionPending on the item tag
   (actors 0x141..0x144) so the tag dispatches its stored action. */
void daObjBlockL_c::NotifyLinkedActor()
{
    dActor_c* linkedActor = mLinkedActor;
    int actorID;
    if (linkedActor == 0)
        return;

    actorID = linkedActor->actorID;
    if (actorID <= 0x121) {
        if (actorID == 0x121)
            ((u8*)linkedActor)[0x3b0] = 0;
        return;
    }

    switch (actorID) {
    case 0x141:
    case 0x142:
    case 0x143:
    case 0x144:
        ((daObjBlockItemTag_c *)linkedActor)->mActionPending = 1;
    }
}

// @symbol _ZN13daObjBlockL_c16HasNonzeroAngleXEv
/* Both callers are daObjBlockL_c methods in this TU. The field at 0x8c is the
   inherited dActor_c::mAngleX; no external state participates. */
bool daObjBlockL_c::HasNonzeroAngleX()
{
    return mAngleX != 0;
}

// @symbol _ZN13daObjBlockL_c15OnHitByMegaCharER6Player
/* Vtable slot 27. player.IncMegaKillCount() then this class's own Kill
 * (slot 31) through an unqualified virtual call. */
void daObjBlockL_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

// @symbol _ZN13daObjBlockL_c8OnKickedER8dActor_c
/* Vtable slot 24. `void`, NOT `int`: with two locals and two early returns,
 * mwcc allocates registers differently for `int` vs `void` even though the
 * ROM sets no r0 either way. Every other slot in this file (21/22/23/27)
 * byte-matches under `int`, so this is specific to this override.
 *
 * Every call is this class's own Kill (slot 31) through an unqualified
 * virtual call; the 0x2e branch calls Kill and falls through without
 * returning. */
void daObjBlockL_c::OnKicked(dActor_c &other)
{
    int p1;
    if (IsActorID(this, 0x2e))
        Kill();
    p1 = other.param1;
    if (p1 == 3) return;
    if (IsActorID(this, 0x11)) {
        if (p1 != 2) return;
        Kill();
    } else {
        Kill();
    }
}

// @symbol _ZN13daObjBlockL_c11OnAttacked2ER8dActor_c
/* Vtable slot 23. Both branches call this class's own Kill (slot 31)
 * through an unqualified virtual call. */
int daObjBlockL_c::OnAttacked2(dActor_c &other)
{
    if (IsActorID(this, 0x11)) {
        if (other.param1 != 2) return;
        Kill();
        return;
    }
    Kill();
}

// @symbol _ZN13daObjBlockL_c11OnAttacked1ER8dActor_c
/* Vtable slot 22; `other` is never read. The single call is this class's
 * own Kill (slot 31) through an unqualified virtual call. */
int daObjBlockL_c::OnAttacked1(dActor_c &other)
{
    if (IsActorID(this, 0x11))
        return;
    Kill();
}

// @symbol _ZN13daObjBlockL_c15OnGroundPoundedER8dActor_c
/* Vtable slot 21. Both branches call this class's own Kill (slot 31)
 * through an unqualified virtual call. Neither branch sets r0, so the bare
 * `return;` in this `void` override compiles to the same bytes as the ROM's
 * stub, which never touches r0 either. */
void daObjBlockL_c::OnGroundPounded(dActor_c &other)
{
    if (other.param1 == 3) return;
    if (IsActorID(this, 0x11)) {
        if (other.param1 != 2) return;
        Kill();
    } else {
        Kill();
    }
}

// @symbol _ZN13daObjBlockL_c4KillEv
/* daObjBlockL_c::Kill() -- vtable slot 31, the one virtual dBgActor_c adds.
 *
 * ONE CLASS, FIVE ACTOR IDS, which is why this function is a switchboard rather
 * than five overrides: BrickBlock, daObjBlockL_c, BrickBlockSwitchActivated,
 * BlackBrickBlock and FortressTowerWall all allocate 816 bytes, run
 * dBgActor_c::dBgActor_c and store _ZTV13daObjBlockL_c. See include/daObjBlockL_c.h.
 *
 *   0x11  the block that holds a star. If mStarID is set it releases the star:
 *         through dActor_c::UntrackAndSpawnStar when param1's second byte is 0xff
 *         (the star was tracked, so the marker slot has to be freed too), and
 *         through dActor_c::Spawn of actor 0xb2 with the id tagged 0x40 otherwise.
 *         Its poof rides 0x96000 -- 150 20.12 units -- up instead of 0x64000,
 *         and uses particle 0xf.
 *   0x0f  and 0x11 when the star path did not fire: three coins, but only when
 *         mLinkedActor is null. NotifyLinkedActor handles that actor just below.
 *   0x2e  particle 0x48 rather than 0xa.
 *   0x13  the switch-activated one: it sets mBroken and returns WITHOUT
 *         destroying itself, because Behavior re-enables it from the event bit.
 *         Every other id falls through to MarkForDestruction.
 *
 * The ROM materialises every actorID comparison into a register first and
 * then tests THAT (see IsActorID above; argued in full in
 * src/game/actors/d_a_obj_fall_block.cpp). is11 and isF stay named locals:
 * the else-branch reuses the very register is11 was materialised into for
 * `isF || is11`, and spelling isF through IsActorID breaks that.
 *
 * The last Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six.
 *
 * Particle::System::NewSimple and dActor_c::SpawnCoins stay spelled as their
 * mangled names -- both take Fix12<int> BY VALUE and declaring the true types
 * changes how the caller passes them. include/dActor_c.h says the same thing at the
 * bottom of the class, src/runtime/actor/d_bg_actor.cpp argues it in full, and
 * notes/mwccarm-codegen.md 6az is the rule. */
void daObjBlockL_c::Kill()
{
    Vector3 starPos;
    Vector3 pos;
    Vector3 coinPos;
    Vector3 dustPos;

    int is11 = (actorID == 0x11);
    if (is11 && mStarID != 0) {
        starPos.x = mPosX;
        starPos.y = mPosY;
        starPos.z = mPosZ;
        starPos.y += 0x96000;

        if (((param1 >> 8) & 0xff) == 0xff) {
            UntrackAndSpawnStar(mTrackStarID, mStarID, starPos, 4);
        } else {
            Spawn(0xb2, mStarID | 0x40, starPos, 0, mAreaId, -1);
        }
    } else {
        int isF = (actorID == 0xf);
        if (isF || is11) {
            if (mLinkedActor == 0) {
                coinPos.x = mPosX;
                coinPos.y = mPosY;
                coinPos.z = mPosZ;
                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, coinPos, 3, 0xf000, 0);
            }
        }
    }

    {
        Fix12i z = mPosZ;
        Fix12i y = mPosY + 0x64000;
        Fix12i x = mPosX;
        pos.x = x;
        pos.z = z;
        pos.y = y;
    }

    u32 kind = 0xa;
    if (IsActorID(this, 0x11)) {
        pos.y = mPosY + 0x96000;
        kind = 0xf;
    }
    if (IsActorID(this, 0x2e)) {
        kind = 0x48;
    }
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(kind, pos.x, pos.y, pos.z);

    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);

    Sound::PlayBank3(0x41, mCamSpacePos);

    NotifyLinkedActor();

    if (IsActorID(this, 0x13)) {
        mBroken = 1;
        return;
    }

    if (HasNonzeroAngleX()) {
        SpawnSoundObj(1);
    }
    MarkForDestruction();
}

// @symbol _ZN13daObjBlockL_c16CleanupResourcesEv
int daObjBlockL_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov002_02108ab0[mVariant].file->Release();
    data_ov002_02108ab4[mVariant].file->Release();
    return 1;
}

// @symbol _ZN13daObjBlockL_c6RenderEv
int daObjBlockL_c::Render()
{
    if (IsActorID(this, 0x13)) {
        if (!(Event::GetBit(mEventID) != 0 && mBroken == 0)) {
            return 1;
        } else {
            if (mSwitch != 0) {
                unsigned short d = (unsigned short)(mSwitch->mTimeLimit - mSwitch->mTimer);
                if (d < 0x2d) {
                    if ((d & 1) != 0)
                        return 1;
                }
            }
        }
    }
    if (IsActorID(this, 0x10)) {
        RawVector3 v = *(RawVector3 *)&data_ov002_021089e0;
        mModel.Render((Vector3 *)&v);
    } else {
        mModel.Render(0);
    }
    return 1;
}

// @symbol _ZN13daObjBlockL_c8BehaviorEv
/* func_020393a4 and func_02039394 are the ROM's unnamed collision setters;
   IsClsnInRangeOnScreen takes Fix12 by value, so those calls stay spelled
   as raw ABI. */
int daObjBlockL_c::Behavior()
{
    if (IsActorID(this, 0x13)) {
        if (mPrevEventBit != Event::GetBit(mEventID))
            mBroken = 0;

        if (mSwitch == 0) {
            unsigned int id = 0xb;
            do {
                mSwitch = (daObjSwitch_c *)FindWithActorID(id, mSwitch);
            } while (mSwitch == 0 || mEventID != mSwitch->mEventBit);
        }

        if (Event::GetBit(mEventID) == 0 || mBroken != 0) {
            if (mMeshCollider.IsEnabled() != 0)
                mMeshCollider.Disable();
        } else {
            func_020393a4(&mMeshCollider, 0x15e000);
            func_02039394(&mMeshCollider, 0x64000);
            _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x150000, 0);
        }

        mPrevEventBit = Event::GetBit(mEventID);
    } else {
        int v1 = 0x15e000;
        int v5 = 0x64000;
        if (IsActorID(this, 0x10) || IsActorID(this, 0x11) || IsActorID(this, 0x2e)) {
            v1 = 0x1c2000;
            v5 = 0x96000;
        }
        func_020393a4(&mMeshCollider, v1);
        func_02039394(&mMeshCollider, v5);
        _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x600000, 0);
    }
    return 1;
}

// @symbol _ZN13daObjBlockL_c13InitResourcesEv
/* daObjBlockL_c::InitResources -- one class, six actor IDs. mVariant selects the
 * row of the three 0xc-stride resource tables (model, collision, CLPS) that this
 * block loads; the collision scale then varies again by ID -- 0x1800 for 0x10,
 * 0x1000 for the other variant-0/1 blocks, 0x199 for variant 2.
 *
 * Actor 0x11 is the star block: the low byte of param1 is the star index (0xff
 * meaning none) and the second byte being 0xff registers a star marker.
 * Actor 0x13 takes the event bit from the low nibble of param1 instead.
 *
 * The two tail cases return 0 -- refuse to spawn: a 0x11 block whose star is
 * already taken when the player is entering level 1, and a 0x2e block above
 * y 0xdac000 in level 7 until star 1 is collected (or when data_0209f220 is 1).
 *
 * Each actorID test after the switch re-reads actorID through IsActorID; the
 * switch's own copy is not reused, and folding them changes the compare shape.
 * dBgW_KcMbg::SetFile takes Fix12<int> by value (wall 6az) so it stays an
 * extern-C mangled free function.
 */
int daObjBlockL_c::InitResources()
{
    u16 id = actorID;
    u8 idx;

    switch (id) {
    case 0xf:
    case 0x10:
    case 0x13: mVariant = 0; break;
    case 0x11: mVariant = 1; break;
    case 0x12: break;
    case 0x2e: mVariant = 2; break;
    }

    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov002_02108ab0[mVariant].file), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    if (IsActorID(this, 0x11)) {
        int byte1;
        mStarID = param1;
        byte1 = (param1 >> 8) & 0xff;
        if (mStarID == 0xff)
            mStarID = 0;
        if (mStarID != 0 && byte1 == 0xff)
            mTrackStarID = TrackStar(mStarID, 2);
    }

    idx = mVariant;
    if (idx <= 1) {
        if (IsActorID(this, 0x10)) {
            KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov002_02108ab4[idx].file);
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMeshCollider, kcl, &mClsnMat, 0x1800, mAngleY,
                data_ov002_02108ab8[idx].block);
        } else {
            KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov002_02108ab4[idx].file);
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
                data_ov002_02108ab8[idx].block);
        }
    } else {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov002_02108ab4[idx].file);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
            data_ov002_02108ab8[idx].block);
    }

    if (IsActorID(this, 0x13))
        mEventID = param1 & 0xf;
    mPrevEventBit = Event::GetBit(mEventID);

    {
    Player *pl = ClosestPlayer();
    if (pl != 0 && pl->IsEnteringLevel() != 0) {
        if (IsActorID(this, 0x11) && HasNonzeroAngleX()) {
            if (data_0209f2f8 == 1)
                return 0;
        }
    }
    }

    if (IsActorID(this, 0x2e) && data_0209f2f8 == 7) {
        if (data_0209f220 == 1 || IsStarCollectedInCurLevel(1) == 0) {
            if (mPosY >= 0xdac000)
                return 0;
        }
    }
    return 1;
}

/* The five registry factories, one per spawn profile, in ROM order.
 * Reconstructed source-style names: SM64DS proves daObjBlockL_c through
 * RTTI, allocation size, vtable identity, and the BK_FUTA, BLOCK_LL,
 * SW_BLOCK_L, DP_BLOCK_L and BLOCK_L registry profiles; later EAD lineage
 * supplies classInit. Exact original spellings are not preserved.
 * Historical aliases: FortressTowerWall_Spawn, BlackBrickBlock_Spawn,
 * BrickBlockSwitchActivated_Spawn, BigBrickBlock_Spawn, BrickBlock_Spawn. */
// @symbol daObjBlockL_c_classInit_BK_FUTA
extern "C" daObjBlockL_c *daObjBlockL_c_classInit_BK_FUTA()
{
    return new daObjBlockL_c();
}

// @symbol daObjBlockL_c_classInit_BLOCK_LL
extern "C" daObjBlockL_c *daObjBlockL_c_classInit_BLOCK_LL()
{
    return new daObjBlockL_c();
}

// @symbol daObjBlockL_c_classInit_SW_BLOCK_L
extern "C" daObjBlockL_c *daObjBlockL_c_classInit_SW_BLOCK_L()
{
    return new daObjBlockL_c();
}

// @symbol daObjBlockL_c_classInit_DP_BLOCK_L
extern "C" daObjBlockL_c *daObjBlockL_c_classInit_DP_BLOCK_L()
{
    return new daObjBlockL_c();
}

// @symbol daObjBlockL_c_classInit_BLOCK_L
extern "C" daObjBlockL_c *daObjBlockL_c_classInit_BLOCK_L()
{
    return new daObjBlockL_c();
}

/* Retail initializer order. The handles themselves land in address order
 * because their names sort that way; these definitions are the call order. */
extern "C" BlockModelFile data_ov002_0210dcc0(1108);
extern "C" daObjBlockLClsnFile data_ov002_0210dcb8(1109);
extern "C" BlockModelFile data_ov002_0210dce0(1110);
extern "C" daObjBlockLClsnFile data_ov002_0210dcc8(1111);
extern "C" BlockModelFile data_ov002_0210dcd0(1425);
extern "C" daObjBlockLClsnFile data_ov002_0210dcd8(1426);
