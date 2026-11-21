//cpp
/* Production translation unit for ov022/daObjFl_Ring_c, hand-curated.
 * 9 function(s), .text 0x021111a0..0x021115a8.
 *
 * FL_RING, the fire ring of Lethal Lava Land (ov022): a dBgActor_c that turns
 * about Y at a rate kept in its own Z channel (mPrevAngle.z). It starts at
 * -0x100 per frame (or the placement's Z angle); a trigger
 * slows it to 0 and holds it there while a 0x96 frame cooldown runs, during which it spawns actor 0xf3 (OBJ_VOLCANO_CANNON,
 * daObj_volcanoCannon_c) whenever it is on screen and its spawn guard has run
 * down. Then the rate returns to -0x100.
 *
 * NAME: daObjFl_Ring_c is the cartridge's RTTI spelling -- _ZTS at ov022
 * 0x02113ce0 is the byte string "14daObjFl_Ring_c", and _ZTI at 0x02113cd4
 * reads [__si_class_type_info+8, that string, _ZTI10dBgActor_c].
 *
 * The destructor is the key function, so this TU also emits _ZTV14daObjFl_Ring_c,
 * _ZTI14daObjFl_Ring_c and _ZTS14daObjFl_Ring_c as vague linkage, alongside the
 * inherited bases' RTTI records. Every one of them has a configured ROM home,
 * so all of them license as deadstrip-data and the object isolates cleanly. The destructor is declared and defined inline and empty in the class
 * header (see that file for why), so there is no destructor text here.
 *
 * Function order is the reverse of the ROM's: mwccarm 2004/b56 emits one .text
 * section per function in the reverse of source order, so the highest-address
 * ROM function is written first. Do not reorder. The compiler places the D1/D0
 * pair on its own.
 *
 * Notes:
 * - The three pointers this class's file table holds -- the ov022 .bss
 *   SharedFilePtrs at 0x02114500 and 0x02114508, and the ov064 CLPS block at
 *   0x0211bbac -- are unnamed rows in their modules' data. The two handles
 *   are this TU's own .bss, defined at the bottom; the CLPS block is
 *   ov064's.
 * - The run ends with the trigger helper OnStoodOn, the collision callback
 *   StoodOnCallback that InitResources installs, and the registry factory
 *   daObjFl_Ring_c_classInit.
 * - g_profile_FL_RING, the registry row that names that factory, is ov022
 *   .data at 0x02113cf4 and lives outside this TU.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjFl_Ring_c_AssetId { ASSET_FL_RING_BMD = 0x060e, ASSET_FL_RING_KCL = 0x060f };

#include "SpawnInfo.h"
#include "daObjFl_Ring_c.h"
#include "SharedFilePtr.h"
#include "daObj_volcanoCannon_c.h"
#include "Sound.h"
#include "math/CalcLib.h"

/* This class's own file table, ov022 .data at 0x02113cc8: the model and
 * collision SharedFilePtrs (ov022 .bss 0x02114500 / 0x02114508, defined at
 * the bottom of this file) and the CLPS block SetFile is handed (ov064
 * 0x0211bbac). All three targets are unnamed rows in their modules' symbol
 * tables, so the table keeps its address-derived name. */
struct daObjFl_Ring_c_Files {
    SharedFilePtr *mModelFile;
    SharedFilePtr *mClsnFile;
    CLPS_Block *mClps;
};

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct RingModelFilePtr : SharedFilePtr {
    RingModelFilePtr(u32 fileID);
    ~RingModelFilePtr();
};

struct RingCollisionFilePtr : SharedFilePtr {
    RingCollisionFilePtr(u32 fileID);
    ~RingCollisionFilePtr();
};

extern daObjFl_Ring_c_Files data_ov022_02113cc8;
extern RingModelFilePtr gFlRingModelFile;
extern RingCollisionFilePtr gFlRingCollisionFile;

extern "C" {
/* local extern: d_bg_w.cpp defines these and no header declares them. */
}

