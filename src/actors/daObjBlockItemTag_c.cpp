//cpp
/* Recovered translation unit -- ov002/daObjBlockItemTag_c, the item-tag blocks.
 *
 * .text span 0x020b415c..0x020b45e0, 10 functions, ROM ordinals 0..9, exactly
 * the contiguous linker run build/tu_map.json places here. All 10 are assembled
 * below and config/tu_manifest.d/ov002/daObjBlockItemTag_c.json names every one,
 * so the run has no hole and the 10 one-function files under src/ that used to
 * own these bytes are deleted by this change.
 *
 * IDENTITY IS THE CARTRIDGE'S. ov002 holds _ZTS19daObjBlockItemTag_c at
 * 0x02108b88; no BrickBlock RTTI record exists anywhere -- that was a coined
 * alias for this same class. The 31-slot table at 0x02108c18 carries this
 * class's own _ZTI at its typeinfo word, has exactly dActor_c's extent, and
 * overrides only slots 0, 3, 6, 16 and 17. Four adjacent factories
 * (0x020b45e0..0x020b4670) independently allocate 0xdc bytes, call dActor_c's
 * constructor and install this same table: the item-tag variants for One-Up
 * Mushroom, Mega Mushroom, Koopa Shell and Silver Star blocks.
 *
 * FUNCTION ORDER IS THE ROM'S OWN, under `#pragma defer_codegen off`.
 * Deferred code generation stays off for this TU so mwccarm emits one .text
 * section per function in SOURCE order: the lowest-address ROM function
 * (_ZN19daObjBlockItemTag_cD1Ev, 0x020b415c) is written FIRST and the highest
 * (_ZN19daObjBlockItemTag_c13InitResourcesEv, 0x020b451c) LAST. Do not reorder.
 * The verifier reports all 10 sections in the expected ROM-ascending order.
 *
 * THE DESTRUCTOR IS ONE DEFINITION AND TWO SECTIONS (plus a homeless D2).
 * A single `daObjBlockItemTag_c::~daObjBlockItemTag_c()` emits D1 and D0, and
 * with deferred code generation off they land in the cartridge's order, D1
 * first at 0x020b415c and D0 at 0x020b4180.
 * compiler order, which is exactly the cartridge's (0x020b415c D1, 0x020b4180
 * D0). The homeless D2 has no ROM symbol and no inbound relocation once the
 * leaf D1/D0 pair is retained, so the manifest licenses it as deadstrip
 * compiler-only output.
 *
 * KEY-FUNCTION SIDE EFFECT: none claimed. ~daObjBlockItemTag_c is the first
 * non-inline virtual and this TU defines it, but the promotion is text-only:
 * the manifest's compiler_only_output externalizes _ZTV/_ZTI/_ZTS to their
 * canonical public addresses (deadstrip-data) and claims no data or BSS range.
 *
 * WHAT THE PROMOTION CARRIES OVER, and nothing else was changed:
 *   1. The resource handles stay `char[]` with address-of casts at the two use
 *      sites; the duplicate `int` spellings from InitResources' shard are
 *      dropped (every use is address-of plus cast, so the spelling is
 *      codegen-neutral).
 *   2. LinkSilverStarAndStarMarker, Vec3_Dist and the PMF action table stay
 *      spelled as mangled externs: the helpers take their arguments in forms
 *      no shared header declares without changing the call shapes.
 *   3. The destructor is one definition with two stacked markers -- writing
 *      them as two definitions is an `object redefined` error.
 *
 * The 10 legacy one-function sources this folds are named by the manifest's
 * `legacy_source` fields (history); they are not repeated here.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjBlockItemTag_c.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dActor_c.h"
#include "daObjBlockL_c.h"
#include "Model.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow typedef 'void' */
typedef void (daObjBlockItemTag_c::*ItemTagAction)();

extern "C" {
extern s32 data_ov002_020ff090[];
extern "C" void LinkSilverStarAndStarMarker(void *starMarker, void *silverStar);
extern char data_ov002_0210d9d8[];
extern char data_ov002_0210da30[];
extern char data_ov002_0210da18[];
extern "C" ItemTagAction data_ov002_0210dd30[];
extern s32 Vec3_Dist(const void *a, const void *b);
extern void LoadSilverStarAndNumber();
}

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0/1 -- _ZN19daObjBlockItemTag_cD1Ev 0x020b415c, _ZN19daObjBlockItemTag_cD0Ev 0x020b4180. */
/* ONE definition, two emitted sections.                                          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBlockItemTag_cD1Ev
// @symbol _ZN19daObjBlockItemTag_cD0Ev
/* Genuine destructor. A single `daObjBlockItemTag_c::~daObjBlockItemTag_c()`
 * emits D1 and D0, in compiler order, which is exactly the cartridge's
 * (0x020b415c D1, 0x020b4180 D0). D0 additionally returns the object to the
 * actor heap through dActor_c's inline operator delete, which is why nothing
 * below mentions a heap. The raw compiler object also materializes this
 * class's vtable and RTTI passengers; objisolate retains only licensed text. */
