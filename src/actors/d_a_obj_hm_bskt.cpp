//cpp
/* ov030 / daObjHmBskt_c -- the Hazy Maze ukiki cage (registry profile
 * HM_BASKET, RTTI name at ov030:0x02115940).
 *
 * One translation unit for the whole ROM run 0x021111a0..0x0211155c: the four
 * class methods, the destructor pair, the file-local landing callback between
 * them, and the factory, plus the .data run 0x02115934..0x021159f4 that holds
 * the class's RTTI chain, its profile descriptor and its vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order.
 *
 * .data objects are laid out by mwld in ASCENDING SIZE, which is what puts
 * _ZTI (0xc) first, _ZTS (0x10) next, the profile (0x1c) after it and the
 * 0x88 vtable object last -- the ROM's own order for this run.
 *
 * The destructor is NOT written out of line below. It is defined in the class
 * body in include/daObjHmBskt_c.h, which is what makes mwccarm emit the
 * cartridge's D1/D0 pair, in the cartridge's order, and no D2 at all.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjHmBskt_c.h"
#include "SharedFilePtr.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern "C" int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_( dBgActor_c *self, s32 range, s32 offsetY);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
Fix12i scale, s16 angle, CLPS_Block *clps);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
dBgCh_Actr *self, dActor_c *actor, Fix12i radius, Fix12i height,
Vector3_16 *a, Vector3_16 *b);
extern SharedFilePtr data_ov030_02115c88;
extern SharedFilePtr data_ov030_02115c80;
extern CLPS_Block data_ov030_02114ee4;

/* Defined below, in its own C-linkage block; the char* spelling is
 * decl_common.h's and the legacy definition's. */
int func_ov030_0211124c(char *c, char *mc);

