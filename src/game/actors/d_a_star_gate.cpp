//cpp
/* Star door (STAR_GATE 354) -- ov100/daStarGate_c.
 *
 * ov100 is mixed (PATH_LIFT / ROLLING_IRON_BALL / BUTTERFLY / UNCHAINED_CHOMP /
 * FISH / DOOR / STAR_DOOR). RTTI names this class daStarGate_c; the debug table
 * names STAR_GATE. overlay_actors.md's project name is STAR_DOOR.
 * Historical project alias: StarDoor.
 *
 * SM64DS proves the class through RTTI, the STAR_GATE profile, the factory's
 * allocation/vptr shape, and the complete virtual table. Later source lineage
 * supplies the state and method spellings; their class ownership, PTMF layout,
 * argument count, bodies, call graph, and relocations are ROM-proven.
 *
 * mwccarm emits function sections in reverse source order. Keep the ROM-high
 * factory first and the ROM-low destructor last; the D1/D0/D2 group itself is
 * compiler-ordered.
 *
 * deslop
 * Leftover: Camera::GoBehindPlayer / SetFlag_3 stay mangled (no Camera.h;
 *   SetFlag_3 is not on Camera.h). This TU's St_OpenClose_Init / Main.
 * Leftover: func_ov100_02144fcc / 02145014 / 02145070 / 02144f84 stay those
 *   ROM labels. 02144fcc keeps the ellipsis: St_StayClosed_Main passes
 *   (this, player); the body is 0-arg.
 * Leftover: func_02012694 sound at mCamSpacePosX (TalkingToPlayer_Init,
 *   OpenClose_Init / Main).
 * Leftover: StarGateVector3 POD for data_ov100_02148948, messagePos, tmp,
 *   vec -- Vector3's inline dtor is non-POD; a Vector3 BSS object would
 *   register in sinit.
 * Leftover: data_ov100_02148934 model handle (SharedFilePtr.h has no fields;
 *   StarGateModelFilePtr is the 8-byte wrapper). data_ov100_02148390 gate
 *   table.
 * Leftover: *(Vector3 *)&mPosX / mScaleX / unk_0a4 (dActor_c consecutive
 *   s32s; a Vector3 member is not this class).
 * Leftover: data_0209caa0 +0x41 character byte (SaveData.mCharacter).
 *   SaveData.h shifts this TU's @432..@446 PTMF uniquifiers in symbols.txt.
 */
#include "daStarGate_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Model.h"

struct Camera;

extern "C" {
int func_ov100_02144fcc(...);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void func_02012694(int soundID, void *position, ...);
void _ZN6Camera14GoBehindPlayerEj(Camera *self, unsigned int a);
void Vec3_RotateYAndTranslate(Vector3 *out, void *m, short angle, Vector3 *in);
void ChangeArea(int areaID);
int ShowArea(int areaID);
int _ZN6Camera9SetFlag_3Ev(void *camera);
int func_ov100_02145014(void);
void func_ov100_02145070(int value);
unsigned char NumStars(void);
int func_ov100_02144f84(void);
void SetTouchScreenDelay(void);
int AngleDiff(int a, int b);
void Vec3_Sub(Vector3* out, Vector3* a, Vector3* b);
u8 IsAreaShowing(int areaID);
void Matrix4x3_FromRotationY(Matrix4x3 *matrix, int angle);
}

int ApproachLinear(int &x, int a, int b);

extern u8 data_0209f250;
extern Camera *data_0209f318;
extern Player *data_0209f394[];
extern int data_0209caa0[];
extern char data_020a0ebc[];

struct StarGateModelFilePtr : SharedFilePtr {
    u32 words[2];

    StarGateModelFilePtr(u32 fileID);
    ~StarGateModelFilePtr();
};

extern "C" StarGateModelFilePtr data_ov100_02148934;
StarGateModelFilePtr data_ov100_02148934(0x4a1);

struct StarGateVector3 {
    s32 x;
    s32 y;
    s32 z;
};

extern "C" StarGateVector3 data_ov100_02148948;
StarGateVector3 data_ov100_02148948;

/* The declaration order in daStarGate_c fixes the BSS layout.  Definition
 * order fixes the compiler-generated initialization sequence recovered at
 * 0x02147a70. */
daStarGate_c::State daStarGate_c::ST_WAIT = {
    0,
    &daStarGate_c::St_Wait_Main
};
daStarGate_c::State daStarGate_c::ST_TALKING_TO_PLAYER = {
    &daStarGate_c::St_TalkingToPlayer_Init,
    &daStarGate_c::St_TalkingToPlayer_Main
};
daStarGate_c::State daStarGate_c::ST_UNLOCKING = {
    &daStarGate_c::St_Unlocking_Init,
    &daStarGate_c::St_Unlocking_Main
};
daStarGate_c::State daStarGate_c::ST_OPEN_CLOSE = {
    &daStarGate_c::St_OpenClose_Init,
    &daStarGate_c::St_OpenClose_Main
};
daStarGate_c::State daStarGate_c::ST_STAY_CLOSED = {
    0,
    &daStarGate_c::St_StayClosed_Main
};

