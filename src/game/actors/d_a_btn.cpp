//cpp
/**
 * Whomp and King Whomp (profiles BATAN / BATANKING, RTTI daBtn_c). One class
 * serves both; mIsKing (actor ID 0xa5) picks the model, the animation set,
 * the hit points and most tunings.
 *
 * Behavior runs one handler per mState out of the pointer-to-member table
 * data_ov079_02128280, which __sinit_ov079_02127618 fills at load (not
 * claimed here: text-only promotion):
 *    0 func_ov079_02125b44  waiting (the king's introduction talk)
 *    1 func_ov079_021258fc  walking
 *    2 func_ov079_021256d4  the king walking
 *    3 func_ov079_021254b4  leaning in
 *    4 func_ov079_0212538c  falling on its face
 *    5 func_ov079_02125240  slamming down
 *    6 func_ov079_02124b08  lying flat, getting up
 *    7 func_ov079_021249f0  turning back
 *    8 func_ov079_021246dc  defeated
 *    9 func_ov079_021246d8  the king beaten, waiting to go
 *   10 func_ov079_02124638  wobbling from a hit in front
 *   11 func_ov079_02124530  knocked over from behind
 * The handlers retain address names and explicit actor parameters.
 * Converting them to methods remains reconstruction work; unknown original
 * names do not establish a compiler requirement for this free-function form.
 *
 * The file tables: data_ov079_021275ec holds the plain Whomp's five
 * animation handles and, from data_ov079_02127600 on, the king's six, read
 * as [mIsKing * 5 + n] (slot 3 is the walk, slot 2 the getting-up). The
 * model handles are data_ov079_02128170 (plain) / 02128168 (king), the
 * king's face texture 02128178, the collision meshes data_ov079_02127bf0[2].
 *
 * Known limits:
 * - Particle::System::NewSimple, dActor_c::SpawnCoins/Earthquake/
 *   DropShadowScaleXYZ, ModelAnim::SetAnim, TextureSequence::SetFile,
 *   dBgW_KcMbg::SetFile and dBgCh_Actr::Init take Fix12<int> by value, so
 *   they stay mangled (notes/mwccarm-codegen.md 6az); the rest of the
 *   mangled externs follow the legacy call shapes.
 * - func_ov079_02123804 reacts to a hit but nothing in the cartridge's
 *   relocations calls it.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 */

#include "daBtn_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct CLPS_Block;

/* Carried from func_ov079_02123f34's legacy file, where it is load-bearing
   (without it that loop is strength-reduced and every word differs). The
   pragma is file-global last-wins under deferred codegen and a push/pop
   bracket around the one function does not bind; file-wide it moves no
   other member. */
#pragma opt_strength_reduction off

/* A plain x/y/z triple. The helpers that came from C files build their
   scratch vectors in this, not in Vector3: Vector3's inline destructor
   gives a local its own stack home and moves the frame. */
struct BtnVec { s32 x, y, z; };

/* One 0x30-byte block copy. Matrix4x3's own assignment goes member by
   member through Vector3 and comes out longer. */
struct MatrixWords { s32 w[12]; };

/* The shared model/animation handle table entries: word 1 is the file. */
struct BtnFileRef { int unk0; void *file; };

extern "C" {
/* ---- this TU ---- */
void func_ov079_02123804(daBtn_c *self, dActor_c *other);
int func_ov079_02123a8c(daBtn_c *self);
int func_ov079_02123bcc(daBtn_c *self);
void func_ov079_02123d4c(int *out, daBtn_c *self);
void func_ov079_02123f34(daBtn_c *self);
void func_ov079_02124008(daBtn_c *self);
void func_ov079_02124188(daBtn_c *self);
int func_ov079_021243e0(daBtn_c *self, int range);
void func_ov079_02124530(daBtn_c *self);
void func_ov079_02124638(daBtn_c *self);
void func_ov079_021246d8(void);
void func_ov079_021246dc(daBtn_c *self);
void func_ov079_021249f0(daBtn_c *self);
void func_ov079_02124b08(daBtn_c *self);
void func_ov079_02124dec(daBtn_c *self);
void func_ov079_02124ed4(daBtn_c *self);
void func_ov079_02125058(daBtn_c *self, dActor_c *other);
void func_ov079_0212522c(void *a, void *b, void *c);
void func_ov079_02125240(daBtn_c *self);
void func_ov079_0212538c(daBtn_c *self);
void func_ov079_021254b4(daBtn_c *self);
void func_ov079_02125504(daBtn_c *self);
void func_ov079_021256d4(daBtn_c *self);
void func_ov079_021258fc(daBtn_c *self);
void func_ov079_02125b44(daBtn_c *self);

/* ---- arm9 / ov002 ---- */
int AngleDiff(int, int);
unsigned char DecIfAbove0_Byte(unsigned char *p);
int IsStarCollectedInCurLevel(int a);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationZ(void *m, s16 a);
void Matrix4x3_ApplyInPlaceToScale(void *m, int x, int y, int z);
void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Vec3_Asr(void *d, void *s, int sh);
int Vec3_Dist(void *a, void *b);
s16 Vec3_HorzAngle(const void *a, const void *b);
int Vec3_HorzDist(const void *a, const void *b);
int _Z14ApproachLinearRsss(s16 *p, s16 a, s16 b);

void func_01ffb07c(void *, void *);
void func_01ffb098(void *);
void func_01ffb0a4(void *);
void func_01ffb0b0(void *);
void func_01ffb0bc(void *);
void func_0200fa04(void *c, void *v, int b);
void func_02011cfc(void);
void func_02011d38(void);
void func_0201267c(unsigned int id, void *p);
void func_02012694(int a, void *p);
void func_020393c4(void *p, int v);
void func_020393d4(void *p, void *v);
void func_020396d0(int *, int);
int func_ov002_020dd8b8(void *c);

void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int a, int b, void *c, void *d);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *mc, void *kcl, void *mtx, int fix, s16 s, void *clps);
int _ZN11ShadowModel10InitCuboidEv(void *self);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, unsigned int b);
void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *shared);
void _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
void _ZN4dBgW7DisableEv(void *);
int _ZN4dBgW9IsEnabledEv(void *);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *shared);
void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int);
void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
    void *player, void *actor, unsigned int msg, const void *pos, unsigned int a, unsigned int b);
int _ZN6Player12GetTalkStateEv(void *player);
int _ZN6Player9StartTalkER7fBase_cb(void *player, void *actor, int b);
void _ZN7Message11PrepareTalkEv(void);
void _ZN7Message7EndTalkEv(void);
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *shared);
void _ZN7fBase_c18MarkForDestructionEv(void *self);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, const void *v, int f);
void _ZN8dActor_c10PoofDustAtERK7Vector3(void *actor, const void *p);
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const void *pos, u32 n, int fix, s16 s);
void _ZN8dActor_c16TriplePoofDustAtERK7Vector3(void *actor, const void *p);
void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *sm, void *mtx, int a, int b, int c, unsigned int g);
void _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(
    void *actor, signed char *flag, unsigned int id, const void *pos, unsigned int j);
u8 _ZN8dActor_c9TrackStarEjj(void *self, unsigned int a, unsigned int b);
void *_ZN9Animation8LoadFileER13SharedFilePtr(void *shared);
void _ZN9Animation8SetFlagsEi(void *self, int flags);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *mb, void *bmd, int a, int b);

/* ---- data ---- */
extern s16 data_02082214[];
extern unsigned int data_0209b454;
extern u8 data_0209d660;
extern u8 data_0209f21c;
extern u8 data_0209f220;
extern signed char data_0209f2f8;
extern int data_0209f318;
extern Player *data_0209f394[];
extern MatrixWords data_020a0e68;
/* The king's collision mesh takes Whomp's Fortress's own CLPS block. The
   relocation's target address is shared by 15 level overlays; residency
   leaves ov015 and ov026, and only ov015 holds a CLPS block there (ov026's
   words are mid-table), so the data settles it. */
extern CLPS_Block data_ov015_021135d4;