/* Factory-side imports. */
void *_ZN7fBase_cnwEj(unsigned int);
void _ZN10dBgActor_cC2Ev(void *);
void _ZN10dBgCh_ActrC1Ev(void *);
extern int _ZTV13daObjHmBskt_c[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjHmBskt_c_classInit, 0x02111524, size 0x38            */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObjHmBskt_c through RTTI,
 * allocation size, vtable identity, and the HM_BASKET registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: UkikiCage_Spawn. */
// @symbol daObjHmBskt_c_classInit
extern "C" int *daObjHmBskt_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1248);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's
         * symbol is the object start at 0x0211596c and +8 is what reaches the
         * 0x02115974 address point the ROM stores. The addend-0 spelling was
         * right only while the class merely IMPORTED its vtable. */
        p[0] = (int)&_ZTV13daObjHmBskt_c[2];
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x320);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* g_profile_HM_BASKET, 0x02115950, size 0x1c                                 */
/* -------------------------------------------------------------------------- */
struct HmBsktSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x0067 */
    s16 drawOrder;                  /* 0x0080 */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char HmBsktSpawnInfo_size_must_be_0x1c[
    sizeof(HmBsktSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_HM_BASKET
extern "C" HmBsktSpawnInfo g_profile_HM_BASKET = {
    daObjHmBskt_c_classInit, 0x0067, 0x0080, 0x00000003,
    0x000be000, 0x000be000, 0x01000000, 0x01000000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN13daObjHmBskt_c13InitResourcesEv, 0x02111410, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjHmBskt_c13InitResourcesEv
s32 daObjHmBskt_c::InitResources()
{
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov030_02115c88);
    mModel.SetFile(modelFile, 1, -1);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    mHorzSpeed = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *clsnFile = (KCL_File *)dBgW_Kc::LoadFile(data_ov030_02115c80);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, clsnFile, &mClsnMat, 0x199, mAngleY,
        &data_ov030_02114ee4);

    Vector3 starPos;
    starPos.x = mPosX;
    starPos.y = mPosY;
    starPos.z = mPosZ;
    starPos.y += 0x3c000;
    mStarActor = dActor_c::Spawn(
        0xb2, (u8)(param1 & 0xf) | 0x50, starPos, 0, mAreaId, -1);

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN13daObjHmBskt_c8BehaviorEv, 0x02111384, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjHmBskt_c8BehaviorEv
/* The class identity and slot are ROM-proven by _ZTV13daObjHmBskt_c. The two
 * integer arguments below really are Fix12<int> by value, which is a known
 * mwccarm language-mode wall, so that callee remains the narrow C-linkage
 * boundary. This method itself is ordinary typed C++ throughout. */
s32 daObjHmBskt_c::Behavior()
{
    volatile int dummy[4];
    (void)&dummy;

    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);
    mAngleY = mAngleY + mHorzSpeed;

    if (mVertAccel != 0) {
        mFlags &= ~1;
        UpdatePos(0);
        func_ov030_0211124c((char *)this, (char *)&mWithMeshClsn);

        s32 y = mPosY;
        s32 z = mPosZ;
        dActor_c *star = mStarActor;
        s32 x = mPosX;
        star->mPosX = x;
        star->mPosY = y + 0x3c000;
        star->mPosZ = z;
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN13daObjHmBskt_c6RenderEv, 0x02111350, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjHmBskt_c6RenderEv
/* daObjHmBskt_c::Render -- vtable slot 9, ov030 0x02111350, a real daObjHmBskt_c::Render()
 * method: refresh the model and collision transforms (both inherited dBgActor_c
 * methods), then draw the Model sub-object at +0xd4 (dBgActor_c::mModel) through
 * Model::Render (vtable slot 5). The member call keeps its indirect vtable
 * dispatch -- mwccarm does not devirtualize the embedded object -- so no
 * shadow-vtable struct or extern-C shim is needed. */
s32 daObjHmBskt_c::Render() {
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN13daObjHmBskt_c16CleanupResourcesEv, 0x0211130c, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjHmBskt_c16CleanupResourcesEv
/* daObjHmBskt_c::CleanupResources -- vtable slot 3, ov030 0x0211130c. Attributed by
 * the vtable: _ZTV13daObjHmBskt_c carries this address at slot 3, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043bf0. Takes the inherited
 * mesh collider out of the collision world, then releases two shared files. */
s32 daObjHmBskt_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    _ZN13SharedFilePtr7ReleaseEv(&data_ov030_02115c88);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov030_02115c80);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov030_0211124c, 0x0211124c, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_0211124c
/* recovered: shared common types */
#include "common.h"
extern "C" {
extern void dBgCh_Actr_UpdateContinuous_Veneer(void*);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern void _ZN7fBase_c18MarkForDestructionEv(void*);
extern void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, void*, void*, int, int);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);

int func_ov030_0211124c(char *c, char *mc){
  dBgCh_Actr_UpdateContinuous_Veneer(mc);
  int g = _ZNK10dBgCh_Actr10IsOnGroundEv(mc);
  if(!g) return g;
  _ZN7fBase_c18MarkForDestructionEv(*(void**)(c+0x4dc));
  int px = *(int*)(c+0x5c);
  int pz = *(int*)(c+0x64);
  int py = *(int*)(c+0x60) + 0x96000;
  Vector3 p = { px, py, pz };
  _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, (unsigned char)((*(unsigned int*)(c+8))&0xf) | 0x20, &p, 0, *(signed char*)(c+0xcc), -1);
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  _ZN7fBase_c18MarkForDestructionEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN13daObjHmBskt_cD1Ev  0x021111a0  size 0x4c  (complete-object dtor)    */
/*   _ZN13daObjHmBskt_cD0Ev  0x021111ec  size 0x60  (deleting destructor)     */
/* -------------------------------------------------------------------------- */
/* Defined in include/daObjHmBskt_c.h's class body, and deliberately NOT
 * repeated out of line here: out of line mwccarm emits D2, D0, D1, and the
 * cartridge holds D1, D0 with no D2 at all.
 *
 * The bodies are all automatic: this class's vptr, then its own dBgCh_Actr at
 * 0x320 -- the only field it declares -- then dBgActor_c's vptr (inlined,
 * because dBgActor_c defines its destructor in its class body), then
 * dBgActor_c's dBgW_KcMbg (0x124) and Model (0xd4) in reverse declaration
 * order, then dActor_c. D0 additionally returns the object to its heap
 * through dActor_c's inline operator delete. */

// @symbol _ZN13daObjHmBskt_cD0Ev
// @symbol _ZN13daObjHmBskt_cD1Ev
