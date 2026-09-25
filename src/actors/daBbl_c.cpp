//cpp
/* daBbl_c -- the Lava Bubble (Podoboo), BUBBLE, ov064: 14 functions,
 * .text 0x021185c0..0x02118b50.
 *
 * The Lava Bubble of the lava levels. Two
 * variants share the class and are chosen by the spawn parameter's low bit: a
 * fixed flame that only hurts what walks into it, and the jumping bubble that
 * leaps out of the lava under gravity, can be hit, and dies when its fuse runs
 * out. Both run the same two-entry pointer-to-member state table.
 *
 * WHAT THE CARTRIDGE PROVES:
 *   _ZTS  ov064 0x0211beb0  "7daBbl_c" -- nine bytes padded to twelve, exactly
 *                           the length this very name mangles to, so the RTTI
 *                           name is the ROM's own and nothing here is coined
 *   _ZTI  ov064 0x0211bebc  __si_class_type_info; one direct base,
 *                           dEnemyBase_c (_ZTI12dEnemyBase_c, ov002
 *                           0x021081c0), at offset zero
 *   _ZTV  ov064 0x0211beec  the address point -- storage 0x0211bee4, typeinfo
 *                           word 0x0211bee8 -> 0x0211bebc; 31 slots, the same
 *                           extent as dActor_c, with 0, 3, 6, 9, 12, 16, 17
 *                           and 18 overridden
 *   profile ov064 0x0211bec8  g_profile_BUBBLE, whose factory word is
 *                           daBbl_c_classInit at 0x02118b10
 *   size  0x31c             the literal the factory hands operator new
 *
 * WHY THIS IS ONE TRANSLATION UNIT: the run 0x021185c0..0x02118b50 is
 * contiguous. Its left edge is the first instruction after daObjFl_KomaU_c's
 * own factory, where that class's run stops; its right edge is the first
 * instruction of daBbl_c's neighbour BowserPuzzlePiece. Every function inside
 * it is either a named daBbl_c member, this class's factory, or one of the
 * five still-unnamed hooks the class reaches only through its own state
 * pointer at 0x300 -- no foreign class appears anywhere in the span.
 * config/arm9/overlays/ov064/delinks.txt enrolled all fourteen as adjacent
 * `complete` entries with no hole, so the whole run could be claimed at once.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * inline destructor in include/daBbl_c.h emits the retail D1/D0 pair first and
 * emits no D2 body.
 *
 * Known limits:
 * - Particle::System::New and dActor_c::IsTooFarAwayFromPlayer keep their C
 *   ABI spellings: their real declarations pass Fix12<int> by value, which
 *   mwccarm lowers differently at a C++ call site.
 * - the five state hooks are still linker names. They are unmistakably this
 *   class's -- the .data pointer-to-member constants at 0x0211be90..0x0211beb0
 *   name four of the hook bodies, and a pointer-to-member constant can only
 *   name a member -- but the cartridge records no English name for any of
 *   them, and coining four is a rename PR rather than this one. They are typed
 *   against the class here so they at least read as the members they are.
 * - the two State tables at 0x0211c7b8 and 0x0211c7c8 are this overlay's .bss,
 *   filled by the module's own static initializer. This TU consumes them and
 *   does not own them, so its claim is .text only.
 *
 * Consolidated from fourteen one-function sources, one per symbol below. Their
 * retired paths are recorded per function in this TU's manifest entry; naming
 * them here would leave the comment pointing at files the fold deletes.
 * ROM address order:
 *   [0]  0x021185c0  _ZN7daBbl_cD1Ev
 *   [1]  0x021185f8  _ZN7daBbl_cD0Ev
 *   [2]  0x02118644  func_ov064_02118644
 *   [3]  0x0211873c  func_ov064_0211873c
 *   [4]  0x02118760  func_ov064_02118760
 *   [5]  0x021187d0  func_ov064_021187d0
 *   [6]  0x021187ec  func_ov064_021187ec
 *   [7]  0x0211883c  _ZN7daBbl_c16CleanupResourcesEv
 *   [8]  0x02118844  _ZN7daBbl_c16OnPendingDestroyEv
 *   [9]  0x02118848  _ZN7daBbl_c6RenderEv
 *   [10] 0x02118850  _ZN7daBbl_c8BehaviorEv
 *   [11] 0x021189d8  _ZN7daBbl_c13InitResourcesEv
 *   [12] 0x02118b08  _ZN7daBbl_c13OnYoshiTryEatEv
 *   [13] 0x02118b10  daBbl_c_classInit
 */