extern int data_ov079_021275dc[];
extern int data_ov079_021275e4[];
extern void *data_ov079_021275ec[];
extern void *data_ov079_02127600[];
extern CLPS_Block data_ov079_02127ba0;   /* the plain Whomp's CLPS block */
extern void *data_ov079_02127bf0[];
extern int data_ov079_02127cfc[];
extern BtnFileRef data_ov079_02128168;
extern BtnFileRef data_ov079_02128170;
extern BtnFileRef data_ov079_02128178;
extern BtnFileRef data_ov079_021281b0;
}

/* OnAimedAtWithEggReturnVec's view of func_ov079_02123d4c: the (out, this)
   pair of the definition below is exactly the hidden-return-pointer shape
   of a Vector3-by-value call, so this view lets the virtual forward to it. */
namespace vecret { extern "C" Vector3 func_ov079_02123d4c(void *thiz); }

typedef int (dActor_c::*PMF)();
extern "C" PMF data_ov079_02128280[];

// @symbol daBtn_c_classInit_BATAN
extern "C" daBtn_c *daBtn_c_classInit_BATAN()
{
    return new daBtn_c();
}

// @symbol daBtn_c_classInit_BATANKING
extern "C" daBtn_c *daBtn_c_classInit_BATANKING()
{
    return new daBtn_c();
}

// @symbol _ZN7daBtn_c13InitResourcesEv
/* Load the class's model, animations (and the king's face texture), set up
   the shadow, the collision mesh and the wall probe, record the spawn pose,
   and start waiting (state 0) on the walk animation. The king claims its
   star marker, and does not come back once that star is collected. */
int daBtn_c::InitResources()
{
    u16 id;
    int r6;
    u8 idx;
    void *bmd;
    void *kcl;
    void *anim;
    int i;

    id = actorID;
    int b = (id == 0xa5);
    if (b) {
        mIsKing = 1;
        bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov079_02128168);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, bmd, 1, -1);
        r6 = 0;
        do {
            anim = data_ov079_02127600[r6];
            _ZN9Animation8LoadFileER13SharedFilePtr(anim);
            r6 += 1;
        } while (r6 < 6);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov079_02128178);
    } else {
        mIsKing = 0;
        bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov079_02128170);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, bmd, 1, -1);
        r6 = 0;
        do {
            anim = data_ov079_021275ec[r6];
            _ZN9Animation8LoadFileER13SharedFilePtr(anim);
            r6 += 1;
        } while (r6 < 5);
    }

    idx = mIsKing;
    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov079_02127bf0[idx]);
    (void)kcl;

    if (_ZN11ShadowModel10InitCuboidEv(&mShadowModel) == 0) {
        return 0;
    }

    if (mIsKing != 0) {
        mHitPoints = 3;
        TextureSequence::Prepare(*(BMD_File *)data_ov079_02128168.file,
                                 *(BTP_File *)data_ov079_02128178.file);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            &mTextureSequence, data_ov079_02128178.file, 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(&mTextureSequence, 0x40000000);
        mTextureSequence.speed = 0x1000;
        mTextureSequence.currFrame = 0;
        mStarID = (u8)(param1 & 0xf);
        unk_408 = _ZN8dActor_c9TrackStarEjj(this, mStarID, 2);
    } else {
        mHitPoints = 1;
    }

    func_ov079_02124188(this);
    func_ov079_02124008(this);

    idx = mIsKing;
    if (idx == 0) {
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMovingMeshCollider, ((BtnFileRef *)data_ov079_02127bf0[idx])->file, mMeshMatrix,
            0x199, mAngleY, &data_ov079_02127ba0);
    } else {
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMovingMeshCollider, ((BtnFileRef *)data_ov079_02127bf0[idx])->file, mMeshMatrix,
            0x1000, mAngleY, &data_ov015_021135d4);
    }

    func_01ffb0bc(&mMovingMeshCollider);
    func_020393d4(&mMovingMeshCollider, (void *)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    {
        s32 sp[3];
        sp[0] = 0;
        sp[1] = 0;
        sp[2] = 0x1000;
        func_01ffb07c(&mMovingMeshCollider, sp);
    }

    func_020396d0((int *)&mMovingMeshCollider, 0xb50);

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);

    mState = 0;
    mTargetAngle = mPrevAngleY;
    mSubState = 0;
    mSubTimer = 0;

    idx = mIsKing;
    anim = ((BtnFileRef *)data_ov079_021275ec[idx * 5 + 3])->file;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, anim, 0, 0x1000, 0);

    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mSpawnAngleX = mAngleX;
    mSpawnAngleY = mAngleY;
    mSpawnAngleZ = mAngleZ;

    if (mIsKing != 0) {
        mVertAccel = -0x8000;
    } else {
        mVertAccel = -0x4800;
    }
    mTerminalVelocity = -0x31000;

    mPounded = 0;
    mTouched = 0;

    if (mIsKing != 0) {
        mCoinsLeft = 4;
    } else {
        mCoinsLeft = 6;
    }

    mTipLift = 0;
    mShouldRender = 1;
    unk_407 = 0;
    mResumePending = 0;
    mStateTimer = 0;

    i = 0;
    do {
        mPlayers[i] = 0;
        i += 1;
    } while (i < 4);

    {
        s32 cnt = data_0209f21c;
        if (cnt > 0) {
            i = 0;
            do {
                Player *v = data_0209f394[i];
                mPlayers[i] = v;
                i += 1;
            } while (i < cnt);
        }
    }

    if (mIsKing != 0
        && data_0209f2f8 == 7
        && data_0209f220 != 1
        && IsStarCollectedInCurLevel(1) != 0) {
        MarkForDestruction();
        return 0;
    }

    return 1;
}

// @symbol _ZN7daBtn_c8BehaviorEv
/* Per frame: the king claims the camera's target while a player is within
   0x1770000 (except once beaten), then survey the players, move, keep off
   ledges, collide with the walls, run the state handler, pose the model and
   shadow, and pose the collision mesh whenever it is live. */
int daBtn_c::Behavior()
{
    if (mIsKing != 0 && mState != 9) {
        if (DistToCPlayer() < 0x1770000) {
            *(int*)(*(int*)&data_0209f318 + 0x114) = (int)((dActor_c*)this);
        }
    }

    func_ov079_02123f34(this);
    UpdatePos(0);

    if (mHorzSpeed != 0) {
        if (IsGoingOffCliff(mWithMeshClsn, 0x3c000, (short)0x2888, false, false, 0x32000)) {
            /* The step would leave the ledge: rewind to last frame's position. */
            mPosX = mSafePosX;
            mPosY = mSafePosY;
            mPosZ = mSafePosZ;
        } else {
            mSafePosX = mPosX;
            mSafePosY = mPosY;
            mSafePosZ = mPosZ;
        }
    } else {
        mSafePosX = mPosX;
        mSafePosY = mPosY;
        mSafePosZ = mPosZ;
    }

    UpdateWMClsn(mWithMeshClsn, 0);

    {
        int idx = mState;
        PMF* pmf = &data_ov079_02128280[idx];
        (((dActor_c*)this)->**pmf)();

        {
            /* dEnemyBase_c's 0x100 counts frames spent in the current state: it is
               incremented here and reset the moment the handler changed mState. */
            unsigned short* ctr = (unsigned short*)&mStateTimer;
            *ctr = *ctr + 1;
            if (idx != mState) {
                *ctr = 0;
                mSubState = 0;
            }
        }
    }

    func_ov079_02124188(this);

    if (func_ov079_021243e0(this, 0) == 0 || func_ov079_02123a8c(this) != 0) {
        func_ov079_02124008(this);
    }

    mTouched = 0;
    return 1;
}

// @symbol _ZN7daBtn_c6RenderEv
int daBtn_c::Render()
{
  if(mShouldRender==0) return 1;
  if(mIsKing!=0)
    mTextureSequence.Update(mModelAnim.data);
  mModelAnim.Render(0);
  return 1;
}