/* dActor_c::mFlags bit 0x08 is written by the framework while the actor is
 * off screen. This int-returning predicate is what makes the ROM
 * materialize 0/1 before testing it. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

/* Reconstructed source-style name: SM64DS proves daObjFl_Ring_c through RTTI,
 * allocation size, vtable identity, and the FL_RING registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: VolcanoRing_Spawn.
 *
 * `new daObjFl_Ring_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x328), dBgActor_c's base constructor, then the
 * vptr store. */
daObjFl_Ring_c *daObjFl_Ring_c_classInit(void) {
    return new daObjFl_Ring_c;
}

/* The collision callback InitResources installs in mMeshCollider's slot. The
 * slot passes three arguments; the trigger helper wants the last two.
 * long_calls keeps the pooled absolute tail call. */
#pragma push
#pragma long_calls on
void daObjFl_Ring_c::StoodOnCallback(dBgW *collider, daObjFl_Ring_c *self, dActor_c *other) {
    self->OnStoodOn(other);
}
#pragma pop

/* Something touched the ring: raise mTriggered, which the next Behavior
 * reads once and clears. */
void daObjFl_Ring_c::OnStoodOn(dActor_c *other) {
    mTriggered = 1;
}

s32 daObjFl_Ring_c::InitResources() {
    BMD_File *f = dExtSimpleModel_c::LoadFile(*data_ov022_02113cc8.mModelFile);
    mModel.SetFile(f, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *k = dBgW_Kc::LoadFile(*data_ov022_02113cc8.mClsnFile);
    mMeshCollider.SetFile(k, mClsnMat, 0x199, mAngle.y, *data_ov022_02113cc8.mClps);

    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);
    mMeshCollider.SetStoodOnCallback((::StoodOnCallback)StoodOnCallback);

    /* The ring's own Z channel carries its spin rate, not an angle: it rests
       at -0x100 per frame unless the map row gave it one. */
    mPrevAngle.z = -0x100;
    if (mAngle.z != 0)
        mPrevAngle.z = mAngle.z;
    return 1;
}

s32 daObjFl_Ring_c::Behavior() {
    switch (mState) {
    case 0:
        /* Turning: once the cooldown is spent and mTriggered is set, slow
           the spin to 0. */
        if (DecIfAbove0_Short(&mCooldown) != 0)
            break;
        if (mTriggered == 0)
            break;
        if (ApproachAngle(&mPrevAngle.z, 0, 0xf, 0x30, 2) != 0)
            break;
        mCooldown = 150;
        mState = 1;
        Sound::PlayBank3(0xc, mCamSpacePos);
        break;

    case 1:
        /* Stopped: spawn while the cooldown runs, then spin back up. */
        if (DecIfAbove0_Short(&mCooldown) == 0) {
            if (ApproachAngle(&mPrevAngle.z, -0x100, 0xf, 0x30, 2) != 0)
                break;
            mCooldown = 150;
            mState = 0;
            break;
        }
        if (IsOffScreen(this))
            break;
        if (DecIfAbove0_Byte(&mSpawnGuard) != 0)
            break;
        {
            /* A Vector3 v emits ~Vector3, which drags dBgActor_c's D2 and this class's RTTI into the TU (measured). */
            int v[3];
            daObj_volcanoCannon_c *s;
            v[0] = mPos.x;
            v[1] = mPos.y;
            v[2] = mPos.z;
            v[1] = mPos.y + 0x1f4000;
            s = (daObj_volcanoCannon_c *)dActor_c::Spawn(0xf3, 0, *(Vector3 *)v, 0, mAreaId, -1);
            s->mSpawner = this;
            s->mKillPosY = mPos.y;
            mSpawnCount++;
            mSpawnGuard = 2;
        }
        break;
    }

    mPrevAngle.y += mPrevAngle.z;
    mAngle.y = mPrevAngle.y;

    mMeshCollider.SetClipRadius(0x780000);
    mMeshCollider.SetClipOffsetY(0x1000);
    UpdateModelPosAndRotY();
    if (IsClsnInRange(0x780000, 0x1000) != 0)
        UpdateClsnPosAndRot();

    mTriggered = 0;
    return 1;
}