#include "daBbl_c.h"
#include "Player.h"

/* Ramp `value` toward `target` by at most `step`. The mangled name this
 * declaration produces is the compiler's own answer, not a hand-mangle. */
void ApproachLinear(int &value, int target, int step);

extern "C" {
/* Counts a halfword timer down and returns what it was; a short* helper, so
 * the slot it is handed is a HALFWORD. */
u16 DecIfAbove0_Short(u16 *timer);

/* The six-argument particle spawner: it takes the handle it last returned,
 * rolls it forward and hands back the new one. Kept at its C ABI spelling
 * because its real declaration passes three Fix12<int> by value; the trailing
 * two parameters are spelt as its definition spells them. */
struct Vec3;
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 handle, u32 effectID, int x, int y, int z, const Vec3 *rot,
    void *callback);

/* True while the player is further than `dist` away. Fix12<int> by value
 * again. */
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(dActor_c *self, int dist);

/* The actor-collision cylinder's own initialiser; two more Fix12<int> by
 * value, so it stays at its C ABI spelling too. */
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height,
    u32 flags, u32 vulnFlags);

/* The mesh collider's initialiser. include/dBgCh_Actr.h declares it with the
 * scalar Fix12i spelling for readers, but the definition the cartridge links
 * carries Fix12<int> in its own name, so the call site has to spell the ABI
 * seam -- and that definition takes every argument past the receiver as a
 * plain word, which is why the owner is handed over as one. */
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, int actor, int radius, int height, int rotA, int rotB);

/* This TU's own state hooks -- see Known limits above. Declared here
 * because the state tables reach them by pointer-to-member, and because
 * InitResources, written first, calls the transition. */
int func_ov064_02118644(daBbl_c *self);
int func_ov064_0211873c(daBbl_c *self);
int func_ov064_02118760(daBbl_c *self);
int func_ov064_021187d0(daBbl_c *self);
int func_ov064_021187ec(daBbl_c *self, daBbl_c::State *state);

/* The two state tables, this overlay's .bss. include/decl_common.h declares
 * both as raw byte arrays for the whole tree, so they keep that spelling here
 * and the class's own view of them is applied where they are handed over. */
extern char data_ov064_0211c7b8[];
extern char data_ov064_0211c7c8[];
}

// @symbol daBbl_c_classInit
/* Reconstructed source-style name: the cartridge proves daBbl_c through RTTI,
 * allocation size and vtable identity, and the BUBBLE profile at 0x0211bec8
 * takes this function's address as its factory slot; later EAD lineage
 * supplies the classInit spelling. The exact original spelling is not
 * preserved. Historical alias: LavaBubble_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`. The
 * literal 0x31c is the class's own size, handed to the leaf operator new the
 * header declares; then dEnemyBase_c's C2, this class's vptr store, and the
 * dCcAc_c (0x110) and dBgCh_Actr (0x144) member constructors, in declaration
 * order. The null check is the one `new` itself emits, and the implicit
 * default constructor is what keeps that construction inlined -- declaring a
 * constructor of our own would emit a `bl` the factory does not have. */
extern "C" daBbl_c *daBbl_c_classInit()
{
    return new daBbl_c();
}

// @symbol _ZN7daBbl_c13OnYoshiTryEatEv
/* Slot 18. Yoshi is told 5 -- the reply a thing made of fire gives. */
s32 daBbl_c::OnYoshiTryEat()
{
    return 5;
}