// @symbol _ZN7daBtn_c16CleanupResourcesEv
int daBtn_c::CleanupResources()
{
  int i;
  if(mMovingMeshCollider.IsEnabled())
    mMovingMeshCollider.Disable();
  ((SharedFilePtr *)(data_ov079_02127bf0[mIsKing]))->Release();
  if(mIsKing){
    ((SharedFilePtr *)&data_ov079_02128168)->Release();
    for(i=0;i<6;i++)
      ((SharedFilePtr *)(data_ov079_02127600[i]))->Release();
    ((SharedFilePtr *)&data_ov079_02128178)->Release();
  } else {
    ((SharedFilePtr *)&data_ov079_02128170)->Release();
    for(i=0;i<5;i++)
      ((SharedFilePtr *)(data_ov079_021275ec[i]))->Release();
  }
  return 1;
}

// @symbol func_ov079_02125b44
/* State 0, waiting. A plain Whomp wakes (state 1) when a player comes
   within 0x3e8000. The king instead holds its introduction: once the
   nearest player stands close (0x190000) and not far below it, start a
   talk, turn to face them, show message 0xa4, swap the music while the
   message box is up, and after the talk ends start walking (state 2). */
extern "C" void func_ov079_02125b44(daBtn_c *self)
{
    int pos[3];

    if (self->mModelAnim.Finished() != 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &self->mModelAnim,
            ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5 + 3])->file,
            0, 0x1000, 0);
    }

    if (self->mIsKing == 0)
        goto variant0;

    self->mModelAnim.Advance();
    self->mModelAnim.speed = 0x1000;

    switch (self->mSubState) {
    case 0:
        if (self->mNearestDist < 0x190000) {
            Player *p = self->mPlayers[self->mNearestPlayer];
            if (p->mPosY >= self->mPosY - 0xa000) {
                if (_ZN6Player9StartTalkER7fBase_cb(p, self, 1) != 0) {
                    u8 *st = &self->mSubState;
                    (*st)++;
                }
                break;
            }
        }
        self->mHitPoints = 3;
        break;
    case 1:
        if (_Z14ApproachLinearRsss(&self->mPrevAngleY, self->mTargetAngle, 0x800) != 0) {
            u8 *st = &self->mSubState;
            (*st)++;
        }
        self->mAngleY = self->mPrevAngleY;
        break;
    case 2:
        if (_ZN6Player12GetTalkStateEv(self->mPlayers[self->mNearestPlayer]) == 0) {
            int *src;
            u16 ang;
            int i;
            int y;
            {
                Player *p = self->mPlayers[self->mNearestPlayer];
                int *src = &p->mPosX;
                pos[0] = src[0];
                pos[1] = src[1];
                pos[2] = src[2];
            }
            ang = Vec3_HorzAngle(pos, &self->mPosX);
            i = (u16)ang >> 4;
            y = pos[1] + 0x96000;
            pos[0] = data_02082214[i * 2] * 0xd7 + pos[0];
            pos[2] = data_02082214[i * 2 + 1] * 0xd7 + pos[2];
            pos[1] = y;
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                    self->mPlayers[self->mNearestPlayer],
                    self, 0xa4, pos, 0, 0) != 0) {
                u8 *st = &self->mSubState;
                (*st)++;
                _ZN7Message11PrepareTalkEv();
                func_0201267c(0x133, &self->mCamSpacePosX);
            }
        }
        break;
    case 3:
        if (data_0209d660 != 0) {
            u8 *st = &self->mSubState;
            (*st)++;
        }
        break;
    case 4:
        if (data_0209d660 == 0) {
            _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
            func_02011d38();
            _ZN7Message7EndTalkEv();
            {
                u8 *st = &self->mSubState;
                (*st)++;
            }
        }
        break;
    case 5:
        if (_ZN6Player12GetTalkStateEv(self->mPlayers[self->mNearestPlayer]) == -1) {
            u8 *st = &self->mSubState;
            (*st)++;
        }
        break;
    case 6:
        self->mState = 2;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &self->mModelAnim,
            ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5 + 3])->file,
            0, 0x1000, 0);
        break;
    }
    func_ov079_02125504(self);
    return;

variant0:
    if (self->mNearestDist < 0x3e8000) {
        self->mState = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &self->mModelAnim,
            ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5 + 3])->file,
            0, 0x1000, 0);
    }
    self->mModelAnim.Advance();
}

// @symbol func_ov079_021258fc
/* State 1, the plain Whomp's walk: advance on the walk cycle (standing
   still for its first frames), play the footsteps, and head back (state 7)
   once 0x2bc000 from the spawn point -- 0xc8000 in level 0x27. A player
   dead ahead within 0xc8000 starts the lean (state 3); within 0x5dc000 the
   pace picks up. */
extern "C" void func_ov079_021258fc(daBtn_c *self)
{
    unsigned int frame;
    int new_var;
    u8 idx;
    unsigned int q;
    void *e;
    unsigned int cur;
    unsigned int want;
    int lim;

    idx = self->mIsKing;
    q = self->mModelAnim.currFrame;
    e = data_ov079_021275ec[((int)idx * 5) + 3];
    cur = *(volatile unsigned int *)&self->mModelAnim.file;
    if ((self && self) && self) {
    }
    want = *(unsigned int *)((char *)e + 4);
    frame = (unsigned short)(q >> 12);

    if (cur != want) {
        if (frame < 3) {
            self->mHorzSpeed = 0;
        } else {
            new_var = idx;
            self->mHorzSpeed = data_ov079_021275dc[new_var];
        }
        if (self->mModelAnim.Finished() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                &self->mModelAnim,
                ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5 + 3])->file,
                0, 0x1000, 0);
        }
    } else {
        new_var = idx;
        self->mHorzSpeed = data_ov079_021275dc[new_var];
    }

    if (data_0209f2f8 == 0x27)
        lim = 0xc8000;
    else
        lim = 0x2bc000;
    self->mModelAnim.speed = 0x1000;
    if (Vec3_HorzDist(&self->mPosX, &self->mSpawnPosX) > lim) {
        u32 *flags;
        self->mState = 7;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &self->mModelAnim,
            ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5 + 3])->file,
            0, 0x1000, 0);
        flags = &self->mFlags;
        *flags = *flags & ~0x2000000;
        goto after_dist;
    }

    if (self->mFrontDist < 0x5dc000) {
        self->mHorzSpeed = 0x9000;
        self->mModelAnim.speed = data_ov079_021275dc[self->mIsKing];
    }
    {
        int five = 5;
        if ((int)self->mModelAnim.file == *(int *)((char *)data_ov079_021275ec[self->mIsKing * five + 3] + 4)) {
            if (self->mFrontDist < 0xc8000) {
                self->mState = 3;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    &self->mModelAnim,
                    ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * five])->file,
                    0x40000000, 0x1000, 0);
            }
        }
    }
after_dist:
    if ((int)self->mModelAnim.file == *(int *)((char *)data_ov079_021275ec[self->mIsKing * 5 + 3] + 4)) {
        if (frame > 7) {
            if (frame < 0x1e)
                goto clear_flag;
            if (frame > 0x26)
                goto clear_flag;
        }
        self->mHorzSpeed = 0;
        if (self->mStepPlayed != 0)
            goto done_flag;
        self->mStepPlayed = 1;
        func_0201267c(0xc5, &self->mCamSpacePosX);
        goto done_flag;
    clear_flag:
        self->mStepPlayed = 0;
    done_flag:
        ;
        ;
        ;
    }

    self->mModelAnim.Advance();
}

// @symbol func_ov079_021256d4
/* State 2, the king's walk: turn toward the nearest player and shove them
   aside (func_ov079_02123bcc), speed up within 0x5dc000 and lean (state 3)
   at a non-mega player dead ahead within 0x190000. If the nearest player
   ends up more than 0x37f000 below, give up: restore the music and put the
   king back where and how it spawned, waiting (state 0). */
