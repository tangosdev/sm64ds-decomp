//cpp
/**
 * Wall sign Mario reads (KANBAN registry).
 *
 * With no player nearby, Behavior offers each colliding Mario a
 * conversation; with one talking, it runs a three-state script by
 * talk state -- approach check, walk-to check, then the message --
 * and releases the player when talk ends. InitResources loads the
 * sign model and lays the collider.
 *
 * What stays: Vec3_ApproachHorz / AngleDiff have no shared header.
 * What stays: daPly_c::func_ov002_020bec9c, the player's SetAnim wrapper,
 *   is still named by address. Naming belongs to the daPly_c TU.
 * What stays: the (s16)param1 cast on the message ID is load-bearing
 *   (it sign-extends the halfword).
 * What stays: Behavior copies the player's position through a scoped
 *   `const Vector3 &`. The ROM holds that base pointer, and a plain
 *   `tpos = tgt->mPos` is a word short.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjKanban_c_AssetId { ASSET_OBJ_KANBAN_BMD = 0x0474 };

#include "daObjKanban_c.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/Approach.h"
#include "math/CalcLib.h"

/* 8-byte file handle. The model uses func_02017acc / func_02017ab4. The
 * spelling is local; the manifest aliases the generated names to those ROM
 * symbols. */
struct KanbanModelFilePtr : SharedFilePtr {
    KanbanModelFilePtr(u32 fileID);
    ~KanbanModelFilePtr();
};

/* The sign model's shared file; Cleanup releases it, Init loads it. Defined
 * at the end of this file so the constructor does not enter .text. */
extern KanbanModelFilePtr data_ov085_02130858;

/* the collider's offset, in .rodata */
extern Vector3 data_ov085_0212f29c;

/* local extern: no shared header declares these. */

extern int Vec3_ApproachHorz(Vector3 *out, Vector3 *a, int maxStep);
extern "C" {

extern int AngleDiff(int a, int b);
}


/* the player side of the conversation */


#pragma defer_codegen off

/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dCcAcPos_c (0x320), then dBgActor_c's
 * inlined, then dBgActor_c's dBgW_KcMbg and dExtSimpleModel_c, then dActor_c. */
daObjKanban_c::~daObjKanban_c() {}

/* One release. A wall sign owns exactly one shared file -- the sign
 * model -- and gives it back on the way out. */
int daObjKanban_c::CleanupResources() {
    data_ov085_02130858.Release();
    return 1;
}

int daObjKanban_c::Render() {
    mModel.Render(0);
    return 1;
}

int daObjKanban_c::Behavior() {
    daPly_c *tgt = mTalkingPlayer;
    Vector3 mine, pos, tpos;

    if (tgt != 0) {
        mine = mPos;
        pos = mPos;
        pos.x += FX_MUL(FX_SinIdx((u16)mAngleY >> 4), 0x5a000);
        pos.z += FX_MUL(FX_CosIdx((u16)mAngleY >> 4), 0x5a000);
        {
            const Vector3 &plyPos = tgt->mPos;
            tpos = plyPos;
        }

        switch (tgt->GetTalkState()) {
        case 0:
            switch (mState) {
            case 0:
                if (Vec3_HorzDist(&tpos, &pos) < 0x32000) {
                    mState += 1;
                } else if (ApproachLinear(tgt->mAngleY, Vec3_HorzAngle(&tpos, &pos), 0x800)) {
                    mState += 1;
                    tgt->func_ov002_020bec9c(1, 0, 0x1000, 0);
                }
                break;
            case 1:
                if (Vec3_ApproachHorz(&tgt->mPos, &pos, 0xa000)) {
                    tgt->func_ov002_020bec9c(0, 0, 0x1000, 0);
                    mState += 1;
                }
                break;
            case 2:
                if (ApproachLinear(tgt->mAngleY, (s16)(mAngleY + 0x8000), 0x800)) {
                    mMessageID = 0;
                    if (param1 != 0xffff)
                        mMessageID = (s16)param1;
                    tgt->ShowMessage2(*this, (unsigned int)mMessageID, &mine, 0, 1);
                    mState = 0;
                }
                break;
            default:
                break;
            }
            break;
        case 1:
            break;
        default:
            mTalkingPlayer = 0;
            break;
        }
    } else {
        dActor_c *other;
        Vector3 opos;
        int isPlayer;

        if ((mdCcAcPos_c.hitFlags & 0x8000000) != 0) {
            other = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
            if (other != 0) {
                isPlayer = (other->actorID == 0xbf);
                if (isPlayer != 0) {
                    const Vector3 &plyPos = other->mPos;
                    opos = plyPos;
                    if (AngleDiff(Vec3_HorzAngle(&mPos, &opos), mAngleY) < 0x4000) {
                        if (static_cast<daPly_c *>(other)->StartTalk(*this, 0))
                            mTalkingPlayer = static_cast<daPly_c *>(other);
                    }
                }
            }
        }
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

int daObjKanban_c::InitResources() {
    struct Vector3 v;
    BMD_File *file = dExtSimpleModel_c::LoadFile(data_ov085_02130858);
    mModel.SetFile(file, 1, -1);
    UpdateModelPosAndRotY();
    v.x = data_ov085_0212f29c.x;
    v.y = data_ov085_0212f29c.y;
    v.z = data_ov085_0212f29c.z;
    mdCcAcPos_c.Init(this, v, 0x78000, 0x64000, 0x4800002, 0);
    return 1;
}

/* The registry factory behind the KANBAN profile. `return new
 * daObjKanban_c()` is the whole body: the synthesized constructor is
 * what runs dBgActor_c's base step, stores the vptr and then
 * constructs the dCcAcPos_c at 0x320, in that order, and `operator
 * new` is fBase_c's. */
extern "C" daObjKanban_c *daObjKanban_c_classInit(void) {
    return new daObjKanban_c();
}

KanbanModelFilePtr data_ov085_02130858(ASSET_OBJ_KANBAN_BMD);
