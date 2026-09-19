//cpp
// @symbol _ZN7daTrs_c13InitResourcesEv
/* daTrs_c::InitResources (slot 0): builds the variant. The 0xd2 boss branch
 * loads the big Boo's model and anims (key models and two extra anims for
 * variant 0xf, a spawned 0xd3 follower for 0xc); the 0xd1 branch loads the
 * ordinary Boo. The cap id, model selector and carried item follow from
 * param1's nibbles, star-gated variants bail out early, and the tail seeds
 * home position/angles, timers, the collision base offset and the leash.
 *
 * deslop leftovers:
 * - ModelAnim::SetAnim / dCcAcPos_c::Init / dBgCh_Actr::Init stay mangled:
 *   all three carry Fix12<int> BY VALUE, and the header member form
 *   size-DIFFs (same 6az wall as daBmb_c's InitResources).
 * - The mFlags_5d4 bit sets stay FLAGS16 shift-ORs: the member form
 *   if-converts the containing branch and addresses via the 0x500 base where
 *   the ROM has branch-outs and a literal-pool address. Bit reads are
 *   member-spelled (Render proves b1/b3; Behavior b7/b2/b0/b5 match).
 * - `*(int *)(this+8) = param1 & 0xfff` stays asymmetric on purpose: spelling
 *   both sides identically lets mwccarm CSE the field address (addlt + [r2]),
 *   one instruction the ROM does not have -- it wants [r4,#8] direct. Same
 *   lever as daKrb_c::OnTurnIntoEgg in src/actors/daKrb_c.cpp.
 * - The MulMat4x3 first argument stays a raw 0x394 load: the bone-matrix
 *   pointer inside Model.data, and ModelComponents is only forward-declared
 *   tree-wide.
 * - LoadKeyModels / LoadBlueCoinModel / NumStars / IsStarCollectedInCurLevel /
 *   IsAreaShowing / Matrix4x3_FromTranslation / MulMat4x3Mat4x3 / SubVec3 /
 *   func_02035800 have no header declarations; the TU-local externs stay.
 *   data_0209f2f8 / data_0209f264 stay raw state bytes (course id etc.), the
 *   way daBmb_c keeps them.
 */
/* common.h first so the flat Matrix4x3 (s32 m[12]) wins over math/Matrix.h's
   composite spelling. */
#include "common.h"
#include "daTrs_c.h"
#include "SharedFilePtr.h"
#include "Player.h"

extern "C" {
void LoadKeyModels(int n);
void LoadBlueCoinModel(void *self);
int NumStars(void);
int IsStarCollectedInCurLevel(int n);
void func_02035800(void *self);
void Matrix4x3_FromTranslation(Matrix4x3 *m, s32 x, s32 y, s32 z);
void MulMat4x3Mat4x3(Matrix4x3 *a, Matrix4x3 *b, Matrix4x3 *dst);
void SubVec3(Vector3 *a, Vector3 *b, Vector3 *dst);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *file, int a, int speed, unsigned int d);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *actor, void *pos, int r, int h, unsigned int e, unsigned int g);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int d);
}

extern SharedFilePtr data_ov063_0211edd4;
extern SharedFilePtr data_ov063_0211edcc;
extern SharedFilePtr data_ov063_0211ede4;
extern SharedFilePtr data_ov063_0211edf4;
extern SharedFilePtr data_ov063_0211eddc;
extern SharedFilePtr data_ov063_0211edc4;
extern SharedFilePtr data_ov063_0211edec;
extern SharedFilePtr data_ov002_0210d9c8;
extern SharedFilePtr data_ov002_0210d9f8;
extern SharedFilePtr data_ov002_0210d9b8;
extern "C" {
extern signed char data_0209f2f8;
extern unsigned char data_0209f264;
/* Per-variant tables indexed by unk_5cf: death mode, hurt damage. */
extern int data_ov063_0211e22c[];
extern int data_ov063_0211e1ec[];
extern Matrix4x3 data_020a0e68;
}

/* The flags halfword keeps its raw spelling: the member form if-converts
   the containing branch (0x5d4), off the ROM's shape. */
#define FLAGS16 (*(unsigned short *)((long long)(c + 0x5d4)))
#define FLAGS16T (*(unsigned short *)((long long)((char *)((long long)c) + 0x5d4)))