extern "C" void func_ov079_021256d4(daBtn_c *self)
{
    Player *r;

    if ((int)self->mModelAnim.file != *(int *)((char *)data_ov079_021275ec[self->mIsKing * 5 + 3] + 4)) {
        if (self->mModelAnim.Finished() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                &self->mModelAnim,
                ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5 + 3])->file,
                0, 0x1000, 0);
        }
    }

    self->mHorzSpeed = data_ov079_021275dc[self->mIsKing];
    self->mModelAnim.speed = 0x1000;
    func_ov079_02123bcc(self);

    _Z14ApproachLinearRsss(&self->mPrevAngleY, self->mTargetAngle, 0x200);
    self->mAngleY = self->mPrevAngleY;

    if (*(u16 *)&self->mStateTimer > 0x1e) {
        if (self->mFrontDist < 0x5dc000) {
            self->mHorzSpeed = 0xd800;
            self->mModelAnim.speed = 0x3000;
        }
        r = self->mPlayers[self->mFrontPlayer];
        if (r->mIsMega == 0) {
            if (self->mFrontDist < 0x190000) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    &self->mModelAnim,
                    ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5])->file,
                    0x40000000, 0x1000, 0);
                self->mState = 3;
            }
        }
    }

    func_ov079_02125504(self);
    self->mModelAnim.Advance();

    r = self->mPlayers[self->mNearestPlayer];
    if (r != 0) {
        if (self->mPosY - r->mPosY <= 0x37f000)
            return;
    }

    {
        s16 t;
        void *anim;
        int five;
        void **tab;
        ModelAnim *ma;

        _ZN5Sound22StopLoadedMusic_Layer3Ev();
        func_02011cfc();
        self->mState = 0;
        self->mPosX = self->mSpawnPosX;
        self->mPosY = self->mSpawnPosY;
        five = 5;
        self->mPosZ = self->mSpawnPosZ;
        tab = data_ov079_021275ec;
        t = self->mSpawnAngleX;
        ma = &self->mModelAnim;
        self->mAngleX = t;
        t = self->mSpawnAngleY;
        self->mAngleY = t;
        t = self->mSpawnAngleZ;
        self->mAngleZ = t;
        t = self->mSpawnAngleX;
        self->mPrevAngleX = t;
        t = self->mSpawnAngleY;
        self->mPrevAngleY = t;
        t = self->mSpawnAngleZ;
        self->mPrevAngleZ = t;
        self->mTextureSequence.currFrame = 0;
        self->mHorzSpeed = 0;
        anim = ((BtnFileRef *)tab[self->mIsKing * five + 3])->file;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ma, anim, 0, 0x1000, 0);
    }
}

struct BtnEnt { int f0; int f4; };

// @symbol func_ov079_02125504
/* The walk cycle's footfalls: on the walk animation, frames 0..7 and
   0x1e..0x26 are a foot coming down -- stop, and once per footfall play the
   step, shake the camera (unless flag 0x800000 of data_0209b454 is set) and
   kick up dust beside the body, left or right by the frame. */
extern "C" void func_ov079_02125504(daBtn_c *self)
{
    unsigned int frame;
    BtnVec v[2];
    s16 t;
    s32 px, pz;

    {
        unsigned int q;
        u8 i;
        BtnEnt *e;
        unsigned int cur;
        unsigned int want;
        i = self->mIsKing;
        q = self->mModelAnim.currFrame;
        e = (BtnEnt *)data_ov079_021275ec[(int)i * 5 + 3];
        cur = *(volatile unsigned int *)&self->mModelAnim.file;
        want = e->f4;
        frame = (unsigned short)(q >> 12);
        if (cur != want)
            return;
    }

    if (frame > 7) {
        if (frame < 0x1e)
            goto reset;
        if (frame > 0x26)
            goto reset;
    }

    self->mHorzSpeed = 0;
    if (self->mStepPlayed != 0)
        return;
    self->mStepPlayed = 1;
    func_0201267c(0xc5, &self->mCamSpacePosX);

    if ((data_0209b454 & 0x800000) == 0) {
        v[1].x = self->mPosX;
        v[1].y = self->mPosY;
        v[1].z = self->mPosZ;
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(self, &v[1], 0x320000);
    }

    {
        s16 a0 = self->mAngleY;
        s16 a1 = self->mBodyYaw;
        s32 py;
        px = self->mPosX;
        v[0].x = px;
        py = self->mPosY;
        t = (s16)(a0 + a1 + 0x4000);
        v[0].y = py;
        pz = self->mPosZ;
        v[0].z = pz;
        v[0].y = py + 0x28000;
    }

    if (frame <= 7) {
        int idx = (u16)t >> 4;
        s16 sn = data_02082214[idx * 2];
        s16 cs = data_02082214[idx * 2 + 1];
        v[0].x = px - sn * 0xaa;
        v[0].z = pz - cs * 0xaa;
    } else {
        int idx = (u16)t >> 4;
        s16 sn = data_02082214[idx * 2];
        s16 cs = data_02082214[idx * 2 + 1];
        v[0].x = px + sn * 0xaa;
        v[0].z = pz + cs * 0xaa;
    }
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, v[0].x, v[0].y, v[0].z);
    return;
reset:
    self->mStepPlayed = 0;
}

// @symbol func_ov079_021254b4
/* State 3, the lean: stand still until the topple animation reaches frame
   15, then start falling (state 4). */
extern "C" void func_ov079_021254b4(daBtn_c *self)
{
    self->mHorzSpeed = 0;
    self->mModelAnim.Advance();
    if (!self->mModelAnim.WillHitFrame(15))
        return;
    self->mState = 4;
    self->mFlags |= 0x2000000;
}

// @symbol func_ov079_0212538c
/* State 4, the fall: keep the mesh collider live, hop on the first frame,
   and from frame 8 tip forward with a growing mPitchSpeed until the body is
   flat (mAngleX 0x4000), which lands it in state 5. The mesh is raised with
   the tilt so the face stays on the ground. */
extern "C" void func_ov079_0212538c(daBtn_c *self)
{
    func_01ffb0b0(&self->mMovingMeshCollider);
    func_01ffb0a4(&self->mMovingMeshCollider);
    self->mModelAnim.Advance();
    if (*(u16 *)&self->mStateTimer == 0)
        self->mVertSpeed = data_ov079_021275e4[self->mIsKing];
    if (*(u16 *)&self->mStateTimer < 8)
        return;
    s16 *pitch = &self->mAngleX;
    if (self->mIsKing) {
        s32 *speed = &self->mPitchSpeed;
        *speed += 0x130;
    } else {
        s32 *speed = &self->mPitchSpeed;
        *speed += 0x100;
    }
    *pitch = *pitch + self->mPitchSpeed;
    if (self->mAngleX > 0x4000) {
        self->mPitchSpeed = 0;
        self->mAngleX = 0x4000;
        self->mState = 5;
        func_0201267c(0xc6, &self->mCamSpacePosX);
    }
    if (self->mIsKing) {
        self->mTipLift = (s16)self->mAngleX * (s16)0x15;
        if (self->mTipLift >= 0x53000)
            self->mTipLift = 0x53000;
    } else {
        self->mTipLift = (s16)self->mAngleX * (s16)0xa;
        if (self->mTipLift >= 0x25000)
            self->mTipLift = 0x25000;
    }
}

// @symbol func_ov079_02125240
/* State 5, the slam: once the face hits the ground, stop falling, freeze
   the mesh collider, shake the camera, start the lying-down animation and
   move to state 6; from then on the collider's contact callback is the
   veneer above. The king lands with a heavier dust burst. */
extern "C" void func_ov079_02125240(daBtn_c *self){
  Vector3 v;
  Vector3 a;
  Vector3 b;
  if(self->mSubState) return;
  if(!self->mWithMeshClsn.IsOnGround()) return;
  self->mVertSpeed = 0;
  func_01ffb098(&self->mMovingMeshCollider);
  func_01ffb0bc(&self->mMovingMeshCollider);
  { u8 *sub = &self->mSubState; *sub += 1; }
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, ((BtnFileRef *)data_ov079_021275ec[self->mIsKing*5+1])->file, 0x40000000, 0x1000, 0);
  v.x = self->mPosX;
  v.y = self->mPosY;
  v.z = self->mPosZ;
  _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(self, &v, 0x7d0000);
  self->mState = 6;
  func_020393c4(&self->mMovingMeshCollider, (int)func_ov079_0212522c);
  if(self->mIsKing != 0){
    func_ov079_02123d4c((int *)&a, self);
    func_0200fa04(self, &a, 0);
  } else {
    func_ov079_02123d4c((int *)&b, self);
    self->LandingDustAt(b, false);
  }
}

