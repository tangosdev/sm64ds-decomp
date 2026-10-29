//cpp
/* Cool Cool Mountain's shatterable ice sheet (ICE_BOARD, actor 295): a
 * mesh collider you can stand on until a ground pound or a mega-Mario
 * hit breaks it -- one sound, three particle systems, then gone.
 *
 * _ZTS15daObjIceBoard_c 0x02113afc. _ZTI15daObjIceBoard_c 0x02113af0.
 * _ZTV15daObjIceBoard_c 0x02113b34. InitResources is the key function;
 * the destructor stays inline in the header, declared last: D1 at
 * 0x021127bc, D0 at 0x02112800, in cartridge order.
 *
 * Order is load-bearing: source runs REVERSE of ROM (highest address
 * first). Do not reorder.
 */

// Note: the daObjIceBoard_c_* file-handle names are this TU's own; the retired
//   shard's data_ov018_* labels for the same addresses are dropped from
//   symbols.txt with their decl_common.h lines.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_ice_board_AssetId {
    ASSET_OBJ_ICE_BOARD_BMD = 0x0472,
    ASSET_OBJ_ICE_BOARD_KCL = 0x0473
};

#include "daObjIceBoard_c.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x472, collision file 0x473). mwcc emits
 * __sinit_d_a_obj_ice_board.cpp from those definitions. The wrapper names
 * are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. The definitions
 * carry the TU's daObjIceBoard_c_* names the code already uses (mecha05 /
 * wcmizu precedent); the retired shard's data_ov018_* labels for the same
 * addresses are dropped from symbols.txt, and their stale decl_common.h
 * lines with them (S42). */
struct IceBoardModelFile : SharedFilePtr {
    IceBoardModelFile(u32 fileID);
    ~IceBoardModelFile();
};

struct IceBoardCollisionFilePtr : SharedFilePtr {
    IceBoardCollisionFilePtr(u32 fileID);
    ~IceBoardCollisionFilePtr();
};

extern IceBoardModelFile daObjIceBoard_c_ModelFile;
extern IceBoardCollisionFilePtr daObjIceBoard_c_ClsnFile;

extern CLPS_Block data_ov002_0210d754;

// @symbol daObjIceBoard_c_classInit
/* Every instruction the cartridge has here falls out of the one `new`.
 * 0x02112a48 loads 0x320 -- the class's own size -- into the header's leaf
 * operator new; 0x02112a58 calls dBgActor_c's C2 and the store at 0x02112a64
 * lays down this class's vptr. The null check is the one `new` itself emits.
 * The implicit constructor is what keeps the intervening dBgActor_c
 * construction inlined; declaring a constructor of our own would emit a `bl`
 * the factory does not have. */
extern "C" daObjIceBoard_c *daObjIceBoard_c_classInit() {
    return new daObjIceBoard_c();
}

// @symbol _ZN15daObjIceBoard_c13InitResourcesEv
int daObjIceBoard_c::InitResources() {
    void *modelFile = dExtSimpleModel_c::LoadFile(daObjIceBoard_c_ModelFile);
    mModel.SetFile((BMD_File *)modelFile, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *clsnFile = (KCL_File *)dBgW_Kc::LoadFile(daObjIceBoard_c_ClsnFile);
    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile(clsnFile, mClsnMat, 0x199, mAngleY, data_ov002_0210d754);
    return 1;
}

// @symbol _ZN15daObjIceBoard_c8BehaviorEv
int daObjIceBoard_c::Behavior() {
    if (!mMeshCollider.IsEnabled())
        mMeshCollider.Enable(this);
    return 1;
}

// @symbol _ZN15daObjIceBoard_c6RenderEv
int daObjIceBoard_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjIceBoard_c16CleanupResourcesEv
int daObjIceBoard_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    daObjIceBoard_c_ModelFile.Release();
    daObjIceBoard_c_ClsnFile.Release();
    return 1;
}

// @symbol _ZN15daObjIceBoard_c15OnGroundPoundedER8dActor_c
/* daObjIceBoard_c::OnGroundPounded -- vtable slot 21, ov018 0x021128e0.
 *
 * param1 is the player's character id; 2 is Wario. The trailing unqualified
 * Kill() reaches this class's own slot-31 override through the vtable. */
void daObjIceBoard_c::OnGroundPounded(dActor_c &other) {
    if (&other == 0)
        return;
    if (other.param1 != 2)
        return;
    Kill();
}

// @symbol _ZN15daObjIceBoard_c4KillEv
/* daObjIceBoard_c::Kill() at ov018 0x02112880, 0x60 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV15daObjIceBoard_c (ov018 0x02113b34) carries
 * 0x02112880 at slot 31 (0x02113bb0) and _ZTV10dBgActor_c carries
 * _ZN10dBgActor_c4KillEv at the same slot, so this is this class's own
 * override of dBgActor_c's Kill. The RTTI agrees on the class:
 * _ZTI15daObjIceBoard_c (ov018 0x02113af0) is a __si_class_type_info with ONE
 * base at subobject offset 0, ov002 0x021089ec -- dBgActor_c.
 *
 * The sheet shatters: the sound first, then three particle systems -- 0x74,
 * 0x75, 0x76 -- all at the actor's own position, and then it is gone.
 *
 * THE POSITION IS RE-READ PER CALL. This TU's Kill issues three `ldr` from
 * [this,#0x5c/#0x60/#0x64] ahead of each of the three calls, nine loads in
 * all, rather than loading the triple once into callee-saved registers.
 * Hoisting them into locals is one instruction shorter per call and does not
 * reproduce; naming the fields at each call site is what asks for the loads
 * the cartridge has. */
void daObjIceBoard_c::Kill() {
    Sound::PlayBank3(0x41, mCamSpacePos);
    Particle::System::NewSimple(0x74, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0x75, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0x76, mPosX, mPosY, mPosZ);
    MarkForDestruction();
}

// @symbol _ZN15daObjIceBoard_c15OnHitByMegaCharER6Player
/* daObjIceBoard_c::OnHitByMegaChar -- vtable slot 27, ov018 0x02112858.
 *
 * Player::IncMegaKillCount is a real method, and the trailing unqualified
 * Kill() reaches this class's own slot-31 override. */
void daObjIceBoard_c::OnHitByMegaChar(Player &player) {
    player.IncMegaKillCount();
    Kill();
}

// @symbol _ZN15daObjIceBoard_cD1Ev
// @symbol _ZN15daObjIceBoard_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjIceBoard_c.h and declared LAST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it last, with the factory's `new` instantiating the class, is
 * what emits the used D1/D0 pair in cartridge order and keeps this TU the
 * vtable's home.
 *
 * Both bodies are short because the chain is short and this class adds no
 * member of its own: this class's vptr store, then dBgActor_c's -- inlined,
 * its destructor is defined in its class body -- then dBgActor_c's dExtSimpleModel_c and
 * dBgW_KcMbg, then dActor_c. D0's trailing deallocation is the inherited
 * inline operator delete, which is why nothing here names a heap.
 */

/* Static-init globals (was the handwritten __sinit_ov018_02112e00 shard).
 * Definition order is the retail initializer construction order. */
IceBoardModelFile daObjIceBoard_c_ModelFile(ASSET_OBJ_ICE_BOARD_BMD);
IceBoardCollisionFilePtr daObjIceBoard_c_ClsnFile(ASSET_OBJ_ICE_BOARD_KCL);