extern "C" daStarGateInfo data_ov100_02148390[] = {
    { 12, 5, 28, 37 },
    { 30, 6, 29, 38 },
    { 50, 7, 30, 39 },
    { 80, 8, 31, 36 }
};

struct StarGateSpawnInfo {
    daStarGate_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char StarGateSpawnInfo_size_must_be_0x1c[
    sizeof(StarGateSpawnInfo) == 0x1c ? 1 : -1];

extern "C" daStarGate_c *daStarGate_c_classInit();

extern "C" StarGateSpawnInfo g_profile_STAR_GATE = {
    daStarGate_c_classInit,
    0x0162,
    0x0162,
    0x00000002,
    0x00096000,
    0x00096000,
    0x01f40000,
    0
};

// @symbol daStarGate_c_classInit
extern "C" daStarGate_c *daStarGate_c_classInit()
{
    return new daStarGate_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c13InitResourcesEv
int daStarGate_c::InitResources()
{
    param1 = (u32)param1 >> 16;

    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov100_02148934);
    if (!mModel.SetFile(file, 1, 1))
        return 0;

    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
    ChangeState(&ST_WAIT,
                (Player *)data_0209f394[data_0209f250]);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c8BehaviorEv
int daStarGate_c::Behavior()
{
    Player *player = CalculateRelativePlayerPos();
    if (mState->main != 0)
        (this->*(mState->main))(player);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c6RenderEv
int daStarGate_c::Render()
{
    if (IsAreaShowing((s8)mAngleX) != 0 ||
        IsAreaShowing((s8)mAngleZ) != 0) {
        Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
        mModel.mat4x3.t.x = (mPosX + unk_0a4) >> 3;
        mModel.mat4x3.t.y = (mPosY + mVertSpeed) >> 3;
        mModel.mat4x3.t.z = (mPosZ + unk_0ac) >> 3;
        mModel.Render(0);

        Matrix4x3_FromRotationY(&mModel.mat4x3, (s16)(mAngleY + 0x8000));
        mModel.mat4x3.t.x = (mPosX - unk_0a4) >> 3;
        mModel.mat4x3.t.y = (mPosY - mVertSpeed) >> 3;
        mModel.mat4x3.t.z = (mPosZ - unk_0ac) >> 3;
        mModel.Render(0);
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c16OnPendingDestroyEv
void daStarGate_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c16CleanupResourcesEv
int daStarGate_c::CleanupResources()
{
    data_ov100_02148934.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c11ChangeStateEPNS_5StateEP6Player
bool daStarGate_c::ChangeState(State *nextState, Player *player)
{
    mState = nextState;
    if (mState->init == 0)
        return true;
    return (this->*(mState->init))(player);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c26CalculateRelativePlayerPosEv
Player *daStarGate_c::CalculateRelativePlayerPos()
{
    StarGateVector3 tmp;
    Player* player = data_0209f394[data_0209f250];
    Vec3_Sub((Vector3 *)&tmp, (Vector3*)&player->mPosX, (Vector3*)&mPosX);
    Vec3_RotateYAndTranslate((Vector3*)&mScaleX, data_020a0ebc,
                             (short)-mAngleY, (Vector3 *)&tmp);
    return player;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c17IsInFrontOfPlayerEP6Player
bool daStarGate_c::IsInFrontOfPlayer(Player *player)
{
  int v, z, a;
  v = mScaleX; if (v < 0) v = -v; if (v > 0x82000) goto fail;
  v = mScaleY; if (v < 0) v = -v; if (v > 0x32000) goto fail;
  z = mScaleZ;
  v = (z < 0) ? -z : z; if (v > 0x78000) goto fail;
  if (z < 0) a = mAngleY;
  else a = mAngleY + 0x8000;
  a = (short)a;
  if (AngleDiff(a, player->mAngleY) < 0x2000) return true;
fail:
  return false;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c11TryOpenDoorEP6Player
void daStarGate_c::TryOpenDoor(Player *player)
{
    int a;
    int z = mScaleZ;
    if (z < 0) a = mAngleY;
    else a = mAngleY + 0x8000;
    if (player->TryEnterStarDoor(*(Vector3 *)&mPosX, a))
        ChangeState(&ST_OPEN_CLOSE, player);
    SetTouchScreenDelay();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c12St_Wait_MainEP6Player
bool daStarGate_c::St_Wait_Main(Player *player)
{
    daStarGateInfo *entry;
    int isSpecial, notEnoughStars, isMario, messageID;
    StarGateVector3 messagePos;

    if (IsInFrontOfPlayer(player)) {
        entry = &data_ov100_02148390[param1];
        isSpecial = entry->numStars == 0x50;
        notEnoughStars = (int)NumStars() < entry->numStars;
        isMario = ((unsigned char *)data_0209caa0)[0x41] == 0;

        if (data_0209caa0[1] & (0x8000 << entry->saveFlag))
            goto tryOpen;
        if (isSpecial) {
            if (mScaleZ <= 0)
                goto tryOpen;
            if (!notEnoughStars && isMario)
                goto tryOpen;
        }

        {
            int x = mPosX;
            int z = mPosZ;
            int y = mPosY + 0xb4000;
            messagePos.x = x;
            messagePos.y = y;
            messagePos.z = z;
        }

        if (!(isMario && !isSpecial && !notEnoughStars)) {
            if (func_ov100_02144f84() == 0)
                return true;
            messageID = isMario ? entry->notEnoughStarsMsgID
                                : entry->notMarioMsgID;
            if (player->ShowMessage(*this, messageID,
                                    (Vector3 *)&messagePos, 0, 2) == 0)
                goto tryOpen;
            if (isSpecial)
                player->OpenBigDoor();
            ChangeState(&ST_TALKING_TO_PLAYER, player);
            return true;
        } else {
            if (player->TryTalkToDoor(1) == 0)
                goto tryOpen;
            ChangeState(&ST_UNLOCKING, player);
            return true;
        }

    tryOpen:
        TryOpenDoor(player);
    }
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c23St_TalkingToPlayer_InitEP6Player
bool daStarGate_c::St_TalkingToPlayer_Init(Player *)
{
    func_02012694(0xb8, &mCamSpacePosX);
    func_ov100_02145070(0x87);
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c23St_TalkingToPlayer_MainEP6Player
bool daStarGate_c::St_TalkingToPlayer_Main(Player *player)
{
  func_ov100_02145014();
  if (player->GetTalkState() == -1) {
    if (data_ov100_02148390[param1].numStars == 0x50) {
      TryOpenDoor(player);
    } else if (!IsInFrontOfPlayer(player)) {
      ChangeState(&ST_STAY_CLOSED, player);
    }
  }
  return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c17St_Unlocking_InitEP6Player
bool daStarGate_c::St_Unlocking_Init(Player *player)
{
    player->OpenBigDoor();
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c17St_Unlocking_MainEP6Player
bool daStarGate_c::St_Unlocking_Main(Player *player)
{
    if (!player->IsOpeningDoorWithStar()) {
        daStarGateInfo *entry = &data_ov100_02148390[param1];
        s8 sh = entry->saveFlag;
        data_0209caa0[1] |= 0x8000 << sh;
        TryOpenDoor(player);
    }
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c17St_OpenClose_InitEP6Player
bool daStarGate_c::St_OpenClose_Init(Player *)
{
    ShowArea((s8)mAngleX);
    ShowArea((s8)mAngleZ);
    _ZN6Camera9SetFlag_3Ev((void *)data_0209f318);
    mTerminalVelocity = -0x8a000;
    func_02012694(0x4e, &mCamSpacePosX);
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c17St_OpenClose_MainEP6Player
bool daStarGate_c::St_OpenClose_Main(Player *player)
{
    int ready;
    StarGateVector3 vec;
    int gy, gz, vd;

    ready = func_ov100_02144fcc();
    if (ApproachLinear(mHorzSpeed, mTerminalVelocity, 0x8000) != 0) {
        if (mTerminalVelocity != 0) {
            if (mCloseDoorTimer == 0) {
                mCloseDoorTimer = 0x1c;
            } else if (DecIfAbove0_Byte(&mCloseDoorTimer) == 0) {
                mTerminalVelocity = 0;
                func_02012694(0x4f, &mCamSpacePosX, 0);
            } else if (mCloseDoorTimer == 8) {
                _ZN6Camera14GoBehindPlayerEj(data_0209f318, data_0209f250);
            }
        } else if (ready != 0) {
            ChangeState(&ST_WAIT, player);
            {
                int t;
                if (mScaleZ < 0)
                    t = mAngleX;
                else
                    t = mAngleZ;
                t = (s8)t;
                player->mAreaId = t;
                ChangeArea(t);
            }
        }
    }

    vd = mHorzSpeed;
    gy = data_ov100_02148948.y;
    gz = data_ov100_02148948.z;
    data_ov100_02148948.x = vd;
    vec.x = vd;
    vec.z = gz;
    vec.y = gy;
    Vec3_RotateYAndTranslate((Vector3 *)&unk_0a4, &data_020a0ebc,
                             mAngleY, (Vector3 *)&vec);
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daStarGate_c18St_StayClosed_MainEP6Player
bool daStarGate_c::St_StayClosed_Main(Player *player)
{
    if (func_ov100_02144fcc(this, player))
        ChangeState(&ST_WAIT, player);
    return true;
}