// @symbol func_ov079_0212522c
/* The mesh collider's contact callback while the body lies flat: drops the
   collider argument and forwards (actor, other). long_calls emits the pooled
   `ldr ip,[pc,#8]; bx ip` absolute tail-call; bracketed closed immediately
   because it is positional. */
#pragma long_calls on
extern "C" void func_ov079_0212522c(void *a, void *b, void *c)
{
    func_ov079_02125058((daBtn_c *)b, (dActor_c *)c);
}
#pragma long_calls off

// @symbol func_ov079_02125058
/* Contact from the lying body's back. Only players count. A contact that
   func_ov002_020dd8b8 accepts is a hit: it arms mPounded and remembers the
   player (the king ignores it while a resumed state 0 is pending). Any
   other touch pays out one coin per contact while mCoinsLeft lasts -- for
   the king only when the player stands at least 0x2bc000 above it in
   states 1 and 2. */
extern "C" void func_ov079_02125058(daBtn_c *self, dActor_c *other)
{
    volatile BtnVec v1, v2;
    BtnVec w1, w2;
    int eq = (int)(other->actorID == 0xbf);
    if (!eq) return;
    self->mTouched = 1;
    if (func_ov002_020dd8b8(other) != 0) {
        if (self->mResumePending != 0 && self->mIsKing != 0) {
            if (self->mResumeState == 0) return;
        }
        self->mPounded = 1;
        self->mPounder = (Player *)other;
        return;
    }
    if (self->mCoinGiven != 0) return;
    if (self->mIsKing == 0) {
        BtnVec* p;
        s32 x, y, z;
        if (DecIfAbove0_Byte(&self->mCoinsLeft) == 0) return;
        p = (BtnVec*)&other->mPosX;
        x = p->x; v1.x = x;
        y = p->y; v1.y = y;
        z = p->z; v1.z = z;
        w1.x = x; w1.y = y; w1.z = z;
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &w1, 1, 0x8000, 0);
        self->mCoinGiven = 1;
        return;
    }
    {
        BtnVec* p;
        s32 x, y, z;
        u32 t = self->mState;
        if (t - 1 > 1) return;
        p = (BtnVec*)&other->mPosX;
        x = p->x; v2.x = x;
        y = p->y; v2.y = y;
        z = p->z; v2.z = z;
        if (y - self->mPosY < 0x2bc000) return;
        if (DecIfAbove0_Byte(&self->mCoinsLeft) == 0) return;
        w2.x = v2.x; w2.y = v2.y; w2.z = v2.z;
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &w2, 1, 0x8000, 0);
        self->mCoinGiven = 1;
        return;
    }
}

// @symbol func_ov079_02124ed4
/* The king's lying-down phase of state 6. A ground-pound on its back
   (mPounded) costs a hit point, with a sound and a burst at the player's
   feet: the last one defeats it (state 8), any other swaps the face texture
   and moves on to the hurt shake -- ten frames of bobbing -- before phase
   10 gets it up. */
extern "C" void func_ov079_02124ed4(daBtn_c *self)
{
    self->mModelAnim.Advance();
    self->mCoinGiven = 0;
    if (self->mSubState == 0) {
        if (self->mPounded != 0) {
            self->mPounded = 0;
            {
                u8 *hp = &self->mHitPoints;
                *hp = *hp - 1;
            }
            func_02012694(0x134, &self->mCamSpacePosX);
            {
                s32 *p = &self->mPounder->mPosX;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(3, p[0], p[1], p[2]);
            }
            if (self->mHitPoints == 0) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, data_ov079_021281b0.file, 0x40000000, 0x1000, 0);
                self->mState = 8;
            } else {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, data_ov079_021281b0.file, 0x40000000, 0x1000, 0);
                self->mPounder = 0;
                self->mTextureSequence.Advance();
                {
                    u8 *sub = &self->mSubState;
                    *sub = *sub + 1;
                }
            }
        }
        self->mSubTimer = 0;
        return;
    } else {
        unsigned short v;
        {
            u16 *t = &self->mSubTimer;
            *t = *t + 1;
        }
        v = self->mSubTimer;
        if (v <= 10) {
            if ((int)v % 2) {
                s32 *y = &self->mPosY;
                *y = *y + 0x10000;
            } else {
                s32 *y = &self->mPosY;
                *y = *y - 0x10000;
            }
            return;
        }
        self->mSubState = 10;
        self->mSubTimer = 0;
    }
}

// @symbol func_ov079_02124dec
/* The plain Whomp's lying-down phase of state 6: a ground-pound on its
   back (mPounded) pays out five coins and defeats it (state 8); a plain
   touch only advances the phase, and letting go resets it. */
extern "C" void func_ov079_02124dec(daBtn_c *self)
{
    volatile Vector3 v1;
    Vector3 w1;
    self->mModelAnim.Advance();
    if (self->mSubState == 0) {
        if (self->mTouched != 0) {
            if (self->mPounded != 0) {
                int x = self->mPosX;
                v1.x = x;
                int y = self->mPosY;
                v1.y = y;
                int z = self->mPosZ;
                y = y + 0x78000;

                *(volatile int *)&w1.x = x;
                v1.z = z;
                w1.z = z;
                v1.y = y;
                w1.y = y;

                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &w1, 5, 0x8000, 0);
                self->mState = 8;
            } else {
                u8 *sub = &self->mSubState;
                (*sub)++;
            }
        } else {
            self->mCoinGiven = 0;
        }
    } else {
        if (self->mTouched == 0)
            self->mSubState = 0;
    }
}

// @symbol func_ov079_02124b08
/* State 6, lying flat. Phases below 10 belong to the class's own handler
   (the king's hit counting, the plain Whomp's coin/defeat check); after
   100 frames (120 for the king), or 30 once unk_407 is armed, the body
   starts to get up (phase 11). Phase 10, after the king's hurt shake,
   waits 30 frames and gets up too. Phase 11 pitches back up to level, then
   either resumes the state a hit interrupted or walks again (state 1, or 2
   for the king); the mesh lift follows a per-frame table meanwhile. */
extern "C" void func_ov079_02124b08(daBtn_c *self)
{
    u8 st = self->mSubState;

    if (st < 0xa) {
        self->mHorzSpeed = 0;
        self->mPitchSpeed = 0;
        self->unk_3cc = 0;
        self->unk_3d0 = 0;
        if (self->mIsKing != 0)
            func_ov079_02124ed4(self);
        else
            func_ov079_02124dec(self);

        {
            int lim = self->mIsKing * 0x14 + 0x64;
            u16 cur = *(u16 *)&self->mStateTimer;
            if ((int)cur <= lim) {
                if (self->unk_407 != 1)
                    return;
                if (cur <= 0x1e)
                    return;
            }
            self->unk_407 = 0;
            self->mSubState = 0xb;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                &self->mModelAnim,
                ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5 + 2])->file,
                0x40000000, 0x1000, 0);
            self->mSubTimer = 0;
            return;
        }
    }


    if (st == 0xa) {
        self->mModelAnim.Advance();
        {
            u16 *t = &self->mSubTimer;
            *t = *t + 1;
        }
        if (self->mSubTimer <= 0x1e)
            return;
        self->unk_407 = 0;
        {
            u8 *sub = &self->mSubState;
            *sub = *sub + 1;
        }
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &self->mModelAnim,
            ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5 + 2])->file,
            0x40000000, 0x1000, 0);
        self->mSubTimer = 0;
        return;
    }

    if (self->mAngleX > 0) {
        if (self->mSubTimer >= 0xc) {
            s16 *pitch = &self->mAngleX;
            self->mPitchSpeed = -0x290;
            *pitch = (s16)(*pitch + self->mPitchSpeed);
        }
        {
            u16 *t = &self->mSubTimer;
            *t = *t + 1;
        }
    } else {
        int z = 0;
        self->mPitchSpeed = z;
        self->mAngleX = z;
        if (self->mModelAnim.Finished() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                &self->mModelAnim,
                ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5 + 4])->file,
                0x40000000, 0x1000, 0);

            if (self->mIsKing == 0) {
                func_020393c4(&self->mMovingMeshCollider, 0);
            }
            self->mTouched = 0;
            self->mPounded = 0;
            if (self->mResumePending != 0) {
                self->mState = self->mResumeState;
                self->mResumePending = 0;
                if (self->mState == 7)
                    self->mResumeState = 0xb;
                return;
            }
            if (self->mIsKing != 0)
                self->mState = 2;
            else
                self->mState = 1;
            return;
        }
    }

    self->mModelAnim.Advance();
    {
        u8 v;
        unsigned int q;
        int m;
        v = self->mIsKing;
        q = self->mModelAnim.currFrame;
        m = v * 0x30;
        self->mTipLift = data_ov079_02127cfc[m + (int)((unsigned short)(q >> 12))];
    }
}

