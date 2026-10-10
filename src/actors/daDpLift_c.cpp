//cpp
/* daDpLift_c: DP_LIFT (actor 88), a dBgActor_c. It idles until mHadClsn
 * is set, shakes, sinks through ten markers spaced 0x1cc000 apart, and
 * settles at Y 0x80000 with a second short shake. Render draws mModel
 * and one mModel2 copy per marker not yet passed.
 *
 * Nine functions, ov025 .text 0x021120e4..0x02112654: six methods, then
 * the mesh-collider callback pair that sets mHadClsn and the registry
 * factory daDpLift_c_classInit, last. The RTTI string at ov025
 * 0x021139a0 is "10daDpLift_c".
 *
 * `#pragma defer_codegen off` is file-level and load-bearing. It emits
 * each function as it is parsed, so the out-of-line destructor comes
 * out D1 then D0, and it lets the opt_strength_reduction bracket around
 * InitResources bind. Do not move either pragma.
 *
 * Leftover:
 * - InitResources walks a copy of this by 0xc. Offsets 0x37c, 0x380 and
 *   0x384 are mMarkerPositions x, y and z. An indexed for-loop
 *   size-DIFF 0x124->0x118. An indexed do-loop size-DIFF 0x124->0x128.
 *   A Vector3* from &mMarkerPositions[0] stays 0x124 and DIFFs 5 words.
 *   A Vector3* recomputed as (Vector3 *)(ip + 0x37c) size-DIFF
 *   0x124->0x128. An s32* of mk[0], mk[1], mk[2] stays 0x124 and DIFFs
 *   24 words. That s32* plus a separate y pointer size-DIFF
 *   0x124->0x128. One store of mPosY - prod, with no reload,
 *   size-DIFF 0x124->0x118. offsetof(daDpLift_c, mMarkerPositions) does
 *   not compile ("( expected").
 * - Behavior: the marker test flipped to
 *   `mMarkerPositions[mNextMarker].y + 0x14000 >= mPosY` stays 0x210
 *   and DIFFs 10 words. Settle `mShakeTimer == 8` instead of `>= 8`
 *   stays 0x210 and DIFFs 3 words. IsClsnInRange as a Fix12<int>
 *   method size-DIFF 0x210->0x224. `Fix12<int>{0}` does not compile
 *   ("( expected"). The free call with two ints matches.
 * - Render: one Vector3 local then one Matrix4x3_FromTranslation
 *   size-DIFF 0x8c->0xa4.
 * - SetFile as a method with a Fix12<int> scale size-DIFF
 *   0x124->0x130. The free call with int 0x199 matches.
 *   func_020393d4 and func_020393c4 stay int* stores into the mesh
 *   callback slots. data_02082214 stays the unnamed sine table.
 * - The two file handles live after InitResources; the compiler's
 *   __sinit_daDpLift_c.cpp constructs them at overlay load. Their wrapper
 *   names are local -- the constructors are the ROM resource-family
 *   functions, recorded as aliases in the manifest.
 */

#include "daDpLift_c.h"
#include "SharedFilePtr.h"

#pragma defer_codegen off

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct DpLiftModelFilePtr : SharedFilePtr {
    u32 words[2];

    DpLiftModelFilePtr(u32 fileID);
    ~DpLiftModelFilePtr();
};

struct DpLiftCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    DpLiftCollisionFilePtr(u32 fileID);
    ~DpLiftCollisionFilePtr();
};

extern "C" {
/* Lift model, collision file (ov025 .bss) and marker model (ov002 .bss).
 * This TU's two handles are defined at the end of this file so the
 * constructors do not enter .text. */
extern DpLiftModelFilePtr data_ov025_02113ae0;
extern SharedFilePtr data_ov002_0210d9f0;
extern DpLiftCollisionFilePtr data_ov025_02113ad8;
extern CLPS_Block data_ov025_02112d08;

extern s16 data_02082214[];

void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);

void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const Matrix4x3 *mtx, int scale, s16 angleY, void *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);

void func_020393d4(int *p, int v);
void func_020393c4(int *p, int v);
void func_ov025_021125bc(daDpLift_c *self, dActor_c *other);
void func_ov025_021125dc(void *collider, daDpLift_c *self, dActor_c *other);
}

// @symbol _ZN10daDpLift_cD1Ev
// @symbol _ZN10daDpLift_cD0Ev
daDpLift_c::~daDpLift_c()
{
}

// @symbol _ZN10daDpLift_c16CleanupResourcesEv
s32 daDpLift_c::CleanupResources()
{
    data_ov002_0210d9f0.Release();
    data_ov025_02113ae0.Release();
    data_ov025_02113ad8.Release();
    return 1;
}

// @symbol _ZN10daDpLift_c6RenderEv
s32 daDpLift_c::Render()
{
    mModel.Render(0);
    for (int i = mNextMarker; i < 10; i++) {
        Matrix4x3_FromTranslation(&mModel2.mat4x3,
                                  mMarkerPositions[i].x >> 3,
                                  mMarkerPositions[i].y >> 3,
                                  mMarkerPositions[i].z >> 3);
        mModel2.Render(0);
    }
    return 1;
}

/* mState: idle until mHadClsn, shake while mShakeTimer counts to 8,
   sink until Y is below 0x80000, then the settling shake. */
enum {
    STATE_IDLE,
    STATE_SHAKE,
    STATE_SINK,
    STATE_SETTLE
};

