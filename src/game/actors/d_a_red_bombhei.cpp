//cpp
/* Bob-omb Buddy (RED_BOMBHEI), ov084 .text 0x0212c10c..0x0212d248,
 * twenty-three functions.
 *
 * Stands in the level turning to face the nearest player. When a player walks
 * into its talk cylinder it starts a conversation. The red-coin variant picks
 * its message from the red coins collected; the cannon variants, while the
 * level's cannon is still closed, run a short cutscene that swings the camera
 * over to the cannon shutter, opens it, waits for it to finish and swings the
 * camera back.
 *
 * NAME: daRedBombhei_c is the cartridge's RTTI spelling (_ZTI14daRedBombhei_c
 * at 0x021309f4, _ZTS at 0x02130a00, vtable address point 0x02130a38). The
 * tree called the class BobOmbBuddy until then. daRedBombhei_c_classInit is
 * reconstructed (RTTI daRedBombhei_c, RED_BOMBHEI registry); retail does not
 * store that spelling. Historical alias: BobOmbBuddy_Spawn.
 *
 * Behavior runs one of three states out of a pointer-to-member table in .bss
 * at 0x02130dc4 that this module's static initializer (__sinit_ov084_02130558)
 * fills from the .data records at 0x021309c4. Each state is an {enter, update}
 * pair:
 *
 *   0 func_ov084_0212c92c / func_ov084_0212c8b0  idle: wait for a talk
 *   1 func_ov084_0212c89c / func_ov084_0212c508  talk
 *   2 func_ov084_0212c4a0 / func_ov084_0212c1a0  open the cannon
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so
 * the highest-address ROM function, the factory, is written FIRST here. Do
 * not reorder.
 *
 * The state bodies and helpers are daRedBombhei_c members under their
 *   address names. The state table reaches the state bodies only by
 *   address, through the .data words the static initializer copies into
 *   it.
 * Leftover: ModelAnim::SetAnim, dCcAc_c::Init and
 *   dActor_c::DropShadowRadHeight stay mangled; each takes Fix12<int> by
 *   value (notes/mwccarm-codegen.md 6az). Player::ShowMessage and
 *   dCamera_c::SetFlag_3 stay mangled too; the reasons are at their
 *   declarations.
 * Leftover: the two shared files (0x02130da4 model, 0x02130d9c animation)
 *   and the state table live in .bss this text-only TU does not own, and keep
 *   their address names.
 */

#include "daRedBombhei_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "dCamera_c.h"
#include "dBgCh_Gnd.h"
#include "decl_SaveData.h"
#include "daObjCannonShutter_c.h"

struct BCA_File;

/* A flat view of Matrix4x3 for the one whole-matrix copy below. The
   translation-vector spelling in math/Matrix.h lowers the same assignment to
   two ldm/stm pairs plus three word copies; the cartridge has three pairs. */
struct RedBombheiMtx { s32 m[12]; };

enum {
    kPlayerActorID        = 0xbf,
    kCannonShutterActorID = 0x0e,
};

/* The low byte of the spawn X angle picks what this buddy does. */
enum {
    kVariantCannonPan = 1,  /* opens the cannon; the camera pans to the shutter */
    kVariantCannonCut = 2,  /* opens the cannon; the camera cuts to the shutter */
    kVariantRedCoins  = 3,  /* explains red coins */
};

enum {
    kStateIdle   = 0,
    kStateTalk   = 1,
    kStateCannon = 2,
};

/* The three character-specific spawn parameters: a buddy placed with one of
   these only appears for Mario, Luigi or Wario respectively. */
enum {
    kParamMario = 0xb26,
    kParamLuigi = 0xb27,
    kParamWario = 0xb28,
};

/* Behavior calls the current state through this member-pointer table. */
typedef void (daRedBombhei_c::*RedBombheiStatePMF)();
struct RedBombheiState {
    RedBombheiStatePMF enter;
    RedBombheiStatePMF update;
};

bool ApproachLinear(short &value, short target, short step);

extern SharedFilePtr data_ov084_02130da4;   /* the model */
extern SharedFilePtr data_ov084_02130d9c;   /* the idle animation */

