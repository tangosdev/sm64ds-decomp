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
 * Leftover: func_ov100_02144fcc / 02145014 / 02145070 / 02144f84 stay those
 *   ROM labels. 02144fcc keeps the ellipsis: St_StayClosed_Main passes
 *   (this, player); the body is 0-arg.
 * Leftover: func_02012694 sound at mCamSpacePos (TalkingToPlayer_Init,
 *   OpenClose_Init / Main).
 * Leftover: POD VecFx32 for data_ov100_02148948 -- Vector3's inline
 *   dtor is non-POD; a Vector3 BSS object would register in sinit.
 * Leftover: data_ov100_02148390 gate table.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_star_gate_AssetId {
    ASSET_OBJ_STARGATE_BMD = 0x04a1
};

#include "daStarGate_c.h"
#include "SpawnInfo.h"
#include "daPly_c.h"
#include "daDoor_c.h"
#include "SharedFilePtr.h"
#include "dExtSimpleModel_c.h"
#include "dCamera_c.h"
#include "math/Approach.h"
#include "nitro/fx/fx.h"
#include "SaveBlock.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

void func_02012694(unsigned int soundID, const Vector3 *position);
extern "C" void ChangeArea(int areaID);
void ShowArea(int areaID);
unsigned char NumStars(void);
void SetTouchScreenDelay(void);
u8 IsAreaShowing(int areaID);
extern "C" {
int AngleDiff(int a, int b);
}
extern u8 gLocalPlayerId;
extern dCamera_c *gCamera;
extern daPly_c *gPlayers[];
extern Vector3 data_020a0ebc;

struct StarGateModelFilePtr : SharedFilePtr {
    StarGateModelFilePtr(u32 fileID);
    ~StarGateModelFilePtr();
};

extern StarGateModelFilePtr gObjStargateModelFile;
StarGateModelFilePtr gObjStargateModelFile(ASSET_OBJ_STARGATE_BMD);

extern VecFx32 data_ov100_02148948;
VecFx32 data_ov100_02148948;

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

daStarGateInfo data_ov100_02148390[] = {
    {12, 5, 28, 37}, {30, 6, 29, 38}, {50, 7, 30, 39}, {80, 8, 31, 36}};

daStarGate_c *daStarGate_c_classInit();

SpawnInfo<daStarGate_c> g_profile_STAR_GATE = {
    daStarGate_c_classInit, 0x0162, 0x0162, 0x00000002, 0x00096000, 0x00096000, 0x01f40000, 0};

/* |v| for the scale-space reach test in IsInFrontOfPlayer. */
static inline int Abs(int v) {
    return v < 0 ? -v : v;
}

daStarGate_c *daStarGate_c_classInit() {
    return new daStarGate_c();
}

/* InitResources keeps only param1's high halfword, the gate's index into
   data_ov100_02148390, with a read through this accessor. A plain
   `param1 = param1 >> 16` lets 2004/b56 value-number both sides and
   materialise the +8 address once (0xac, one word longer than the
   cartridge's folded ldr/lsr/str). Same lever as daDoor_c::GetParam1. */