daObjBlockItemTag_c::~daObjBlockItemTag_c()
{
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN19daObjBlockItemTag_c11GetSpawnPosER7Vector3RS_, 0x020b41b8, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBlockItemTag_c11GetSpawnPosER7Vector3RS_
void daObjBlockItemTag_c::GetSpawnPos(Vector3 &destination, daObjBlockItemTag_c &tag)
{
    u8 index = tag.mActionIndex;
    s32 z = tag.mPosZ;
    s32 y = tag.mPosY + data_ov002_020ff090[index];
    s32 x = tag.mPosX;

    destination.x = x;
    destination.y = y;
    destination.z = z;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN19daObjBlockItemTag_c15SpawnKoopaShellEv, 0x020b41f8, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBlockItemTag_c15SpawnKoopaShellEv
void daObjBlockItemTag_c::SpawnKoopaShell()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    dActor_c *shell = Spawn(0x11d, 0, spawnPos, 0, mAreaId, -1);
    if (shell)
        *(u8 *)((char *)shell + 0x3c6) = 0xb4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN19daObjBlockItemTag_c17SpawnMegaMushroomEv, 0x020b4250, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBlockItemTag_c17SpawnMegaMushroomEv
void daObjBlockItemTag_c::SpawnMegaMushroom()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    Spawn(0x115, 0, spawnPos, 0, mAreaId, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN19daObjBlockItemTag_c18SpawnOneUpMushroomEv, 0x020b429c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBlockItemTag_c18SpawnOneUpMushroomEv
void daObjBlockItemTag_c::SpawnOneUpMushroom()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    Spawn(0x114, 0, spawnPos, 0, mAreaId, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN19daObjBlockItemTag_c15SpawnSilverStarEv, 0x020b42e4, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBlockItemTag_c15SpawnSilverStarEv
void daObjBlockItemTag_c::SpawnSilverStar()
{
    Vector3 spawnPos;
    dActor_c *starMarker;
    dActor_c *silverStar;

    UntrackStar(mTrackStarID);
    GetSpawnPos(spawnPos, *this);
    starMarker = Spawn(0xb4, 0x50, spawnPos, 0, mAreaId, -1);
    silverStar = Spawn(0xb3, mStarID | 0x10, spawnPos, 0, mAreaId, -1);
    if (starMarker == 0)
        return;
    if (silverStar == 0)
        return;

    /* Silver Star's marker-owner unique ID is the word at 0x434. */
    *(u32 *)((char *)silverStar + 0x434) = starMarker->uniqueID;
    LinkSilverStarAndStarMarker(starMarker, silverStar);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN19daObjBlockItemTag_c16CleanupResourcesEv, 0x020b4394, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBlockItemTag_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daObjBlockItemTag_c::CleanupResources()
{
  int v = actorID;
  switch(v){
  case 0x141: ((SharedFilePtr *)(data_ov002_0210d9d8))->Release(); break;
  case 0x142: ((SharedFilePtr *)(data_ov002_0210da30))->Release(); break;
  case 0x143: ((SharedFilePtr *)(data_ov002_0210da18))->Release(); break;
  case 0x144: UnloadSilverStarAndNumber(); break;
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN19daObjBlockItemTag_c8BehaviorEv, 0x020b440c, size 0x110 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBlockItemTag_c8BehaviorEv
int daObjBlockItemTag_c::Behavior()
{
    dActor_c *block = 0;
    if (mIsAttached != 0)
        goto attached;

    block = dActor_c::Next(0);
    while (block) {
        u16 type = block->actorID;
        int isBlock;
        isBlock = (type == 0x10);
        if (!isBlock) {
            isBlock = (type == 0x0f);
            if (!isBlock) {
                isBlock = (type == 0x11);
                if (!isBlock)
                    goto next;
            }
        }
        if (Vec3_Dist(&mPosX, &block->mPosX) < 0x32000) {
            ((daObjBlockL_c *)block)->mLinkedActor = this;
            mIsAttached = 1;
            return 1;
        }
    next:
        block = dActor_c::Next(block);
    }

    if (block)
        goto attached;
    MarkForDestruction();
    return 1;

attached:
    if (mActionPending != 0) {
        int index = mActionIndex;
        (this->*data_ov002_0210dd30[index])();
        MarkForDestruction();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN19daObjBlockItemTag_c13InitResourcesEv, 0x020b451c, size 0xc4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBlockItemTag_c13InitResourcesEv
int daObjBlockItemTag_c::InitResources()
{
    mStarID = param1 & 0x7f;
    if (mStarID == 0x7f)
        mStarID = 0;

    switch (actorID) {
    case 0x141:
        mActionIndex = 0;
        Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210d9d8);
        break;
    case 0x142:
        mActionIndex = 1;
        Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210da30);
        break;
    case 0x143:
        mActionIndex = 2;
        Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210da18);
        break;
    case 0x144:
        mActionIndex = 3;
        /* TrackStar's star index is unsigned even though this stored byte is
         * signed at the other ROM load sites. The cast preserves the ldrb. */
        mTrackStarID = TrackStar((u8)mStarID, 1);
        LoadSilverStarAndNumber();
        break;
    }
    return 1;
}