extern "C" {

/* -- the state table __sinit_ov084_02130558 fills in -- */
extern RedBombheiState data_ov084_02130dc4[];

/* -- globals -- */
extern dCamera_c *data_0209f318;           /* the camera */
extern s8  data_0209f2f8;               /* current level */
extern u8  data_0209f220;
extern u8  data_0209f284;
extern u8  data_0209f288;
extern int data_0209caa0[];             /* save data; word 2 holds the progress flags */
extern u8  data_0209d660;               /* nonzero while a message box is open */
extern u8  data_0209d6bc;               /* message box page state */
extern u16 data_0209d6d4;               /* message on screen */
extern Matrix4x3 data_020a0e68;         /* the shared scratch matrix */
extern Matrix4x3 IDENTITY_MATRIX4X3;

/* -- other modules -- */
int  IsCannonOpenInCurLevel(void);
void OpenCannonInCurLevel(void);
int  ObjectMessageIDToActualMessageID(int);
int  IsStarCollected(int level, int star);
int  SublevelToLevel(int sublevel);
s8   NumRedCoins(void);
void func_02012694(unsigned int soundID, const Vector3 *camSpacePos);
unsigned int func_02012790(unsigned int soundID);

short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int   Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void  Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
void  Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
int   LenVec3(const Vector3 *v);
void  Vec3_MulScalarInPlace(Vector3 *v, int s);
void  MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *out);
void  Matrix4x3_FromRotationY(void *m, int angle);
int   Math_Function_0203b14c(int *value, int target, int a, int b, int c);
int   _ZN4cstd4fdivEii(int a, int b);

/* local extern: Player::ShowMessage takes its two flag bytes as unsigned
   char; the real member call re-masks the caller's word (measured: +1
   instruction in func_ov084_0212c9f0). */
int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(Player *self, fBase_c *actor, unsigned int msgID, const Vector3 *pos, unsigned int a, unsigned int b);

/* local extern: dCamera_c.h does not declare this member. */
void _ZN9dCamera_c9SetFlag_3Ev(dCamera_c *cam);

void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *thiz, BCA_File *file, int flags, int speed, unsigned short startFrame);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(dCcAc_c *self, dActor_c *actor, int radius, int height, unsigned int flags, unsigned int vulnFlags);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(dActor_c *self, dExtShadowModel_c *shadow, Matrix4x3 *mtx, int radius, int depth, unsigned char opacity);

}

// @symbol daRedBombhei_c_classInit
extern "C" daRedBombhei_c *daRedBombhei_c_classInit()
{
    return new daRedBombhei_c();
}

// @symbol _ZN14daRedBombhei_c13InitResourcesEv
s32 daRedBombhei_c::InitResources()
{
    Vector3 pos;

    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov084_02130da4);
    mModelAnim.SetFile(modelFile, 1, -1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(data_ov084_02130d9c);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x8c000, 0x8c000, 0x4200004, 0);
    func_ov084_0212c960(0);
    mShutterID = 0;

    {
        int z = mPosZ;
        int y = mPosY + 0x64000;
        pos.x = mPosX;
        pos.y = y;
        pos.z = z;
    }

    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn() != 0)
        mPosY = ground.clsnY;

    if (func_ov084_0212ca60() != 0) {
        u8 character = ClosestPlayer()->mCharacter;
        if ((character == 0 && param1 == kParamMario) ||
            (character == 1 && param1 == kParamLuigi) ||
            (character == 2 && param1 == kParamWario) ||
            _ZN8SaveData16HasPlayerLostCapEv())
            return 0;
    }

    if (func_ov084_0212cac0() != 0 && data_0209f2f8 == 8 &&
        (data_0209f220 == 1 || IsStarCollected(SublevelToLevel(8), 1) == 0))
        return 0;
    return 1;
}

// @symbol _ZN14daRedBombhei_c8BehaviorEv
/* Runs the state, keeps turning toward a player within 300.0, and plays
   sound 0xd7 at the start of each animation loop. */