// @symbol _ZN7daBbl_c13InitResourcesEv
/* Slot 0. The spawn parameter's low bit picks the variant, inverted: bit set
 * means the fixed flame, bit clear means the jumping bubble.
 *
 * The flame gets a bare flag-1 cylinder and dActor_c's own 0x1 flag, which
 * pins it in place; the jumper gets a 50.0 x 80.0 cylinder that can be hit
 * (0x200002 / 0x8000) plus gravity and a terminal velocity. Both remember
 * where they started, and both adopt a state table before returning. */
int daBbl_c::InitResources()
{
    mJumps = (param1 & 1) ^ 1;
    if (mJumps == 0) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0, 0, 1, 0);
    } else {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, 0x32000, 0x50000, 0x200002, 0x8000);
    }
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    if (mJumps == 0) {
        mFlags |= 1;
    } else {
        mVertAccel = -0x4000;
        mTerminalVelocity = -0x3c000;
    }
    mStateTimer = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, (int)this, 0x32000, 0x32000, 0, 0);
    mWithMeshClsn.SetLimMovFlag();
    if (mJumps != 0) {
        func_ov064_021187ec(this, (daBbl_c::State *)data_ov064_0211c7c8);
    } else {
        func_ov064_021187ec(this, (daBbl_c::State *)data_ov064_0211c7b8);
    }
    return 1;
}

// @symbol _ZN7daBbl_c8BehaviorEv
/* Slot 6. Two early outs come first, on the two dEnemyBase_c flags that mean
 * the enemy is being held or is already dying: 0x20000 still runs the state
 * hook but skips everything else, 0x40000 skips the frame outright.
 *
 * Then the ordinary frame: destroy the actor once the player is more than
 * 1500.0 away -- but only the jumping variant, which a flame fountain
 * respawns; the fixed flame is part of the level. Count the state timer down,
 * burn whatever touched the cylinder if it is the player, run the state hook,
 * integrate position, and -- only for the variant that has gravity -- collide
 * with the mesh. */
int daBbl_c::Behavior()
{
    int flags = mFlags;
    int held = (flags & 0x20000) != 0;

    if (held) {
        if (mState->mExecute != 0)
            (this->*(mState->mExecute))();
        return 1;
    }

    {
        int dying = (flags & 0x40000) != 0;
        if (dying)
            return 1;
    }

    if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(this, 0x5dc000)) {
        if (mJumps != 0)
            MarkForDestruction();
        return 1;
    }

    DecIfAbove0_Short((u16 *)&mStateTimer);

    {
        u32 otherID = mdCcAc_c.otherOwner;
        if (otherID != 0) {
            if ((mdCcAc_c.hitFlags & 0x8000) == 0) {
                dActor_c *other = dActor_c::FindWithID(otherID);
                if (other != 0) {
                    int isPlayer = (other->actorID == 0xbf);
                    if (isPlayer)
                        ((Player *)other)->Burn();
                }
            } else {
                mdCcAc_c.flags |= 1;
            }
        }
    }

    if (mState->mExecute != 0)
        (this->*(mState->mExecute))();

    UpdatePos(&mdCcAc_c);

    if (mVertAccel != 0)
        UpdateWMClsn(mWithMeshClsn, 0);

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN7daBbl_c6RenderEv
/* Slot 9. `return 1` and nothing else -- the whole ROM body is
 * `mov r0,#1; bx lr`. A lava bubble has no model of its own; its two particle
 * systems draw it, and the render slot only reports success. */
int daBbl_c::Render()
{
    return 1;
}

// @symbol _ZN7daBbl_c16OnPendingDestroyEv
/* Slot 12. Empty -- the ROM body is a single `bx lr`. The override exists to
 * suppress what the base does on pending destroy, not to do anything itself. */
void daBbl_c::OnPendingDestroy()
{
}