// @symbol func_ov079_021249f0
/* State 7, turn back: spin in place 0x400 a frame for 32 frames, then walk
   off at the class's pace and hand over to state 1 after 42. A hit that
   interrupted this state resumes it where it left off. */
extern "C" void func_ov079_021249f0(daBtn_c *self)
{
    if ((unsigned int)(self->mResumeState - 10) <= 1) {
        self->mSubState = self->mSavedSubState;
        *(u16 *)&self->mStateTimer = self->mSavedStateTimer + 1;
        self->mResumeState = 7;
    }
    if (self->mSubState == 0) {
        self->mHorzSpeed = 0;
        self->mModelAnim.speed = 0x2000;
        if (*(u16 *)&self->mStateTimer >= 0x20) {
            u8 *sub = &self->mSubState;
            (*sub)++;
        } else {
            s16 *heading = &self->mPrevAngleY;
            *heading = *heading + 0x400;
            self->mAngleY = self->mPrevAngleY;
        }
    } else {
        self->mHorzSpeed = data_ov079_021275dc[self->mIsKing];
        if (*(u16 *)&self->mStateTimer > 0x2a) {
            self->mState = 1;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                &self->mModelAnim,
                ((void **)data_ov079_021275ec[self->mIsKing * 5 + 3])[1],
                0, 0x1000, 0);
        }
    }
    self->mModelAnim.Advance();
}

// @symbol func_ov079_021246dc
/* State 8, defeat. The plain Whomp just bursts into dust and is gone. The
   king first talks: start a conversation with the player that beat it,
   show message 0xa5 beside the nearest player, wait for it to close, then
   burst, spawn its star (star mStarID, from 0x64000 above it) and go. */
extern "C" void func_ov079_021246dc(daBtn_c *self)
{
    BtnVec pos;
    BtnVec starPos;
    BtnVec v;
    BtnVec v2;
    BtnVec dp;
    BtnVec dp2;

    if (self->mIsKing != 0) {
        switch (self->mSubState) {
        case 0:
            if (_ZN6Player9StartTalkER7fBase_cb(self->mPounder, self, 1) == 0)
                return;
            (*(u8 *)&self->mSubState)++;
            return;
        case 1:
            if (_ZN6Player12GetTalkStateEv(self->mPounder) != 0)
                return;
            {
                int idx = self->mNearestPlayer;
                Player *other = self->mPlayers[idx];
                BtnVec *op = (BtnVec *)&other->mPosX;
                unsigned short ang;
                int i;
                short mag = 0x10e;
                pos.x = op->x;
                pos.y = op->y;
                pos.z = op->z;
                ang = Vec3_HorzAngle(&pos, &self->mPosX);
                i = ang >> 4;
                pos.y += 0x32000;
                pos.x = data_02082214[i * 2] * mag + pos.x;
                pos.z = data_02082214[i * 2 + 1] * mag + pos.z;
                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(self->mPounder, self, 0xa5, &pos, 0, 0) == 0)
                    return;
                (*(u8 *)&self->mSubState)++;
                func_0201267c(0x133, &self->mCamSpacePosX);
            }
            return;
        case 2:
            if (_ZN6Player12GetTalkStateEv(self->mPounder) != -1)
                return;
            (*(u8 *)&self->mSubState)++;
            return;
        case 3:
            self->mState = 9;
            starPos.x = self->mPosX;
            starPos.y = self->mPosY;
            starPos.z = self->mPosZ;
            starPos.y += 0x64000;
            func_0201267c(0xc6, &self->mCamSpacePosX);
            _ZN5Sound22StopLoadedMusic_Layer3Ev();
            func_02011cfc();
            func_ov079_02123d4c((int *)&v, self);
            v.y += 0x50000;
            dp.x = v.x;
            dp.y = v.y;
            dp.z = v.z;
            _ZN8dActor_c16TriplePoofDustAtERK7Vector3(self, &dp);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(4, v.x, v.y, v.z);
            self->mShouldRender = 0;
            _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(self, (signed char *)&self->unk_408, self->mStarID, &starPos, 4);
            if (_ZN4dBgW9IsEnabledEv(&self->mMovingMeshCollider) != 0)
                _ZN4dBgW7DisableEv(&self->mMovingMeshCollider);
            _ZN7fBase_c18MarkForDestructionEv(self);
            return;
        }
    } else {
        func_0201267c(0xc6, &self->mCamSpacePosX);
        func_ov079_02123d4c((int *)&v2, self);
        v2.y += 0x28000;
        dp2.x = v2.x;
        dp2.y = v2.y;
        dp2.z = v2.z;
        _ZN8dActor_c10PoofDustAtERK7Vector3(self, &dp2);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(5, v2.x, v2.y, v2.z);
        _ZN7fBase_c18MarkForDestructionEv(self);
    }
}

// @symbol func_ov079_021246d8
/* State 9: the king has been beaten and is waiting to be destroyed. */
extern "C" void func_ov079_021246d8(void)
{
}

// @symbol func_ov079_02124638
/* State 10, the hit wobble: rock about the pitch axis for 10 of the 90
   frames the hit armed, then sit level, then put back the state the hit
   interrupted. */
extern "C" void func_ov079_02124638(daBtn_c *self)
{
    u8 old = self->mWobbleTimer;
    u8 *timer = &self->mWobbleTimer;
    u8 v = *timer;
    *timer = (u8)(v - 1);
    if (old != 0) {
        u32 t = self->mWobbleTimer;
        if (t <= 0x50) {
            self->mAngleX = 0;
            return;
        }
        {
            s32 amp = (s32)((t - 0x50) << 6);
            u16 ang = (u16)(s16)(t << 13);
            self->mAngleX = (s16)(((s64)amp * data_02082214[(ang >> 4) * 2] + 0x800) >> 12);
        }
        return;
    }
    self->mState = self->mResumeState;
    if (self->mState == 7)
        self->mResumeState = 10;
    self->mResumePending = 0;
}

// @symbol func_ov079_02124530
/* State 11, knocked over from behind: finish the hop, then tip forward like
   state 4 with variant-specific pitch acceleration until flat, landing in
   state 5. While the hop plays the body rises. */
extern "C" void func_ov079_02124530(daBtn_c *self)
{
    self->mModelAnim.Advance();
    if (self->mModelAnim.Finished()) {
        s16 *pitch = &self->mAngleX;
        if (self->mIsKing) { s32 *speed = &self->mPitchSpeed; *speed = *speed + 0xe0; }
        else { s32 *speed = &self->mPitchSpeed; *speed = *speed + 0x140; }
        *pitch = *pitch + self->mPitchSpeed;
        if (self->mAngleX > 0x4000) {
            self->mPitchSpeed = 0;
            self->mAngleX = 0x4000;
            self->mState = 5;
            func_0201267c(0xc6, &self->mCamSpacePosX);
        }
        if (self->mIsKing) {
            self->mTipLift = (s16)self->mAngleX * (s16)0x15;
            if (self->mTipLift >= 0x53000)
                self->mTipLift = 0x53000;
        } else {
            self->mTipLift = (s16)self->mAngleX * (s16)0xa;
            if (self->mTipLift >= 0x25000)
                self->mTipLift = 0x25000;
        }
        return;
    }
    {
        s32 *y = &self->mPosY;
        *y = *y + ((self->mIsKing + 1) << 15);
    }
}