s32 daRedBombhei_c::Behavior()
{
    Vector3 playerPos;
    Player *player;
    func_ov084_0212c9a8();
    player = ClosestPlayer();
    if (player != 0) {
        Vector3 *src = (Vector3 *)&player->mPosX;
        playerPos = *src;
        if (Vec3_HorzDist((Vector3 *)&mPosX, &playerPos) < 0x12c000) {
            short angle = Vec3_HorzAngle((Vector3 *)&mPosX, &playerPos);
            ApproachLinear(mAngleY, angle, 0x100);
        }
    }
    mModelAnim.Advance();
    if ((unsigned short)(mModelAnim.currFrame >> 12) == 0)
        func_02012694(0xd7, (Vector3 *)&mCamSpacePosX);
    func_ov084_0212ce50();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN14daRedBombhei_c6RenderEv
s32 daRedBombhei_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN14daRedBombhei_c16CleanupResourcesEv
s32 daRedBombhei_c::CleanupResources()
{
    data_ov084_02130da4.Release();
    data_ov084_02130d9c.Release();
    return 1;
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212ce50Ev
/* Places the model and drops the shadow under the body. */
void daRedBombhei_c::func_ov084_0212ce50()
{
    Matrix4x3_FromRotationY(&this->mModelAnim.mat4x3, this->mAngleY);
    this->mModelAnim.mat4x3.t.x = this->mPosX >> 3;
    this->mModelAnim.mat4x3.t.y = (this->mPosY + 0x4000) >> 3;
    this->mModelAnim.mat4x3.t.z = this->mPosZ >> 3;
    *(RedBombheiMtx *)&this->mShadowMat = *(RedBombheiMtx *)&IDENTITY_MATRIX4X3;
    this->mShadowMat.t.x = this->mPosX >> 3;
    this->mShadowMat.t.y = (this->mPosY - 0x8000) >> 3;
    this->mShadowMat.t.z = this->mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(this, &this->mShadowModel, &this->mShadowMat, 0x64000, 0x32000, 0xf);
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212cda0EP7Vector3S1_
/* Moves the camera point *cur one step toward target, easing the distance
   between them down toward zero. Returns 1 once it has arrived. */
int daRedBombhei_c::func_ov084_0212cda0(Vector3 *cur, Vector3 *target)
{
    Vector3 delta;
    int len;
    int remaining;
    int newLen;
    Vec3_Sub(&delta, cur, target);
    len = LenVec3(&delta);
    if (len == 0) return 1;
    newLen = len;
    remaining = Math_Function_0203b14c(&newLen, 0, 0x400, 0x100000, 0x1000);
    Vec3_MulScalarInPlace(&delta, _ZN4cstd4fdivEii(newLen, len));
    {
        Vector3 next;
        Vec3_Add(&next, target, &delta);
        cur->x = next.x;
        cur->y = next.y;
        cur->z = next.z;
    }
    return remaining == 0 ? 1 : 0;
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212ccb4Ev
/* Cutscene step 3: pan the camera back to where it was before the cutscene.
   Returns 1 once both points have arrived. */
int daRedBombhei_c::func_ov084_0212ccb4()
{
    dCamera_c *cam = data_0209f318;
    int posDone;
    Vector3 lookAt;
    Vector3 *camLookAt = &cam->lookAt;
    Vector3 *camPos = &cam->pos;
    Vector3 pos;
    lookAt.x = camLookAt->x;
    lookAt.y = camLookAt->y;
    lookAt.z = camLookAt->z;
    pos.x = camPos->x;
    pos.y = camPos->y;
    pos.z = camPos->z;
    int lookAtDone = func_ov084_0212cda0(&lookAt, &this->mSavedCamLookAt);
    posDone = func_ov084_0212cda0(&pos, &this->mSavedCamPos);
    if ((this->mAngleX & 0xff) == kVariantCannonCut) {
        lookAtDone = 1;
        posDone = 1;
        cam->SetLookAt(this->mSavedCamLookAt);
        cam->SetPos(this->mSavedCamPos);
    } else {
        cam->SetLookAt(lookAt);
        cam->SetPos(pos);
    }
    if (lookAtDone != 0 && posDone != 0) return 1;
    return 0;
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212cae0Ev
/* Cutscene step 1: pan the camera over to the cannon shutter -- looking at a
   point 128.0 up and 10.0 behind it from 512.0 up and 200.0 in front. Returns 1
   once both points have arrived. */
int daRedBombhei_c::func_ov084_0212cae0()
{
    int posDone, lookAtDone;
    dCamera_c *cam;
    dActor_c *shutter;
    Vector3 lookAt, pos, targetLookAt, targetPos, offset, sum, sum2;
    unsigned id = this->mShutterID;
    cam = data_0209f318;
    if (id != 0) {
        shutter = dActor_c::FindWithID(id);
        if (shutter != 0) goto body;
    }
    goto end;
body:
    lookAtDone = 0;
    targetLookAt.x = lookAtDone; targetLookAt.y = 0x80000;  targetLookAt.z = -0xa000;
    targetPos.x = lookAtDone;    targetPos.y = 0x200000;    targetPos.z = 0xc8000;
    offset.x = lookAtDone; offset.y = lookAtDone; offset.z = lookAtDone;
    Matrix4x3_FromRotationY(&data_020a0e68, shutter->mAngleY);
    MulVec3Mat4x3(&targetLookAt, &data_020a0e68, &offset);
    Vec3_Add(&sum, (Vector3 *)&shutter->mPosX, &offset);
    {
        int dx = sum.x; int dy = sum.y; int z = lookAtDone;
        *(volatile int *)&offset.x = z; *(volatile int *)&targetLookAt.x = dx;
        *(volatile int *)&offset.y = z; *(volatile int *)&targetLookAt.y = dy;
        int dz = *(volatile int *)&sum.z;
        *(volatile int *)&offset.z = z; *(volatile int *)&targetLookAt.z = dz;
    }
    MulVec3Mat4x3(&targetPos, &data_020a0e68, &offset);
    Vec3_Add(&sum2, (Vector3 *)&shutter->mPosX, &offset);
    {
        int ey = sum2.y; int ex = sum2.x;
        targetPos.y = ey;
        int ez = sum2.z;
        targetPos.x = ez ? ex : ex;
        targetPos.z = ez;
    }
    {
        int *camLookAt = (int *)(unsigned)&cam->lookAt;
        lookAt.x = camLookAt[0];
        {
            int *camPos = (int *)(unsigned)&cam->pos;
            lookAt.y = camLookAt[1]; lookAt.z = camLookAt[2];
            pos.x = camPos[0]; pos.y = camPos[1]; pos.z = camPos[2];
        }
    }
    lookAtDone = func_ov084_0212cda0(&lookAt, &targetLookAt);
    posDone = func_ov084_0212cda0(&pos, &targetPos);
    if ((this->mAngleX & 0xff) == kVariantCannonCut) {
        lookAtDone = 1; posDone = 1;
        cam->SetLookAt(targetLookAt);
        cam->SetPos(targetPos);
    } else {
        cam->SetLookAt(lookAt);
        cam->SetPos(pos);
    }
end:
    return (lookAtDone != 0 && posDone != 0) ? 1 : 0;
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212cac0Ev
/* Is this the buddy that opens the cannon? */
int daRedBombhei_c::func_ov084_0212cac0()
{
    int variant = this->mAngleX;
    int result = 1;
    variant = variant & 0xff;
    if (variant == kVariantCannonPan) return result;
    if (variant != kVariantCannonCut) result = 0;
    return result;
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212caa8Ev
/* Is this the red-coin tutor? */
int daRedBombhei_c::func_ov084_0212caa8()
{
    return (this->mAngleX & 0xff) == kVariantRedCoins;
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212ca60Ev
/* Was this buddy placed for one character only? */
int daRedBombhei_c::func_ov084_0212ca60()
{
    int result = 1;
    int isMarioOrLuigi = result;
    int param = this->param1;
    if (param != kParamMario) {
        if (param != kParamLuigi)
            isMarioOrLuigi = 0;
    }
    if (!isMarioOrLuigi) {
        if (param != kParamWario)
            result = 0;
    }
    return result;
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212c9f0Eij
/* Opens message msgID on the talking player's screen, with the talk sound. */
void daRedBombhei_c::func_ov084_0212c9f0(int msgID, unsigned int msgFlag)
{
    Player *player = this->mTalkPlayer;
    Vector3 pos;
    int x = this->mPosX;
    int z = this->mPosZ;
    int y = this->mPosY + 0x32000;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    func_02012694(0x108, (Vector3 *)&this->mCamSpacePosX);
    _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(player, this, (s16)msgID, &pos, msgFlag, 0);
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212c9a8Ev
void daRedBombhei_c::func_ov084_0212c9a8()
{
    int j = this->mState;
    (this->*data_ov084_02130dc4[j].update)();
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212c960Ei
void daRedBombhei_c::func_ov084_0212c960(int state)
{
    this->mState = state;
    int j = this->mState;
    (this->*data_ov084_02130dc4[j].enter)();
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212c92cEv
void daRedBombhei_c::func_ov084_0212c92c()
{
    unsigned int flags = 0;
    BCA_File *file = (BCA_File *)(((int *)&data_ov084_02130d9c)[1]);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, file, 0, 0x1000, flags);
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212c8b0Ev
/* State 0 update: start talking when a player walks into the talk cylinder. */
void daRedBombhei_c::func_ov084_0212c8b0()
{
    dActor_c *other;
    if ((this->mdCcAc_c.hitFlags & 0x8000000) == 0)
        return;
    other = dActor_c::FindWithID(this->mdCcAc_c.otherOwner);
    if (other == 0)
        return;
    {
        int isPlayer = (other->actorID == kPlayerActorID);
        if (isPlayer == 0)
            return;
    }
    this->mTalkPlayer = (Player *)other;
    if (this->mTalkPlayer->StartTalk(*this, false) == 0)
        return;
    func_ov084_0212c960(kStateTalk);
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212c89cEv
/* State 1 enter. */
void daRedBombhei_c::func_ov084_0212c89c()
{
    this->mMsgPage = 0;
    this->mPrevMsgPageState = 0;
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212c508Ev
/* State 1 update: turn to the player, open the right message, and track the
   message box to light the hint arrow on the pages that point somewhere. */
void daRedBombhei_c::func_ov084_0212c508()
{
    Player *player = this->mTalkPlayer;
    int msgID = 0;
    int param = this->param1;
    s16 angle;
    Vector3 playerPos;
    int msg;
    u16 shownMsg;
    if (param != 0xffff)
        msgID = (u16)param;
    {
        int *p = (int *)(unsigned)&player->mPosX;
        int a = p[0];
        playerPos.x = a;
        int b = p[1];
        playerPos.y = b;
        int c = p[2];
        playerPos.z = c;
    }
    angle = Vec3_HorzAngle((Vector3 *)&this->mPosX, &playerPos);

    if (func_ov084_0212cac0() != 0) {
        if (IsCannonOpenInCurLevel() == 0) {
            func_ov084_0212c960(kStateCannon);
            return;
        }
    }

    switch (player->GetTalkState()) {
    case 0:
        if (ApproachLinear(this->mAngleY, angle, 0x800) != 0) {
            if (func_ov084_0212caa8() != 0) {
                if ((data_0209caa0[2] & 0x8000) == 0) {
                    data_0209caa0[2] |= 0x8000;
                    msg = 0x15b;
                } else if (NumRedCoins() <= 3) {
                    msg = 0x15c;
                } else if (NumRedCoins() <= 6) {
                    msg = 0x15d;
                } else if (NumRedCoins() == 7) {
                    msg = 0x15e;
                } else {
                    msg = 0x15f;
                }
                func_ov084_0212c9f0(msg, 0);
                data_0209f288 = 1;
            } else if (func_ov084_0212cac0() != 0 && IsCannonOpenInCurLevel() != 0) {
                if (this->mCannonOpened == 0) {
                    if (data_0209f2f8 == 6)
                        msg = 0x8f;
                    else
                        msg = 0x14a;
                } else {
                    if (data_0209f2f8 == 6)
                        msg = 0x90;
                    else
                        msg = 0x14b;
                }
                this->mCannonOpened = 1;
                func_ov084_0212c9f0(msg, 0);
            } else {
                msg = ObjectMessageIDToActualMessageID((int)(s16)msgID);
                msg = msg + player->param1;
                func_ov084_0212c9f0((u16)msg, 0);
            }
        }
        break;
    case 1:
        break;
    default:
        func_ov084_0212c960(kStateIdle);
        break;
    }

    if (data_0209d660 == 0)
        return;

    if (data_0209d6bc == 9) {
        if (func_ov084_0212caa8() != 0)
            data_0209f288 = 0;
    }

    {
        u8 pageState = data_0209d6bc;
        if (this->mPrevMsgPageState != pageState) {
            if (pageState == 3)
                goto do_inc;
            if (pageState != 9)
                goto skip_inc;
        do_inc:
            {
                u8 *page = &this->mMsgPage;
                *page = (u8)(*page + 1);
            }
        skip_inc:
            ;
        }
    }

    shownMsg = data_0209d6d4;
    if (shownMsg == 0x15c || shownMsg == 0x15e || func_ov084_0212ca60() != 0) {
        if (this->mMsgPage == 0)
            data_0209f284 = 1;
        else
            data_0209f284 = 0;
    }

    if (shownMsg == 0x15b || shownMsg == 0x15d) {
        if (this->mMsgPage == 1)
            data_0209f284 = 1;
        else
            data_0209f284 = 0;
    }

    if (shownMsg == 0x8a) {
        if (this->mMsgPage == 1)
            data_0209f284 = 1;
        if (data_0209d6bc == 9)
            data_0209f284 = 0;
    }

    if (this->mMsgHint != data_0209f284 && data_0209f284 != 0)
        func_02012790(0x24);

    this->mMsgHint = data_0209f284;
    this->mPrevMsgPageState = data_0209d6bc;
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212c4a0Ev
/* State 2 enter: find this level's closed cannon shutter. */
void daRedBombhei_c::func_ov084_0212c4a0()
{
    dActor_c *actor;
    actor = dActor_c::FindWithActorID(kCannonShutterActorID, 0);
    while (actor) {
        if ((actor->param1 & 0xff) == 1) {
            if (((daObjCannonShutter_c *)actor)->mCannonOpen == 0) {
                this->mShutterID = actor->uniqueID;
            }
        }
        actor = dActor_c::FindWithActorID(kCannonShutterActorID, actor);
    }
    this->mCutsceneStep = 0;
}

// @symbol _ZN14daRedBombhei_c19func_ov084_0212c1a0Ev
/* State 2 update: the cannon-opening cutscene, one step at a time. */
void daRedBombhei_c::func_ov084_0212c1a0()
{
    Player *player = this->mTalkPlayer;
    dCamera_c *cam = data_0209f318;
    Vector3 playerPos;
    s16 angle;
    int *src = &player->mPosX;

    playerPos.x = src[0];
    playerPos.y = src[1];
    playerPos.z = src[2];
    angle = Vec3_HorzAngle((Vector3 *)&this->mPosX, &playerPos);

    switch (this->mCutsceneStep) {
    case 0:
        /* Show the cannon message; on talk state 2 save the camera and take it. */
        switch (player->GetTalkState()) {
        case 0: {
            int msg;
            if (ApproachLinear(this->mAngleY, angle, 0x800) == 0)
                return;
            msg = data_0209f2f8 == 6 ? 0x8f : 0x14a;
            if (this->mShutterID != 0)
                func_ov084_0212c9f0(msg, 1);
            else
                func_ov084_0212c9f0(msg, 0);
            return;
        }
        case 2: {
            int *lookAt = (int *)&cam->lookAt;
            int *pos = (int *)&cam->pos;
            this->mSavedCamLookAt.x = lookAt[0];
            this->mSavedCamLookAt.y = lookAt[1];
            this->mSavedCamLookAt.z = lookAt[2];
            this->mSavedCamPos.x = pos[0];
            this->mSavedCamPos.y = pos[1];
            this->mSavedCamPos.z = pos[2];
            _ZN9dCamera_c9SetFlag_3Ev(cam);
            this->mFlags &= ~1;
            this->mCutsceneStep += 1;
            return;
        }
        case -1:
            OpenCannonInCurLevel();
            this->mCannonOpened = 1;
            func_ov084_0212c960(kStateIdle);
            return;
        }
        return;
    case 1: {
        /* dCamera_c over to the shutter, then open it. */
        dActor_c *shutter;
        if (this->mShutterID == 0)
            return;
        if (func_ov084_0212cae0() == 0)
            return;
        shutter = dActor_c::FindWithID(this->mShutterID);
        if (shutter == 0)
            return;
        ((daObjCannonShutter_c *)shutter)->func_ov002_020bc990();
        this->mCutsceneStep += 1;
        return;
    }
    case 2: {
        /* Wait for the shutter to finish opening. */
        unsigned int id = this->mShutterID;
        if (id == 0)
            return;
        if (((daObjCannonShutter_c *)dActor_c::FindWithID(id))->mCannonOpen == 1)
            this->mCutsceneStep += 1;
        return;
    }
    case 3:
        /* dCamera_c back. */
        if (func_ov084_0212ccb4() != 0)
            this->mCutsceneStep += 1;
        return;
    case 4: {
        /* Finish the talk and hand the camera back. */
        int talkState = player->GetTalkState();
        if (talkState == 1)
            return;
        if (talkState == 2) {
            int msg = data_0209f2f8 == 6 ? 0x90 : 0x14b;
            func_ov084_0212c9f0(msg, 0);
            return;
        }
        this->mFlags |= 1;
        OpenCannonInCurLevel();
        this->mCannonOpened = 1;
        cam->mFlags &= ~8;
        func_ov084_0212c960(kStateIdle);
        return;
    }
    }
}

// @symbol _ZN14daRedBombhei_cD1Ev
// @symbol _ZN14daRedBombhei_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