// @symbol _ZN7daBbl_c16CleanupResourcesEv
/* Slot 3. `return 1` with no release calls, which is the finding rather than a
 * stub: every other class in this overlay releases at least one SharedFilePtr
 * here (daObjShell_c 3, daObjTbox_c 4, daWater_Hakidasi_c 2, daWater_Ring_c 1).
 * daBbl_c loads no file at all, so it has nothing to give back. */
int daBbl_c::CleanupResources()
{
    return 1;
}

// @symbol func_ov064_021187ec
/* The state transition: adopt a table and run its enter hook once. Behavior
 * runs the execute hook of whatever table is current. */
extern "C" int func_ov064_021187ec(daBbl_c *self, daBbl_c::State *state)
{
    self->mState = state;

    daBbl_c::State *entered = self->mState;
    if (entered->mEnter == 0)
        return 1;
    return (self->*(entered->mEnter))();
}

// @symbol func_ov064_021187d0
/* Enter hook of the fountain's waiting state: stop dead and wait 180 frames. */
extern "C" int func_ov064_021187d0(daBbl_c *self)
{
    self->mHorzSpeed = 0;
    self->mStateTimer = 180;
    return 1;
}

// @symbol func_ov064_02118760
/* Execute hook of the waiting state: once the 180 frames are up, spawn actor
 * 0xd6 -- the bubble that actually jumps -- 120.0 above this one, and start
 * the wait over. This is what makes the fixed flame a flame fountain.
 *
 * THE VOLATILE STACK POSITION IS LOAD-BEARING. The three words are written
 * y, z, x in the cartridge, which is neither source order nor the order any
 * ordinary temporary comes out in; handing Spawn a plain Vector3 lets mwcc
 * re-order the stores and costs the match. */
extern "C" int func_ov064_02118760(daBbl_c *self)
{
    int spawnY;
    int spawnZ;

    if ((u16)self->mStateTimer == 0) {
        volatile int spawnPos[3];

        self->mStateTimer = 0xb4;
        int y = self->mPosY;
        int z = self->mPosZ;
        spawnY = 0x78000;
        spawnY = y + spawnY;
        spawnZ = z;
        int x = self->mPosX;
        spawnPos[1] = spawnY;
        spawnPos[2] = spawnZ;
        spawnPos[0] = x;
        dActor_c::Spawn(0xd6, 0, *(const Vector3 *)spawnPos,
                        (const Vector3_16 *)&self->mPrevAngleX,
                        self->mAreaId, -1);
    }
    return 1;
}

// @symbol func_ov064_0211873c
/* Enter hook of the jumping state: no horizontal drift, 20.0 straight up, and
 * a 135-frame fuse. */
extern "C" int func_ov064_0211873c(daBbl_c *self)
{
    self->mHorzSpeed = 0;
    self->mVertSpeed = 0x14000;
    self->mStateTimer = 135;
    return 1;
}

// @symbol func_ov064_02118644
/* Execute hook of the jumping state. Every landing bounces: the first gives
 * the bubble a little forward drift and a fixed hop, and each one after that
 * ramps the drift toward 8.0 and inverts seven tenths of the vertical speed,
 * so the hops decay. The fire and smoke particles are rolled forward every
 * frame 55.0 above the bubble, and the fuse the enter hook set is what finally
 * destroys it. */
extern "C" int func_ov064_02118644(daBbl_c *self)
{
    if (self->mWithMeshClsn.IsOnGround() != 0) {
        if (self->mHorzSpeed == 0) {
            self->mHorzSpeed = 0x16000;
            self->mVertSpeed = 0x32000;
        } else {
            ApproachLinear(self->mHorzSpeed, 0x8000, 0x3000);
            self->mVertSpeed = self->mVertSpeed * -7 / 10;
        }
    }
    self->mFireParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        self->mFireParticle, 0x4d, self->mPosX, self->mPosY + 0x37000, self->mPosZ, 0, 0);
    self->mSmokeParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        self->mSmokeParticle, 0x4e, self->mPosX, self->mPosY + 0x37000, self->mPosZ, 0, 0);
    if ((u16)self->mStateTimer == 0)
        self->MarkForDestruction();
    return 1;
}