int daTrs_c::InitResources()
{
    char *c = (char *)this;
    int cond;
    int tmp598;
    dActor_c *spawned;
    Player *pp;

    *(u16 *)&mFlags_5d4 = 0;
    mSpawnedActorID = 0;
    cond = 0;
    if (actorID == 0xd2) cond = 1;
    if (cond) {
        unk_5cf = (param1 & 0xf) + 0xc;
        if (unk_5cf == 0xf) {
            LoadKeyModels(3);
            Animation::LoadFile(data_ov063_0211edd4);
            Animation::LoadFile(data_ov063_0211edcc);
        } else if (unk_5cf == 0xc) {
            mFoundActor = dActor_c::Spawn(0xd3, param1, *(const Vector3 *)&mPosX, 0, mAreaIdx, -1);
            if (mFoundActor != 0) {
                mSpawnedActorID = mFoundActor->uniqueID;
            }
            mFoundActor = 0;
        }
        Animation::LoadFile(data_ov063_0211ede4);
        mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov063_0211edf4), 1, 1);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(BCA_File **)((char *)&data_ov063_0211ede4 + 4), 0, 0x1000, 0);
        mDataIdx = 3;
        mClsnRadius = 0xc8;
        mClsnHeight = 0x104;
        mClsnZBias = -0x14000;
        tmp598 = *(volatile int *)&mClsnZBias;
        mClsnOffX = 0;
        mClsnOffY = 0;
        mClsnOffZ = tmp598;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &mClsnOffX, mClsnRadius << 0xc, mClsnHeight << 0xc, 0x200000, 0x207e0);
        if (unk_5cf != 0xf) {
            _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0xdc000, 0xdc000, 0, 0);
        } else {
            _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0xc8000, 0xb4000, 0, 0);
        }
    } else {
        unk_5cf = param1 & 0xf;
        Animation::LoadFile(data_ov063_0211eddc);
        mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov063_0211edc4), 1, 0x16);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(BCA_File **)((char *)&data_ov063_0211eddc + 4), 0, 0x1000, 0);
        mDataIdx = 1;
        if (unk_5cf == 5) {
            mClsnZBias = -0x24000;
            tmp598 = *(volatile int *)&mClsnZBias;
            mClsnOffX = 0;
            mClsnOffY = 0;
            mClsnOffZ = tmp598;
            mClsnRadius = 0x43;
            mClsnHeight = 0x5a;
        } else {
            mClsnZBias = -0x14000;
            tmp598 = *(volatile int *)&mClsnZBias;
            mClsnOffX = 0;
            mClsnOffY = 0;
            mClsnOffZ = tmp598;
            mClsnRadius = 0x4a;
            mClsnHeight = 0x64;
        }
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &mClsnOffX, mClsnRadius << 0xc, mClsnHeight << 0xc, 0x200000, 0x207e0);
        if (data_0209f2f8 == 0xc && mPosX == 0xbb8000 && mAreaId == 2) {
            FLAGS16 = (FLAGS16 & ~1) | 1;
        }
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    }

    mCarriedID = 0x187;
    if (unk_5cf == 5) {
        FLAGS16 |= 2;
        mCarriedID = 0xd4;
        Model::LoadFile(data_ov063_0211edec);
    } else if (unk_5cf == 0 || unk_5cf == 1 || unk_5cf == 2 || unk_5cf == 6 || (unsigned)(unsigned char)(unk_5cf + 0xf8) <= 3) {
        mdCcAcPos_c.vulnFlags |= 0x8000;
        FLAGS16 |= 2;
        if (unk_5cf == 6) {
            mCarriedID = 0x120;
        } else if ((unsigned)(unsigned char)(unk_5cf + 0xf6) <= 1) {
            mCarriedID = 0x121;
        } else {
            mCarriedID = 0x122;
            LoadBlueCoinModel(this);
        }
    }

    mCapId = 6;
    mCapPosX = 0;
    mCapPosY = 0x60000;
    mCapPosZ = 0;
    cond = 0;
    if (actorID == 0xd1) cond = 1;
    if (cond && unk_5cf != 8) {
        unsigned char capIdx;
        mHadBank1Cap = (param1 >> 0xc) & 0xf;
        capIdx = (param1 >> 8) & 0xf;
        AddCap(capIdx);
        if ((mCapId & 7) < 6) {
            /* int on the store side only: spelling both sides identically
               lets mwccarm CSE the field address (addlt r2,r4,#8 + [r2]),
               one instruction the ROM does not have -- it wants [r4,#8] direct.
               Same lever as daKrb_c::OnTurnIntoEgg in src/actors/daKrb_c.cpp. */
            *(int *)((char *)this + 8) = param1 & 0xfff;
        }
        if (DestroyIfCapNotNeeded() == 0) {
            return 0;
        }
    }

    if ((unsigned)(unsigned char)(unk_5cf + 0xf6) <= 1) {
        if ((unsigned)NumStars() < 3) {
            MarkForDestruction();
            return 0;
        }
        if (unk_5cf == 0xb && (unsigned)NumStars() >= 0xf) {
            MarkForDestruction();
            return 0;
        }
        if (IsStarCollectedInCurLevel(1) != 0) {
            mCarriedID = 0x120;
        }
    }

    if (mShadowModel1.InitCylinder() == 0) return 0;
    if (mShadowModel2.InitCylinder() == 0) return 0;

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomeAngleX = mAngleX;
    mHomeAngleY = mAngleY;
    mHomeAngleZ = mAngleZ;
    mStateTimer = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mOpacity = 0xff;
    func_02035800(&mWithMeshClsn);
    unk_5cc = 0;
    unk_5b2 = mPrevAngleY;
    unk_5b8 = 0;
    FLAGS16 |= 4;
    mBigBooID = 0;
    mSpawnerID = 0;
    mCachedActorID = 0;
    mTalkPlayer = 0;
    mChildDeaths = 0;
    unk_5b8 = 0;
    mDeathMode = data_ov063_0211e22c[unk_5cf];
    mHurtDamage = data_ov063_0211e1ec[unk_5cf];
    unk_5a8 = 0;
    FLAGS16 |= 8;
    mTimer180 = 0;
    mTerminalVelocity = -0x3c000;
    mAreaIdx = mAreaId;
    mSoundCount = 0;
    mTimer5c2 = 0;
    mTimer5c4 = 0;
    mLeashDist = ((param1 >> 8) & 0xff) * 0x64000;
    unk_5d3 = 0;
    mBodyScaleX = 0xc00;
    mBodyScaleY = 0xc00;
    mBodyScaleZ = 0xc00;

    if (unk_5cf == 5) {
        if ((unsigned)NumStars() < 0xf) {
            MarkForDestruction();
            return 1;
        }
        if (data_0209f264 == 0) {
            spawned = dActor_c::Spawn(mCarriedID, 0, *(const Vector3 *)&mPosX, 0, mAreaIdx, -1);
            if (spawned != 0) {
                /* A byte on the carried actor (0xd4). */
                *(unsigned char *)((char *)spawned + 0x37e) = 1;
                MarkForDestruction();
                return 1;
            }
        }
        mBodyModel.SetFile((BMD_File *)*(void **)((char *)&data_ov063_0211edec + 4), 1, -1);
    } else if (unk_5cf == 0 || unk_5cf == 1 || unk_5cf == 2 || unk_5cf == 6 || (unsigned)(unsigned char)(unk_5cf + 0xf8) <= 3) {
        unsigned short t = mCarriedID;
        if (t == 0x122) {
            mBodyModel.SetFile((BMD_File *)*(void **)((char *)&data_ov002_0210d9c8 + 4), 1, -1);
        } else if (t == 0x121) {
            mBodyModel.SetFile((BMD_File *)*(void **)((char *)&data_ov002_0210d9f8 + 4), 1, -1);
        } else {
            mBodyModel.SetFile((BMD_File *)*(void **)((char *)&data_ov002_0210d9b8 + 4), 1, -1);
        }
        if (unk_5cf == 0xb) {
            mAreaId = -1;
            pp = ClosestPlayer();
            if (pp != 0 && pp->mPosZ > (int)0xffaec000) {
                FLAGS16T |= 0x10;
            }
        }
    } else {
        if ((unsigned)unk_5cf < 0xc) {
            mdCcAcPos_c.flags |= 1;
        }
        if (unk_5cf == 4) {
            mAreaId = -1;
            pp = ClosestPlayer();
            if (pp != 0 && pp->mPosZ > (int)0xffaec000) {
                FLAGS16T |= 0x10;
            }
        } else if (unk_5cf == 7) {
            FLAGS16T |= 2;
            mCarriedID = 0x120;
        } else if (unk_5cf == 0xe) {
            mAreaId = -1;
        }
    }

    mClsnBaseX = 0;
    mClsnBaseY = 0;
    mClsnBaseZ = 0;
    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
    MulMat4x3Mat4x3(*(Matrix4x3 **)((char *)this + 0x394), &data_020a0e68, &data_020a0e68);
    mClsnBaseX = data_020a0e68.m[9];
    mClsnBaseY = data_020a0e68.m[10];
    mClsnBaseZ = data_020a0e68.m[11];
    SubVec3((Vector3 *)&mClsnBaseX, (Vector3 *)&mPosX, (Vector3 *)&mClsnBaseX);
    mTalkStep = 0;
    mTargetAngleY = mPrevAngleY;
    mParticle1 = 0;
    mParticle0 = mParticle1;
    return 1;
}