// @symbol _ZN10daDpLift_c8BehaviorEv
s32 daDpLift_c::Behavior()
{
    switch (mState) {
    case STATE_IDLE:
        if (mHadClsn) {
            mState = STATE_SHAKE;
            mShakeTimer = 0;
        }
        break;
    case STATE_SHAKE: {
        s16 ang = mShakeTimer << 12;
        s32 wave = (s32)(((s64)data_02082214[((u16)ang >> 4) * 2] * 10 + 0x800) >> 12);
        mPosY = mBasePosY + wave;
        if (mShakeTimer == 8) {
            mState = STATE_SINK;
            mVertSpeed = -0xa000;
        }
        mShakeTimer++;
        break;
    }
    case STATE_SINK: {
        if (mPosY <= mMarkerPositions[mNextMarker].y + 0x14000)
            mNextMarker++;
        mPosY += mVertSpeed;
        if (mPosY < 0x80000) {
            mPosY = 0x80000;
            mState = STATE_SETTLE;
            mShakeTimer = 0;
        }
        break;
    }
    case STATE_SETTLE: {
        s16 ang = mShakeTimer << 12;
        s32 wave = (s32)(((s64)data_02082214[((u16)ang >> 4) * 2] * 10 + 0x800) >> 12);
        mPosY = wave + 0x80000;
        if (mShakeTimer >= 8) {
            mVertSpeed = 0;
            mPosY = 0x80000;
        }
        mShakeTimer++;
        break;
    }
    }
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    mHadClsn = 0;
    return 1;
}

// @symbol _ZN10daDpLift_c13InitResourcesEv
#pragma push
#pragma opt_strength_reduction off
s32 daDpLift_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov025_02113ae0), 1, -1);
    mModel2.SetFile((BMD_File *)Model::LoadFile(data_ov002_0210d9f0), 1, -1);
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov025_02113ad8);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, &data_ov025_02112d08);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    func_020393c4((int *)&mMeshCollider, (int)&func_ov025_021125dc);

    /* Ten markers under the lift, each 0x1cc000 lower than the last.
       The walk is a copy of this; see Leftover. */
    mBasePosX = mPosX;
    int n = 0;
    mBasePosY = mPosY;
    char *ip = (char *)this;
    mBasePosZ = mPosZ;
    mState = 0;
    mHadClsn = 0;
    int step = 0x1cc000;
    do {
        n++;
        *(s32 *)(ip + 0x37c) = mPosX;
        *(s32 *)(ip + 0x380) = mPosY;
        s32 drop = n * step;
        *(s32 *)(ip + 0x384) = mPosZ;
        *(s32 *)(ip + 0x380) -= drop;
        ip += 0xc;
    } while (n < 10);
    return 1;
}
#pragma pop

/* Source order is construction order: model file 1505, collision file 1506.
 * __sinit_daDpLift_c.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
DpLiftModelFilePtr data_ov025_02113ae0(1505);
DpLiftCollisionFilePtr data_ov025_02113ad8(1506);

/* Mesh-collider touch callback: remembers that the player (actor 0xbf)
   touched the lift, for Behavior's idle state. */
// @symbol func_ov025_021125bc
extern "C" void func_ov025_021125bc(daDpLift_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        self->mHadClsn = true;
}

/* The callback InitResources installs: drops the collider argument and
   forwards (lift, other). */
// @symbol func_ov025_021125dc
extern "C" void func_ov025_021125dc(void *collider, daDpLift_c *self, dActor_c *other)
{
    func_ov025_021125bc(self, other);
}

/* local extern: the factory below spells the constructor chain by hand (see
 * its comment), so it names each constructor and array callback by its
 * mangled symbol rather than through the class headers. */
extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
dBgActor_c *_ZN10dBgActor_cC2Ev(dBgActor_c *object);
Model *_ZN5ModelC1Ev(Model *object);
void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
    void (*ctor)(void *), void (*dtor)(void *));
extern void *_ZTV10daDpLift_c[];
Vector3 *_ZN7Vector3D1Ev(Vector3 *object);
void func_0203d384(void);
}

// @symbol daDpLift_c_classInit
/* Reconstructed source-style name: SM64DS proves daDpLift_c through RTTI,
 * allocation size, vtable identity, and the DP_LIFT registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: PyramidLift_Spawn.
 *
 * Spelled by hand rather than `return new daDpLift_c;`, which comes out 0x38
 * bytes for the ROM's 0x64: the ROM constructs mMarkerPositions through
 * __cxa_vec_ctor(..., func_0203d384, _ZN7Vector3D1Ev) with an empty
 * constructor function, and types.h's Vector3 declares no constructor, so the
 * implicit one never emits that call. This TU emits the vtable, whose symbol
 * names the vtable object two words ahead of the slot array, so the vptr
 * store reads &_ZTV10daDpLift_c[2]. */
extern "C" daDpLift_c *daDpLift_c_classInit(void)
{
    daDpLift_c *actor = (daDpLift_c *)_ZN7fBase_cnwEj(sizeof(daDpLift_c));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *(void **)actor = &_ZTV10daDpLift_c[2];
        _ZN5ModelC1Ev(&actor->mModel2);
        __cxa_vec_ctor(actor->mMarkerPositions, 10, sizeof(Vector3),
            (void (*)(void *))func_0203d384, (void (*)(void *))_ZN7Vector3D1Ev);
    }
    return actor;
}