inline u32 daStarGate_c::GetParam1() const {
    return param1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daStarGate_c::InitResources()
{
    param1 = GetParam1() >> 16;

    BMD_File *file = dExtSimpleModel_c::LoadFile(gObjStargateModelFile);
    if (!mModel.SetFile(file, 1, 1))
        return 0;

    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngle.y);
    mModel.mat4x3.t.x = mPos.x >> 3;
    mModel.mat4x3.t.y = mPos.y >> 3;
    mModel.mat4x3.t.z = mPos.z >> 3;
    ChangeState(&ST_WAIT, gPlayers[gLocalPlayerId]);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daStarGate_c::Behavior()
{
    daPly_c *player = CalculateRelativePlayerPos();
    if (mState->main != 0)
        (this->*(mState->main))(player);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daStarGate_c::Render()
{
    if (IsAreaShowing((s8)mAngle.x) != 0 || IsAreaShowing((s8)mAngle.z) != 0) {
        Matrix4x3_FromRotationY(&mModel.mat4x3, mAngle.y);
        mModel.mat4x3.t.x = (mPos.x + mSpeed.x) >> 3;
        mModel.mat4x3.t.y = (mPos.y + mSpeed.y) >> 3;
        mModel.mat4x3.t.z = (mPos.z + mSpeed.z) >> 3;
        mModel.Render(0);

        Matrix4x3_FromRotationY(&mModel.mat4x3, (s16)(mAngle.y + 0x8000));
        mModel.mat4x3.t.x = (mPos.x - mSpeed.x) >> 3;
        mModel.mat4x3.t.y = (mPos.y - mSpeed.y) >> 3;
        mModel.mat4x3.t.z = (mPos.z - mSpeed.z) >> 3;
        mModel.Render(0);
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
void daStarGate_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daStarGate_c::CleanupResources()
{
    gObjStargateModelFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
bool daStarGate_c::ChangeState(State *nextState, daPly_c *player) {
    mState = nextState;
    if (mState->init == 0)
        return true;
    return (this->*(mState->init))(player);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
daPly_c *daStarGate_c::CalculateRelativePlayerPos() {
    Vector3 toPlayer;
    daPly_c *player = gPlayers[gLocalPlayerId];
    Vec3_Sub(&toPlayer, &player->mPos, &mPos);
    Vec3_RotateYAndTranslate(&mScale, &data_020a0ebc, -mAngle.y, &toPlayer);
    return player;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
bool daStarGate_c::IsInFrontOfPlayer(daPly_c *player) {
    int a;
    if (Abs(mScale.x) <= 0x82000 && Abs(mScale.y) <= 0x32000 && Abs(mScale.z) <= 0x78000) {
        if (mScale.z < 0)
            a = mAngle.y;
        else
            a = mAngle.y + 0x8000;
        if (AngleDiff((short)a, player->mAngle.y) < 0x2000)
            return true;
    }
    return false;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
void daStarGate_c::TryOpenDoor(daPly_c *player) {
    int a;
    int z = mScale.z;
    if (z < 0)
        a = mAngle.y;
    else
        a = mAngle.y + 0x8000;
    if (player->TryEnterStarDoor(mPos, a))
        ChangeState(&ST_OPEN_CLOSE, player);
    SetTouchScreenDelay();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
bool daStarGate_c::St_Wait_Main(daPly_c *player) {
    daStarGateInfo *entry;
    int isSpecial, notEnoughStars, isMario, messageID;

    if (IsInFrontOfPlayer(player)) {
        entry = &data_ov100_02148390[param1];
        isSpecial = entry->numStars == 0x50;
        notEnoughStars = NumStars() < entry->numStars;
        isMario = gSaveBlock.file.mCharacter == 0;

        /* A door already unlocked in the save just opens. So does the 80-star
           door from behind (mScale.z <= 0), or for Mario with enough stars.
           Otherwise it shows its message (or, for Mario with enough stars,
           starts the unlock talk) and falls back to TryOpenDoor when that
           call returns 0. */
        if (!(gSaveBlock.file.flags1 & (0x8000 << entry->saveFlag))) {
            if (!isSpecial || (mScale.z > 0 && (notEnoughStars || !isMario))) {
                Vector3 messagePos(mPos.x, mPos.y + 0xb4000, mPos.z);

                if (!(isMario && !isSpecial && !notEnoughStars)) {
                    if (daDoor_c::func_ov100_02144f84() == 0)
                        return true;
                    messageID = isMario ? entry->notEnoughStarsMsgID : entry->notMarioMsgID;
                    if (player->ShowMessage(*this, messageID, &messagePos, 0, 2) != 0) {
                        if (isSpecial)
                            player->OpenBigDoor();
                        ChangeState(&ST_TALKING_TO_PLAYER, player);
                        return true;
                    }
                } else if (player->TryTalkToDoor(1) != 0) {
                    ChangeState(&ST_UNLOCKING, player);
                    return true;
                }
            }
        }

        TryOpenDoor(player);
    }
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
bool daStarGate_c::St_TalkingToPlayer_Init(daPly_c *) {
    func_02012694(0xb8, &mCamSpacePos);
    daDoor_c::func_ov100_02145070(0x87);
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
bool daStarGate_c::St_TalkingToPlayer_Main(daPly_c *player) {
    daDoor_c::func_ov100_02145014();
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
bool daStarGate_c::St_Unlocking_Init(daPly_c *player) {
    player->OpenBigDoor();
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
bool daStarGate_c::St_Unlocking_Main(daPly_c *player) {
    if (!player->IsOpeningDoorWithStar()) {
        daStarGateInfo *entry = &data_ov100_02148390[param1];
        s8 sh = entry->saveFlag;
        gSaveBlock.file.flags1 |= 0x8000 << sh;
        TryOpenDoor(player);
    }
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
bool daStarGate_c::St_OpenClose_Init(daPly_c *) {
    ShowArea((s8)mAngle.x);
    ShowArea((s8)mAngle.z);
    gCamera->SetFlag_3();
    mTerminalVelocity = -0x8a000;
    func_02012694(0x4e, &mCamSpacePos);
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
bool daStarGate_c::St_OpenClose_Main(daPly_c *player) {
    int ready;
    Vector3 vec;
    int gy, gz, vd;

    ready = daDoor_c::func_ov100_02144fcc();
    if (ApproachLinear(mHorzSpeed, mTerminalVelocity, 0x8000) != 0) {
        if (mTerminalVelocity != 0) {
            if (mCloseDoorTimer == 0) {
                mCloseDoorTimer = 28;
            } else if (DecIfAbove0_Byte(&mCloseDoorTimer) == 0) {
                mTerminalVelocity = 0;
                func_02012694(0x4f, &mCamSpacePos);
            } else if (mCloseDoorTimer == 8) {
                gCamera->GoBehindPlayer(gLocalPlayerId);
            }
        } else if (ready != 0) {
            ChangeState(&ST_WAIT, player);
            {
                int t;
                if (mScale.z < 0)
                    t = mAngle.x;
                else
                    t = mAngle.z;
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
    Vec3_RotateYAndTranslate(&mSpeed, &data_020a0ebc, mAngle.y, &vec);
    return true;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
bool daStarGate_c::St_StayClosed_Main(daPly_c *player) {
    if (daDoor_c::func_ov100_02144fcc())
        ChangeState(&ST_WAIT, player);
    return true;
}