s32 daObjFl_Ring_c::Render() {
    mModel.Render(0);
    return 1;
}

s32 daObjFl_Ring_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov022_02113cc8.mModelFile->Release();
    data_ov022_02113cc8.mClsnFile->Release();
    return 1;
}

/* Source order is construction order: model file 1550, collision file 1551.
 * __sinit_d_a_obj_fl_ring.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
RingModelFilePtr gFlRingModelFile(ASSET_FL_RING_BMD);
RingCollisionFilePtr gFlRingCollisionFile(ASSET_FL_RING_KCL);

extern "C" SpawnInfo<daObjFl_Ring_c> g_profile_FL_RING = {
    daObjFl_Ring_c_classInit, 71, 279, 0x00000002, 0x00000000, 0x00780000, 0x02000000, 0x00000000};

/* Course .data that ends at this class's typeinfo. The file table
 * data_ov022_02113cc8 is the last three words. A named section keeps
 * it ahead of the compiler's .data (typeinfo, profile, vtable).
 * Interior labels stay in symbols.txt. */
extern "C" u32 data_ov022_02113360[];
extern "C" u32 data_ov022_02113318[];
extern "C" u32 data_ov022_02113160[];
extern "C" u32 data_ov022_021131f8[];
extern "C" u32 data_ov022_02112954[];
extern "C" const u32 data_ov022_02112950[];
extern "C" u32 data_ov022_02112974[];
extern "C" u32 data_ov022_02112958[];
extern "C" u32 data_ov022_02113298[];
extern "C" u32 data_ov022_02113148[];
extern "C" u32 data_ov022_0211317c[];
extern "C" u32 data_ov022_021136f8[];
extern "C" u32 data_ov022_021133a8[];
extern "C" u32 data_ov022_021131a8[];
extern "C" u32 data_ov022_021131b8[];
extern "C" u32 data_ov022_02113150[];
extern "C" u32 data_ov022_021131c8[];
extern "C" u32 data_ov022_02113158[];
extern "C" u32 data_ov022_021131d8[];
extern "C" u32 data_ov022_02113260[];
extern "C" u32 data_ov022_02113210[];
extern "C" u32 data_ov022_02113170[];
extern "C" u32 data_ov022_02113244[];
extern "C" u32 data_ov022_021132d8[];
extern "C" u32 data_ov022_021131e8[];
extern "C" u32 data_ov022_02113198[];
extern "C" u32 data_ov022_02113144[];
extern "C" u32 data_ov022_02113188[];
extern "C" u32 data_ov064_0211bbac[];
extern "C" u32 data_ov022_02113140[] = {
    0x00000741,
    0x03030304,
    0x00000009,
    (u32)data_ov022_02113360,
    0x0000ea3d,
    0x00000000,
    0x0000ec3d,
    0x00000000,
    0x00000009,
    (u32)data_ov022_02113318,
    0x61676f79,
    0x0030306e,
    0xff020000,
    0x00027fff,
    0x7fffff02,
    (u32)data_ov022_02113160,
    (u32)data_ov022_021131f8,
    0x04e20000,
    0x00c80000,
    0xc00000fa,
    0x000f0000,
    0x000044ff,
    0x0032f3cb,
    0x00c00d2f,
    0x0096e98a,
    0x00401964,
    0xf6a00014,
    0x173e0096,
    0xe0000003,
    0xffff0000,
    0x0352001f,
    0x173e0190,
    0x00000000,
    0xff030000,
    0x0352001f,
    0x173e0190,
    0x00000000,
    0xff030000,
    0x0352001f,
    0x173e0190,
    0x00000000,
    0xff030000,
    0x0032f3cb,
    0x10000d2f,
    0x0096e98a,
    0x00001964,
    0x000000c8,
    (u32)data_ov022_02112954,
    (u32)data_ov022_02112950,
    (u32)data_ov022_02112974,
    0x00000001,
    (u32)data_ov022_02112958,
    0x0000044c,
    0x0beaee6c,
    0xee6c0000,
    0x00001b58,
    0x1b580947,
    0x19960000,
    (u32)data_ov022_02113298,
    (u32)data_ov022_02113148,
    0x07400742,
    0x07440743,
    (u32)data_ov022_0211317c,
    0x06400001,
    0x0000007f,
    0xefcaff02,
    0x1a9002bc,
    0x60000000,
    0xff070000,
    0x00000000,
    0x0000decc,
    0x00000000,
    0xe98ace3d,
    0x1a5e0190,
    0xe6ce0e3e,
    0xe69c00fa,
    0x17d40e3e,
    0xe73202ee,
    0xf4ac0e3e,
    0xf83000fa,
    0xe69c0e3e,
    0x116200fa,
    0x19640e3e,
    0xf66e00af,
    0xe82cee3c,
    0x1d4c01c2,
    0x53504c43,
    0x00070008,
    0x00048fc0,
    0x000000ff,
    0x0004afc0,
    0x000000ff,
    0x00048fc4,
    0x000000ff,
    0x00048fc5,
    0x000000ff,
    0x000c8fc0,
    0x000000ff,
    0x01048fc0,
    0x000000ff,
    0x018c8fcc,
    0x000000ff,
    0xf15a0000,
    0x1806073a,
    0x40000000,
    0x05000000,
    0xf1f00000,
    0x17700096,
    0x40000000,
    0x00000000,
    0xf1f00000,
    0x186a0096,
    0x40000000,
    0x00000000,
    0xf0c40000,
    0x186a0096,
    0x40000000,
    0x00000000,
    0x00005d00,
    (u32)data_ov022_021136f8,
    0x00006a05,
    (u32)data_ov022_021133a8,
    0x00000160,
    (u32)data_ov022_021131a8,
    0x000001a0,
    (u32)data_ov022_021131b8,
    0x000001a5,
    (u32)data_ov022_02113150,
    0x000001c0,
    (u32)data_ov022_021131c8,
    0x000001c5,
    (u32)data_ov022_02113158,
    0x000001e0,
    (u32)data_ov022_021131d8,
    0x000007e5,
    (u32)data_ov022_02113260,
    0x00000402,
    (u32)data_ov022_02113210,
    0x00000203,
    (u32)data_ov022_02113170,
    0x00000204,
    (u32)data_ov022_02113244,
    0x0000010b,
    (u32)data_ov022_02113140,
    0x00000401,
    (u32)data_ov022_021132d8,
    0x00000206,
    (u32)data_ov022_021131e8,
    0x00000207,
    (u32)data_ov022_02113198,
    0x0000010e,
    (u32)data_ov022_02113144,
    0x0000010a,
    (u32)data_ov022_02113188,
    0xe570e225,
    0x0ed80024,
    0xe606e225,
    0x11c600ba,
    0xe958e225,
    0x12c00025,
    0xea07e225,
    0x0a5a0096,
    0xea07e225,
    0x0b090096,
    0xea07e225,
    0x0bb80096,
    0xea07e225,
    0x0c670096,
    0xea07e225,
    0x0d160096,
    0xf894e225,
    0x0d7a0019,
    0xf894e225,
    0x0e100019,
    0xf894e225,
    0x0ea60019,
    0xf34ee225,
    0xfa24003c,
    0xf380e225,
    0xf8f80086,
    0xf574e225,
    0xf7360079,
    0xf6d2e225,
    0xf736003d,
    0xf7fee225,
    0xf0600096,
    0xf894e225,
    0xf0600096,
    0xf92ae225,
    0xf0600096,
    0xf9c0e225,
    0xf0600096,
    0xfa56e225,
    0xf0600096,
    0x0ea6e225,
    0xe958012c,
    0x0ddee225,
    0xe9bc012c,
    0x0d7ae225,
    0xea84012c,
    0x0ddee225,
    0xeb4c012c,
    0x0ea6e225,
    0xebb0012c,
    0x0f6ee225,
    0xeb4c012c,
    0x0fd2e225,
    0xea84012c,
    0x0f6ee225,
    0xe9bc012c,
    0x16f3e225,
    0xe5bb01c1,
    0x1789e225,
    0xe6510239,
    0x181fe225,
    0xe6e7026f,
    0x18b5e225,
    0xe77d0237,
    0x194be225,
    0xe81301b5,
    0x1964e225,
    0xf31c0096,
    0x189ce225,
    0xf3800096,
    0x1838e225,
    0xf4480096,
    0x189ce225,
    0xf5100096,
    0x1964e225,
    0xf5740096,
    0x1a2ce225,
    0xf5100096,
    0x1a90e225,
    0xf4480096,
    0x1a2ce225,
    0xf3800096,
    0x0cb2e225,
    0xf2220096,
    0x0cb2e225,
    0xf5d80096,
    0x109ae225,
    0xf2220096,
    0x109ae225,
    0xf5d80096,
    0x092ee225,
    0x01f40096,
    0x09f6e225,
    0x01f40096,
    0x0abee225,
    0x01f40096,
    0x0b86e225,
    0x01f40096,
    0x0c4ee225,
    0x01f40096,
    0x0000e225,
    0x04e200af,
    0xfbb4e225,
    0x025800af,
    0x044ce225,
    0x025800af,
    0x1b58e225,
    0x044c012c,
    0x1a90e225,
    0x04b0012c,
    0x1a2ce225,
    0x0578012c,
    0x1a90e225,
    0x0640012c,
    0x1b58e225,
    0x06a4012c,
    0x1c20e225,
    0x0640012c,
    0x1c84e225,
    0x0578012c,
    0x1c20e225,
    0x04b0012c,
    0x0000423f,
    0xe5a20352,
    0x0ea6443f,
    0xea840190,
    0x0bb8083d,
    0x1e0701c2,
    0xef5a063f,
    0x00ed00e1,
    0xe9a86226,
    0x011300c8,
    0xec786226,
    0x03e80096,
    0xef486226,
    0x03e800c8,
    0xec786226,
    0x06bd00c8,
    0xef486226,
    0x06bd00c8,
    0xe9a46226,
    0x06b700c8,
    0xe9af6226,
    0x03ed00c8,
    0xec6d6226,
    0x010200c8,
    0x128ee225,
    0xfcae0096,
    0x0000e225,
    0xeda4012c,
    0x0000e225,
    0x128e0096,
    0x0000e225,
    0x13880096,
    0xfe0ce225,
    0xef66012c,
    0x0000e225,
    0x14820096,
    0x0000e225,
    0xfb1e00af,
    0x0000e225,
    0xf128012c,
    0x044ce225,
    0xfda800af,
    0x128ee225,
    0xfe3e0096,
    0x01f4e225,
    0xef66012c,
    0x128ee225,
    0xfd760096,
    0xfbb4e225,
    0xfda800af,
    0xec78e225,
    0xee08015e,
    0xea84e225,
    0xef66015e,
    0xea84e225,
    0xf15a015e,
    0xec78e225,
    0xf2b8015e,
    0xee6ce225,
    0xf15a015e,
    0xee6ce225,
    0xef66015e,
    0x0802e225,
    0xfed400c8,
    0x06a4e225,
    0x06a400c8,
    0x0384e225,
    0x083400c8,
    0x06a4e225,
    0x09c400c8,
    0xfc180227,
    0x1a900000,
    0xf8f80227,
    0x19960000,
    0xf8940227,
    0x16760000,
    0xf8940227,
    0x13560000,
    0xf8940227,
    0x10fe0000,
    0xf8940227,
    0x0b220000,
    0xf8620227,
    0x08020000,
    0xf79a0227,
    0x051401f4,
    0xff380227,
    0x1a900000,
    0x01f40227,
    0x19fa0000,
    0x17700082,
    0x16f3012c,
    0x40000000,
    0xffff0000,
    0x00000083,
    0x00000000,
    0x00000000,
    0xffff0000,
    0x04e200f6,
    0xee6c0000,
    0x00000000,
    0x01000000,
    0x1b5800f6,
    0x09f60000,
    0x00000000,
    0x00010000,
    0xe98a001e,
    0x18380190,
    0x00000000,
    0xff050000,
    0xe66a0031,
    0xe5d4012c,
    0x00000000,
    0x00f30000,
    0xf0f6004c,
    0x170c0096,
    0x0000ffff,
    0x07420000,
    0xf1be004c,
    0x170c0096,
    0x0000ffff,
    0x05140000,
    0x041a004c,
    0x16440096,
    0x0000ffff,
    0x073b0000,
    0xf7040031,
    0x0384026c,
    0x00000000,
    0x00f30000,
    0xf4480031,
    0xf7cc0093,
    0x00000000,
    0x00f30000,
    0x00000031,
    0xe3e00000,
    0x00000000,
    0x00f30000,
    0x186a0031,
    0xe69c028a,
    0x00000000,
    0x00f30000,
    0x19640096,
    0xf448010e,
    0x00000000,
    0xffff0000,
    0x1c8400f0,
    0x044c012c,
    0x00000000,
    0xffff0000,
    0x1a2c00f0,
    0x06a4012c,
    0x00000000,
    0xffff0000,
    0x000000f0,
    0x0e10012c,
    0x00000000,
    0xffff0000,
    0xf3cb0096,
    0x0d2f015e,
    0x00000000,
    0xffff0000,
    0xed0e00f0,
    0xf6d2012c,
    0x00000000,
    0xffff0000,
    0xec4600f0,
    0xf9c0012c,
    0x00000000,
    0xffff0000,
    0x00000031,
    0xf830012c,
    0x00000000,
    0x00f30000,
    0x10040098,
    0xf8940096,
    0x00000000,
    0xffff0000,
    0x03840084,
    0x0834ff6a,
    0x00000000,
    0xffff0000,
    0x06a40084,
    0x06a4ff6a,
    0x00000000,
    0xffff0000,
    0x06a40084,
    0x09c4ff6a,
    0x00000000,
    0xffff0000,
    0x08020084,
    0xfed4ff6a,
    0x00000000,
    0xffff0000,
    0xfb1e0085,
    0x0dea012c,
    0x00000000,
    0xffff0000,
    0xf63c0085,
    0x0dea012c,
    0x80000000,
    0xffff0000,
    0x1b580086,
    0x1bd50000,
    0x00000000,
    0xffff0000,
    0x0ea80087,
    0xf3cb0000,
    0x00000000,
    0xffff0000,
    0xea070089,
    0x0bb80000,
    0x00000000,
    0xffff0000,
    0x0abe0088,
    0x01f40000,
    0x40000000,
    0xffff0000,
    0x0f230088,
    0x055f0000,
    0x40000000,
    0xffff0000,
    0xf92a0088,
    0xf0600000,
    0x40000000,
    0xffff0000,
    0xeb8800ff,
    0x01180064,
    0x00000000,
    0xff000000,
    0xed6800ff,
    0x01180064,
    0x00000000,
    0xff010000,
    0xe9a800ff,
    0x02f80064,
    0x00000000,
    0xff020000,
    0xeb8800ff,
    0x02f80064,
    0x00000000,
    0xff030000,
    0xed6800ff,
    0x02f80064,
    0x00000000,
    0xff040000,
    0xef4800ff,
    0x02f80064,
    0x00000000,
    0xff050000,
    0xe9a800ff,
    0x04d80064,
    0x00000000,
    0xff060000,
    0xeb8800ff,
    0x04d80064,
    0x00000000,
    0xff070000,
    0xed6800ff,
    0x04d80064,
    0x00000000,
    0xff080000,
    0xef4800ff,
    0x04d80064,
    0x00000000,
    0xff090000,
    0xe9a800ff,
    0x06b80064,
    0x00000000,
    0xff0a0000,
    0xeb8800ff,
    0x06b80064,
    0x00000000,
    0xff0b0000,
    0xed6800ff,
    0x06b80064,
    0x00000000,
    0xff0c0000,
    0xef4800ff,
    0x06b80064,
    0x00000000,
    0xff0d0000,
    0xec78008a,
    0xf0600000,
    0x00000000,
    0xffff0000,
    0xec780100,
    0x03e800fa,
    0x00000000,
    0xffff0000,
    0x000000f1,
    0xef66012c,
    0x00000000,
    0xff010000,
    0xec78008b,
    0xf060012c,
    0x00000000,
    0xffff0000,
    0xebb000fe,
    0xfce0012c,
    0x20000000,
    0x10040000,
    0xf254008c,
    0xf0600000,
    0x00000000,
    0xffff0000,
    0xf380008c,
    0xf0600000,
    0x00001000,
    0xffff0000,
    0xf4ac008c,
    0xf0600000,
    0x00002000,
    0xffff0000,
    0x0000008d,
    0xe89001c2,
    0x00000000,
    0xff010000,
    0x0000008d,
    0xe90d01c2,
    0x00000000,
    0xff010000,
    0x0000008d,
    0xe98a01c2,
    0x00000000,
    0xff010000,
    0x0000008d,
    0xea07012c,
    0x00000000,
    0xff010000,
    0x0000008d,
    0xea84012c,
    0x00000000,
    0xff010000,
    0x0000008d,
    0xeb01012c,
    0x00000000,
    0xff010000,
    0x0000008d,
    0xeb7e0096,
    0x00000000,
    0xff010000,
    0x0000008d,
    0xebfb0096,
    0x00000000,
    0xff010000,
    0x0000008d,
    0xec780096,
    0x00000000,
    0xff010000,
    0x0000010a,
    0xf8ad00aa,
    0x00000000,
    0xffff0000,
    0x055f010a,
    0x0e290000,
    0x0000c000,
    0xffff0000,
    0x1af4010a,
    0x0dac0000,
    0x0000c000,
    0xffff0000,
    0x1bbc010a,
    0x13880000,
    0x0000c000,
    0xffff0000,
    0x1af4010a,
    0x157c0000,
    0x0000c000,
    0xffff0000,
    0x0ea600f1,
    0xea8403e8,
    0x00000000,
    0x01020000,
    0xfe0c0133,
    0x10fe0000,
    0x40000000,
    0xffff0000,
    0x01f40133,
    0x12f20000,
    0xc0000000,
    0xffff0000,
    0xe7fa0133,
    0x0a280000,
    0x40000000,
    0xffff0000,
    0xf4ac0133,
    0xeda40000,
    0x00000000,
    0xffff0000,
    0x12c00133,
    0xfaec0000,
    0x40000000,
    0xffff0000,
    0x14e60133,
    0xf8620000,
    0xe0000000,
    0xffff0000,
    0x16760133,
    0xfea20000,
    0xd5560000,
    0xffff0000,
    0x14b4001e,
    0xfaba0190,
    0x00000000,
    0xff060000,
    0x0000008f,
    0x00000000,
    0x00000000,
    0xffff0000,
    0xf31c00fe,
    0x173e0096,
    0x0aaa0000,
    0x01040000,
    0xfd4400fe,
    0x0d48012c,
    0x38e30000,
    0x00040000,
    0x009600fe,
    0x11940096,
    0xf5560000,
    0x02040000,
    0x16a800fe,
    0x06a4012c,
    0xe0000000,
    0x12040000,
    0x0514011c,
    0x17a20096,
    0x00000000,
    0xffff0000,
    0xf1be00fe,
    0x0abe012c,
    0x20000000,
    0x11040000,
    0xe7bc001e,
    0x00e80190,
    0x00000000,
    0xff060000,
    0x05140131,
    0x17a20096,
    0x00000000,
    0x1e000000,
    0xec780031,
    0xf128012c,
    0x00000000,
    0x00f80000,
    0xec780031,
    0xef98012c,
    0x00000000,
    0x00f80000,
    0xebb00031,
    0xf060012c,
    0x00000000,
    0x00f80000,
    0xed400031,
    0xf060012c,
    0x00000000,
    0x00f80000,
    0xec780031,
    0xf06001f4,
    0x00000000,
    0x00470000,
    (u32)&gFlRingModelFile,
    (u32)&gFlRingCollisionFile,
    (u32)data_ov064_0211bbac,
};
