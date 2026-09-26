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
 * Leftover: the state bodies and helpers keep their func_ov084_* linker
 *   names as C-linkage functions over a daRedBombhei_c pointer. The state
 *   table reaches the state bodies only by address, through the .data words
 *   the static initializer copies into it.
 * Leftover: ModelAnim::SetAnim, dCcAc_c::Init and
 *   dActor_c::DropShadowRadHeight stay mangled; each takes Fix12<int> by
 *   value (notes/mwccarm-codegen.md 6az). Player::ShowMessage and
 *   Camera::SetFlag_3 stay mangled too; the reasons are at their
 *   declarations.
 * Leftover: the two shared files (0x02130da4 model, 0x02130d9c animation)
 *   and the state table live in .bss this text-only TU does not own, and keep
 *   their address names.
 */

#include "daRedBombhei_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Camera.h"
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

int ApproachLinear(short &value, short target, short step);

extern SharedFilePtr data_ov084_02130da4;   /* the model */
extern SharedFilePtr data_ov084_02130d9c;   /* the idle animation */

extern "C" {

/* Local helper declarations. */
void func_ov084_0212c960(daRedBombhei_c *self, int state);
void func_ov084_0212c9a8(daRedBombhei_c *self);
void func_ov084_0212c9f0(daRedBombhei_c *self, int msgID, unsigned int msgFlag);
int  func_ov084_0212ca60(daRedBombhei_c *self);
int  func_ov084_0212caa8(daRedBombhei_c *self);
int  func_ov084_0212cac0(daRedBombhei_c *self);
int  func_ov084_0212cae0(daRedBombhei_c *self);
int  func_ov084_0212ccb4(daRedBombhei_c *self);
int  func_ov084_0212cda0(daRedBombhei_c *self, Vector3 *cur, Vector3 *target);
void func_ov084_0212ce50(daRedBombhei_c *self);

/* -- the state table __sinit_ov084_02130558 fills in -- */
extern RedBombheiState data_ov084_02130dc4[];

/* -- globals -- */
extern Camera *data_0209f318;           /* the camera */
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
void func_ov002_020bc990(void *shutter);
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

/* local extern: Camera.h does not declare this member. */
void _ZN6Camera9SetFlag_3Ev(Camera *cam);

void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *thiz, BCA_File *file, int flags, int speed, unsigned short startFrame);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(dCcAc_c *self, dActor_c *actor, int radius, int height, unsigned int flags, unsigned int vulnFlags);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(dActor_c *self, ShadowModel *shadow, Matrix4x3 *mtx, int radius, int depth, unsigned char opacity);

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
    Animation::LoadFile(data_ov084_02130d9c);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x8c000, 0x8c000, 0x4200004, 0);
    func_ov084_0212c960(this, 0);
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

    if (func_ov084_0212ca60(this) != 0) {
        u8 character = ClosestPlayer()->mCharacter;
        if ((character == 0 && param1 == kParamMario) ||
            (character == 1 && param1 == kParamLuigi) ||
            (character == 2 && param1 == kParamWario) ||
            _ZN8SaveData16HasPlayerLostCapEv())
            return 0;
    }

    if (func_ov084_0212cac0(this) != 0 && data_0209f2f8 == 8 &&
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
    func_ov084_0212c9a8(this);
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
    func_ov084_0212ce50(this);
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

// @symbol func_ov084_0212ce50
/* Places the model and drops the shadow under the body. */
extern "C" void func_ov084_0212ce50(daRedBombhei_c *self)
{
    Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
    self->mModelAnim.mat4x3.t.x = self->mPosX >> 3;
    self->mModelAnim.mat4x3.t.y = (self->mPosY + 0x4000) >> 3;
    self->mModelAnim.mat4x3.t.z = self->mPosZ >> 3;
    *(RedBombheiMtx *)&self->mShadowMat = *(RedBombheiMtx *)&IDENTITY_MATRIX4X3;
    self->mShadowMat.t.x = self->mPosX >> 3;
    self->mShadowMat.t.y = (self->mPosY - 0x8000) >> 3;
    self->mShadowMat.t.z = self->mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(self, &self->mShadowModel, &self->mShadowMat, 0x64000, 0x32000, 0xf);
}

// @symbol func_ov084_0212cda0
/* Moves the camera point *cur one step toward target, easing the distance
   between them down toward zero. Returns 1 once it has arrived. */
extern "C" int func_ov084_0212cda0(daRedBombhei_c *self, Vector3 *cur, Vector3 *target)
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

// @symbol func_ov084_0212ccb4
/* Cutscene step 3: pan the camera back to where it was before the cutscene.
   Returns 1 once both points have arrived. */
extern "C" int func_ov084_0212ccb4(daRedBombhei_c *self)
{
    Camera *cam = data_0209f318;
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
    int lookAtDone = func_ov084_0212cda0(self, &lookAt, &self->mSavedCamLookAt);
    posDone = func_ov084_0212cda0(self, &pos, &self->mSavedCamPos);
    if ((self->mAngleX & 0xff) == kVariantCannonCut) {
        lookAtDone = 1;
        posDone = 1;
        cam->SetLookAt(self->mSavedCamLookAt);
        cam->SetPos(self->mSavedCamPos);
    } else {
        cam->SetLookAt(lookAt);
        cam->SetPos(pos);
    }
    if (lookAtDone != 0 && posDone != 0) return 1;
    return 0;
}

// @symbol func_ov084_0212cae0
/* Cutscene step 1: pan the camera over to the cannon shutter -- looking at a
   point 128.0 up and 10.0 behind it from 512.0 up and 200.0 in front. Returns 1
   once both points have arrived. */
extern "C" int func_ov084_0212cae0(daRedBombhei_c *self)
{
    int posDone, lookAtDone;
    Camera *cam;
    dActor_c *shutter;
    Vector3 lookAt, pos, targetLookAt, targetPos, offset, sum, sum2;
    unsigned id = self->mShutterID;
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
    lookAtDone = func_ov084_0212cda0(self, &lookAt, &targetLookAt);
    posDone = func_ov084_0212cda0(self, &pos, &targetPos);
    if ((self->mAngleX & 0xff) == kVariantCannonCut) {
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

// @symbol func_ov084_0212cac0
/* Is this the buddy that opens the cannon? */
extern "C" int func_ov084_0212cac0(daRedBombhei_c *self)
{
    int variant = self->mAngleX;
    int result = 1;
    variant = variant & 0xff;
    if (variant == kVariantCannonPan) return result;
    if (variant != kVariantCannonCut) result = 0;
    return result;
}

// @symbol func_ov084_0212caa8
/* Is this the red-coin tutor? */
extern "C" int func_ov084_0212caa8(daRedBombhei_c *self)
{
    return (self->mAngleX & 0xff) == kVariantRedCoins;
}

// @symbol func_ov084_0212ca60
/* Was this buddy placed for one character only? */
extern "C" int func_ov084_0212ca60(daRedBombhei_c *self)
{
    int result = 1;
    int isMarioOrLuigi = result;
    int param = self->param1;
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

// @symbol func_ov084_0212c9f0
/* Opens message msgID on the talking player's screen, with the talk sound. */
extern "C" void func_ov084_0212c9f0(daRedBombhei_c *self, int msgID, unsigned int msgFlag)
{
    Player *player = self->mTalkPlayer;
    Vector3 pos;
    int x = self->mPosX;
    int z = self->mPosZ;
    int y = self->mPosY + 0x32000;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    func_02012694(0x108, (Vector3 *)&self->mCamSpacePosX);
    _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(player, self, (s16)msgID, &pos, msgFlag, 0);
}

// @symbol func_ov084_0212c9a8
extern "C" void func_ov084_0212c9a8(daRedBombhei_c *self)
{
    int j = self->mState;
    (self->*data_ov084_02130dc4[j].update)();
}

// @symbol func_ov084_0212c960
extern "C" void func_ov084_0212c960(daRedBombhei_c *self, int state)
{
    self->mState = state;
    int j = self->mState;
    (self->*data_ov084_02130dc4[j].enter)();
}

// @symbol func_ov084_0212c92c
extern "C" void func_ov084_0212c92c(daRedBombhei_c *self)
{
    unsigned int flags = 0;
    BCA_File *file = (BCA_File *)(((int *)&data_ov084_02130d9c)[1]);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, file, 0, 0x1000, flags);
}

// @symbol func_ov084_0212c8b0
/* State 0 update: start talking when a player walks into the talk cylinder. */
extern "C" void func_ov084_0212c8b0(daRedBombhei_c *self)
{
    dActor_c *other;
    if ((self->mdCcAc_c.hitFlags & 0x8000000) == 0)
        return;
    other = dActor_c::FindWithID(self->mdCcAc_c.otherOwner);
    if (other == 0)
        return;
    {
        int isPlayer = (other->actorID == kPlayerActorID);
        if (isPlayer == 0)
            return;
    }
    self->mTalkPlayer = (Player *)other;
    if (self->mTalkPlayer->StartTalk(*self, false) == 0)
        return;
    func_ov084_0212c960(self, kStateTalk);
}

// @symbol func_ov084_0212c89c
/* State 1 enter. */
extern "C" void func_ov084_0212c89c(daRedBombhei_c *self)
{
    self->mMsgPage = 0;
    self->mPrevMsgPageState = 0;
}

// @symbol func_ov084_0212c508
/* State 1 update: turn to the player, open the right message, and track the
   message box to light the hint arrow on the pages that point somewhere. */
extern "C" void func_ov084_0212c508(daRedBombhei_c *self)
{
    Player *player = self->mTalkPlayer;
    int msgID = 0;
    int param = self->param1;
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
    angle = Vec3_HorzAngle((Vector3 *)&self->mPosX, &playerPos);

    if (func_ov084_0212cac0(self) != 0) {
        if (IsCannonOpenInCurLevel() == 0) {
            func_ov084_0212c960(self, kStateCannon);
            return;
        }
    }

    switch (player->GetTalkState()) {
    case 0:
        if (ApproachLinear(self->mAngleY, angle, 0x800) != 0) {
            if (func_ov084_0212caa8(self) != 0) {
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
                func_ov084_0212c9f0(self, msg, 0);
                data_0209f288 = 1;
            } else if (func_ov084_0212cac0(self) != 0 && IsCannonOpenInCurLevel() != 0) {
                if (self->mCannonOpened == 0) {
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
                self->mCannonOpened = 1;
                func_ov084_0212c9f0(self, msg, 0);
            } else {
                msg = ObjectMessageIDToActualMessageID((int)(s16)msgID);
                msg = msg + player->param1;
                func_ov084_0212c9f0(self, (u16)msg, 0);
            }
        }
        break;
    case 1:
        break;
    default:
        func_ov084_0212c960(self, kStateIdle);
        break;
    }

    if (data_0209d660 == 0)
        return;

    if (data_0209d6bc == 9) {
        if (func_ov084_0212caa8(self) != 0)
            data_0209f288 = 0;
    }

    {
        u8 pageState = data_0209d6bc;
        if (self->mPrevMsgPageState != pageState) {
            if (pageState == 3)
                goto do_inc;
            if (pageState != 9)
                goto skip_inc;
        do_inc:
            {
                u8 *page = &self->mMsgPage;
                *page = (u8)(*page + 1);
            }
        skip_inc:
            ;
        }
    }

    shownMsg = data_0209d6d4;
    if (shownMsg == 0x15c || shownMsg == 0x15e || func_ov084_0212ca60(self) != 0) {
        if (self->mMsgPage == 0)
            data_0209f284 = 1;
        else
            data_0209f284 = 0;
    }

    if (shownMsg == 0x15b || shownMsg == 0x15d) {
        if (self->mMsgPage == 1)
            data_0209f284 = 1;
        else
            data_0209f284 = 0;
    }

    if (shownMsg == 0x8a) {
        if (self->mMsgPage == 1)
            data_0209f284 = 1;
        if (data_0209d6bc == 9)
            data_0209f284 = 0;
    }

    if (self->mMsgHint != data_0209f284 && data_0209f284 != 0)
        func_02012790(0x24);

    self->mMsgHint = data_0209f284;
    self->mPrevMsgPageState = data_0209d6bc;
}

// @symbol func_ov084_0212c4a0
/* State 2 enter: find this level's closed cannon shutter. */
extern "C" void func_ov084_0212c4a0(daRedBombhei_c *self)
{
    dActor_c *actor;
    actor = dActor_c::FindWithActorID(kCannonShutterActorID, 0);
    while (actor) {
        if ((actor->param1 & 0xff) == 1) {
            if (((daObjCannonShutter_c *)actor)->mCannonOpen == 0) {
                self->mShutterID = actor->uniqueID;
            }
        }
        actor = dActor_c::FindWithActorID(kCannonShutterActorID, actor);
    }
    self->mCutsceneStep = 0;
}

// @symbol func_ov084_0212c1a0
/* State 2 update: the cannon-opening cutscene, one step at a time. */
extern "C" void func_ov084_0212c1a0(daRedBombhei_c *self)
{
    Player *player = self->mTalkPlayer;
    Camera *cam = data_0209f318;
    Vector3 playerPos;
    s16 angle;
    int *src = &player->mPosX;

    playerPos.x = src[0];
    playerPos.y = src[1];
    playerPos.z = src[2];
    angle = Vec3_HorzAngle((Vector3 *)&self->mPosX, &playerPos);

    switch (self->mCutsceneStep) {
    case 0:
        /* Show the cannon message; on talk state 2 save the camera and take it. */
        switch (player->GetTalkState()) {
        case 0: {
            int msg;
            if (ApproachLinear(self->mAngleY, angle, 0x800) == 0)
                return;
            msg = data_0209f2f8 == 6 ? 0x8f : 0x14a;
            if (self->mShutterID != 0)
                func_ov084_0212c9f0(self, msg, 1);
            else
                func_ov084_0212c9f0(self, msg, 0);
            return;
        }
        case 2: {
            int *lookAt = (int *)&cam->lookAt;
            int *pos = (int *)&cam->pos;
            self->mSavedCamLookAt.x = lookAt[0];
            self->mSavedCamLookAt.y = lookAt[1];
            self->mSavedCamLookAt.z = lookAt[2];
            self->mSavedCamPos.x = pos[0];
            self->mSavedCamPos.y = pos[1];
            self->mSavedCamPos.z = pos[2];
            _ZN6Camera9SetFlag_3Ev(cam);
            self->mFlags &= ~1;
            self->mCutsceneStep += 1;
            return;
        }
        case -1:
            OpenCannonInCurLevel();
            self->mCannonOpened = 1;
            func_ov084_0212c960(self, kStateIdle);
            return;
        }
        return;
    case 1: {
        /* Camera over to the shutter, then open it. */
        dActor_c *shutter;
        if (self->mShutterID == 0)
            return;
        if (func_ov084_0212cae0(self) == 0)
            return;
        shutter = dActor_c::FindWithID(self->mShutterID);
        if (shutter == 0)
            return;
        func_ov002_020bc990(shutter);
        self->mCutsceneStep += 1;
        return;
    }
    case 2: {
        /* Wait for the shutter to finish opening. */
        unsigned int id = self->mShutterID;
        if (id == 0)
            return;
        if (((daObjCannonShutter_c *)dActor_c::FindWithID(id))->mCannonOpen == 1)
            self->mCutsceneStep += 1;
        return;
    }
    case 3:
        /* Camera back. */
        if (func_ov084_0212ccb4(self) != 0)
            self->mCutsceneStep += 1;
        return;
    case 4: {
        /* Finish the talk and hand the camera back. */
        int talkState = player->GetTalkState();
        if (talkState == 1)
            return;
        if (talkState == 2) {
            int msg = data_0209f2f8 == 6 ? 0x90 : 0x14b;
            func_ov084_0212c9f0(self, msg, 0);
            return;
        }
        self->mFlags |= 1;
        OpenCannonInCurLevel();
        self->mCannonOpened = 1;
        cam->mFlags &= ~8;
        func_ov084_0212c960(self, kStateIdle);
        return;
    }
    }
}

// @symbol _ZN14daRedBombhei_cD1Ev
// @symbol _ZN14daRedBombhei_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