// @symbol func_ov079_021243e0
/* Keep the mesh collider enabled only near a player: always for a hidden
   body it is off, for the king outside state 0 it is on, otherwise it is on
   while the closest player is within `range` (default: eight clip radii
   from the clip centre). Returns 1 when the collider was left off. */
extern "C" int func_ov079_021243e0(daBtn_c *self, int range)
{
    BtnVec v;
    Player *player;
    int dist;
    int y;

    if (self->mShouldRender == 0) {
        if (self->mMovingMeshCollider.IsEnabled())
            self->mMovingMeshCollider.Disable();
        return 1;
    }

    if (self->mIsKing != 0 && self->mState != 0) {
        if (!self->mMovingMeshCollider.IsEnabled())
            self->mMovingMeshCollider.Enable(self);
        return 0;
    }

    v.x = self->mPosX;
    y = self->mPosY;
    v.y = y;
    v.z = self->mPosZ;
    if (range == 0) {
        int radius = self->mClipRadius;
        v.y = y + self->mClipOffsetY;
        range = radius << 3;
    }

    player = self->ClosestPlayer();
    dist = Vec3_Dist(&v, &player->mPosX);
    if (dist > range) {
        if (self->mMovingMeshCollider.IsEnabled())
            self->mMovingMeshCollider.Disable();
        return 1;
    }

    if (!self->mMovingMeshCollider.IsEnabled())
        self->mMovingMeshCollider.Enable(self);
    return 0;
}

#define FX12(a,b) (int)(((long long)(a) * (int)(b) + 0x800) >> 12)

// @symbol func_ov079_02124188
/* Pose the model from the actor (raised by mTipLift), read the first
   bone's rotation back into mBodyPitch/Yaw/Roll, and drop the shadow: its
   matrix sits under the body's centre, pushed out along the tilt, turned
   to the body's heading. */
extern "C" void func_ov079_02124188(daBtn_c *self)
{
    BtnVec pos;
    BtnVec asr;
    int shadowRad;
    int cosv;
    int sinComp;
    char *bone;

    Matrix4x3_FromRotationXYZExt(&self->mModelAnim.mat4x3,
        self->mAngleX, self->mAngleY, self->mAngleZ);

    self->mModelAnim.mat4x3.t.x = self->mPosX >> 3;
    self->mModelAnim.mat4x3.t.y = (self->mPosY + self->mTipLift) >> 3;
    self->mModelAnim.mat4x3.t.z = self->mPosZ >> 3;

    bone = (char *)self->mModelAnim.data.bones;
    pos.x = self->mPosX;
    pos.y = self->mPosY;
    pos.z = self->mPosZ;

    self->mBodyPitch = (short)-*(unsigned short *)(bone + 0x1c);
    self->mBodyYaw = *(unsigned short *)(bone + 0x1a);
    self->mBodyRoll = (short)(*(unsigned short *)(bone + 0x1e) - 0x4000);

    if (self->mIsKing != 0) {
        cosv = data_02082214[((unsigned short)self->mAngleX >> 4) << 1];
        sinComp = cosv * 0x190;
        shadowRad = cosv * 0x1a9 + 0xc8000;
    } else {
        cosv = data_02082214[((unsigned short)self->mAngleX >> 4) << 1];
        sinComp = cosv * 0xc8;
        shadowRad = cosv * 0xd2 + 0x64000;
    }

    pos.x += FX12(sinComp, data_02082214[((unsigned short)self->mAngleY >> 4) << 1]);
    pos.z += FX12(sinComp, data_02082214[(((unsigned short)self->mAngleY >> 4) << 1) + 1]);

    Vec3_Asr(&asr, &pos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68,
        (short)(self->mAngleY + self->mBodyYaw));

    *(MatrixWords *)self->mShadowMatrix = data_020a0e68;

    if (self->mShouldRender == 0)
        return;

    if (self->mIsKing != 0) {
        _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            self, &self->mShadowModel, self->mShadowMatrix, 0x1cc000, 0x190000, shadowRad, 0xf);
    } else {
        _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            self, &self->mShadowModel, self->mShadowMatrix, 0xf0000, 0x190000, shadowRad, 0xf);
    }
    return;
}

#undef FX12

// @symbol func_ov079_02124008
/* Pose the collision mesh: the model's matrix turned by the body's bone
   rotation (yaw always; pitch and half the roll except while walking,
   states 1 and 7), squashed vertically as the body tilts, and placed at the
   actor, raised by mTipLift and lowered again by the squash. */
extern "C" void func_ov079_02124008(daBtn_c *self)
{
    s16 a = (s16)(self->mAngleX + self->mBodyPitch);
    int s;
    u8 n = self->mIsKing;
    int off;

    if (a < 0) a *= -1;
    s = (s16)(0x4000 - a);

    if (n) {
        if (self->mState == 0) s = s / 32 + 0x1000;
        else s = s / 48 + 0x1000;
    } else {
        s = s / 102;
        s = s + 0x1000;
    }

    off = (n + 1) * ((s - 0x1000) * 0x168);
    *(MatrixWords *)self->mMeshMatrix = *(MatrixWords *)&self->mModelAnim.mat4x3;

    Matrix4x3_ApplyInPlaceToRotationY(self->mMeshMatrix, self->mBodyYaw);
    if (self->mState != 1 && self->mState != 7) {
        Matrix4x3_ApplyInPlaceToRotationX(self->mMeshMatrix, self->mBodyPitch);
        Matrix4x3_ApplyInPlaceToRotationZ(self->mMeshMatrix, (s16)(self->mBodyRoll >> 1));
    }
    Matrix4x3_ApplyInPlaceToScale(self->mMeshMatrix, 0x1000, s, 0x1000);

    self->mMeshMatrix[9] = self->mPosX;
    self->mMeshMatrix[10] = self->mPosY + self->mTipLift - off;
    self->mMeshMatrix[11] = self->mPosZ;

    self->mMovingMeshCollider.Transform(*(Matrix4x3 *)self->mMeshMatrix, self->mBodyYaw);
}

// @symbol func_ov079_02123f34
/* Survey the watched players: the nearest one (index, distance, bearing into
   mTargetAngle) and the nearest one ahead, within 0x2000 of the heading. */
extern "C" void func_ov079_02123f34(daBtn_c *self)
{
    s16 minAngle;
    int minDist, minDist2;
    int dist;
    int i;
    s16 angle;
    BtnVec v;

    self->mTargetAngle = self->mPrevAngleY;
    minAngle = self->mPrevAngleY;
    minDist2 = 0x7fffffff;
    minDist = 0x7fffffff;
    for (i = 0; i < 4; i++) {
        Player *player = self->mPlayers[i];
        BtnVec* p;
        if (player == 0)
            continue;
        p = (BtnVec*)&player->mPosX;
        v.x = p->x;
        v.y = p->y;
        v.z = p->z;
        dist = Vec3_HorzDist(&self->mPosX, &v);
        angle = Vec3_HorzAngle(&self->mPosX, &v);
        if (dist < minDist) {
            self->mNearestPlayer = i;
            minDist = dist;
            minAngle = angle;
        }
        if (self->GetSubtraction(angle, self->mPrevAngleY) < 0x2000) {
            if (dist < minDist2) {
                minDist2 = dist;
                self->mFrontPlayer = i;
            }
        }
    }
    self->mNearestDist = minDist;
    self->mFrontDist = minDist2;
    self->mTargetAngle = minAngle;
}

// @symbol _ZN7daBtn_c15OnHitByMegaCharER6Player
void daBtn_c::OnHitByMegaChar(Player &player)
{
    if (mIsKing != 0) return;
    if (mDeathState == 8) return;
    player.IncMegaKillCount();
    func_02012694(0x1e, &mCamSpacePosX);
    Vector3 pos;
    func_ov079_02123d4c((int *)&pos, this);
    Vector3 dust;
    pos.y = pos.y + 0x28000;
    dust.x = pos.x;
    dust.y = pos.y;
    dust.z = pos.z;
    PoofDustAt(dust);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(5, pos.x, pos.y, pos.z);
    MarkForDestruction();
    if (mMovingMeshCollider.IsEnabled() != 0)
        mMovingMeshCollider.Disable();
}

// @symbol func_ov079_02123d4c
/* The point eggs aim at and dust bursts from: the body's centre, pushed out
   along the tilt (twice as far for the king) and raised by
   OnAimedAtWithEgg's height. OnAimedAtWithEggReturnVec returns exactly this. */
extern "C" void func_ov079_02123d4c(int* out, daBtn_c *self){
    out[0] = self->mPosX;
    out[1] = self->mPosY;
    out[2] = self->mPosZ;
    {
        int m;
        int d;
        if (self->mIsKing) {
            m = (s16)data_02082214[((unsigned short)self->mAngleX >> 4) * 2] * (s16)0x190;
        } else {
            m = (s16)data_02082214[((unsigned short)self->mAngleX >> 4) * 2] * (s16)0xc8;
        }
        out[0] = out[0] + (int)(((s64)m * data_02082214[((unsigned short)self->mAngleY >> 4) * 2] + 0x800) >> 12);
        d = self->OnAimedAtWithEgg();
        {
            int* p1 = (int*)(((int)out + 4));
            int* p2 = (int*)(((int)out + 8));
            *p1 = *p1 + d;
            *p2 = *p2 + (int)(((s64)m * data_02082214[((unsigned short)self->mAngleY >> 4) * 2 + 1] + 0x800) >> 12);
        }
    }
}

// @symbol func_ov079_02123bcc
/* The king shoves the nearest player aside: if that player is no higher
   than the king and stands inside the band ahead of it (within 0x3c000
   sideways and 0xe1000 along the line to them, measured on the bearing
   mTargetAngle), move them 5 units along the body's heading. Returns 1 when
   it did. */
extern "C" int func_ov079_02123bcc(daBtn_c *self) {
    int v[6];
    int dist, rangle, idx, hc, hs, idx2, nx, nz;
    Player *player; s32 *p; s16 cosv, sinv, sin2, cos2;
    int a0, a1, a2, raw, ang;
    player = self->mPlayers[self->mNearestPlayer];
    p = &player->mPosX;
    v[0] = p[0]; v[1] = p[1]; v[2] = p[2];
    if (v[1] > self->mPosY) return 0;
    dist = Vec3_HorzDist(v, &self->mPosX);
    a0 = self->mAngleY; a1 = self->mBodyYaw; a2 = self->mTargetAngle;
    raw = (a0 + a1) << 16; ang = a2 + (raw >> 16); rangle = raw >> 16;
    idx = (u16)(s16)ang >> 4; cosv = data_02082214[idx*2+1];
    hc = (int)(((s64)dist * cosv + 0x800) >> 12);
    if (hc < 0) hc = -hc;
    if (hc <= 0x3c000) {
        sinv = data_02082214[idx*2];
        hs = (int)(((s64)dist * sinv + 0x800) >> 12);
        if (hs < 0) hs = -hs;
        if (hs <= 0xe1000) {
            int vx, vy, vz;
            int i;
            idx2 = (u16)rangle >> 4;
            sin2 = data_02082214[idx2*2]; cos2 = data_02082214[idx2*2+1];
            vx = v[0]; vy = v[1]; vz = v[2];
            nx = (s16)sin2 * 5 + vx;
            nz = (s16)cos2 * 5 + vz;
            i = self->mNearestPlayer;
            player = self->mPlayers[i];
            player->mPosX = nx;
            player->mPosY = vy;
            player->mPosZ = nz;
            return 1;
        }
    }
    return 0;
}

// @symbol _ZN7daBtn_c16OnAimedAtWithEggEv
s32 daBtn_c::OnAimedAtWithEgg() {
    int idx;
    if(mIsKing!=0){
        idx = ((int)(unsigned short)mAngleX >> 4);
        return data_02082214[idx*2+1] * 0x11d + 0x4b000;
    }
    idx = ((int)(unsigned short)mAngleX >> 4);
    return data_02082214[idx*2+1] * 0x90 + 0x24000;
}

// @symbol _ZN7daBtn_c25OnAimedAtWithEggReturnVecEv
Vector3 daBtn_c::OnAimedAtWithEggReturnVec()
{
    return vecret::func_ov079_02123d4c(this);
}

// @symbol func_ov079_02123a8c
/* The mesh collider also stays on while the nearest actor of ID 9 is within
   eight clip radii of the egg-aim point. Returns 1 when it did. */
extern "C" int func_ov079_02123a8c(daBtn_c *self)
{
    Vector3 v;

    if (self->mShouldRender == 0)
        return 0;

    dActor_c *closest = self->ClosestWithActorID(9);
    if (closest) {
        v.x = self->mPosX;
        v.y = self->mPosY;
        v.z = self->mPosZ;
        v.y = v.y + self->OnAimedAtWithEgg();
        int dist = Vec3_Dist(&v, &closest->mPosX);
        if (dist < (self->mClipRadius << 3)) {
            if (!self->mMovingMeshCollider.IsEnabled())
                self->mMovingMeshCollider.Enable(self);
            return 1;
        }
    }
    return 0;
}

// @symbol func_ov079_02123804
/* A hit from `other`, judged by where it stood a frame ago (pulled back
   0x50 along its heading) against the body's facing. From the front (within
   0x3c00) the body wobbles (state 10); from behind (beyond 0x4400) it drops
   a coin at the attacker and is knocked over (state 11). Only a walking or
   turning body reacts (states 1, 2, 7, 10), or a waiting one (0) -- for the
   king only before its introduction starts. The interrupted state is saved
   for resuming. Nothing in the cartridge's relocations calls this. */
extern "C" void func_ov079_02123804(daBtn_c *self, dActor_c *other)
{
    BtnVec v;
    int idx;
    s16 cosv, sinv;
    int ang;

    {
        int* q = &other->mPrevPosX;
        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
    }

    idx = (unsigned short)other->mPrevAngleY >> 4;
    cosv = data_02082214[idx * 2];
    sinv = data_02082214[idx * 2 + 1];
    v.x = v.x - cosv * 0x50;
    v.z = v.z - sinv * 0x50;

    ang = Vec3_HorzAngle(&self->mPosX, &v);

    if (AngleDiff(ang, (s16)(self->mAngleY + self->mBodyYaw)) < 0x3c00) {
        int r1 = self->mState;
        if (r1 == 1 || r1 == 2 || r1 == 7 || r1 == 0xa) goto action1;
        if (r1 != 0) return;
        if (self->mIsKing != 0) {
            if (self->mSubState != 0) return;
        }
    action1:
        if (self->mState != 0xa) {
            self->mResumeState = *(volatile s32 *)&self->mState;
            self->mSavedStateTimer = *(u16 *)&self->mStateTimer;
            self->mSavedSubState = self->mSubState;
        }
        self->mWobbleTimer = 0x5a;
        self->mHorzSpeed = 0;
        self->mState = 0xa;
        self->mResumePending = 1;
        return;
    }

    if (AngleDiff(ang, (s16)(self->mAngleY + self->mBodyYaw)) <= 0x4400) return;

    {
        int r1 = self->mState;
        if (r1 == 1 || r1 == 2 || r1 == 7 || r1 == 0xa) goto action2;
        if (r1 != 0) return;
        if (self->mIsKing != 0) {
            if (self->mSubState != 0) return;
        }
    action2:
        if (self->mState != 0xa) {
            self->mResumeState = *(volatile s32 *)&self->mState;
            self->mSavedStateTimer = *(u16 *)&self->mStateTimer;
            self->mSavedSubState = self->mSubState;
        }
        {
            BtnVec pos;
            int* q = &other->mPosX;
            pos.x = q[0];
            pos.y = q[1];
            pos.z = q[2];
            _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &pos, 1, 0, 0);
        }
        self->mHorzSpeed = 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &self->mModelAnim,
            ((BtnFileRef *)data_ov079_021275ec[self->mIsKing * 5])->file,
            0x40000000, 0x1000, 0);
        self->mModelAnim.speed = 0x4000;
        self->mState = 0xb;
        self->mResumePending = 1;
        return;
    }
}

// @symbol _ZN7daBtn_cD1Ev
// @symbol _ZN7daBtn_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
