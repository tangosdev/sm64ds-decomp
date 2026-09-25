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
 * deslop
 * Leftover: _Z14ApproachLinearRsss / Vec3_HorzDist / Vec3_HorzAngle /
 *   Vec3_ApproachHorz / AngleDiff keep linker names (no header home;
 *   kaitendai precedent keeps ApproachLinear mangled too).
 * Leftover: func_ov002_020bec9c is still the linker name of the
 *   Player SetAnim wrapper owned by the Player TU. Naming belongs there.
 * Leftover: data_02082214 is the shared arm9 sine table.
 * Leftover: target + 0x8e is a Player short with no named header field.
 * Leftover: the (s16)mMessageID / (u16 *)&mAngleY casts are
 *   load-bearing (sign-extends / zero-extends the halfword load).
 */

#include "daObjKanban_c.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* The sign model's shared file; Cleanup releases it, Init loads it. */
extern SharedFilePtr data_ov085_02130858;

extern "C" {

/* the collider's offset, in .rodata */
extern struct Vector3 data_ov085_0212f29c;

/* math / vector helpers */
extern int Vec3_HorzDist(Vector3 *a, Vector3 *b);
extern s16 Vec3_HorzAngle(Vector3 *a, Vector3 *b);
extern int Vec3_ApproachHorz(Vector3 *out, Vector3 *a, int maxStep);
extern int _Z14ApproachLinearRsss(s16 *p, s16 target, s16 step);
extern int AngleDiff(int a, int b);
extern s16 data_02082214[];

/* actor / collision plumbing */
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(char *self, char *actor, const struct Vector3 *pos, int a, unsigned int b, unsigned int c, unsigned int d);

/* model loading */

/* the player side of the conversation */
extern int func_ov002_020bec9c(void *c, unsigned int a, int b, int d, unsigned short e);

}

#pragma defer_codegen off

// @symbol _ZN13daObjKanban_cD1Ev
// @symbol _ZN13daObjKanban_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dCcAcPos_c (0x320), then dBgActor_c's
 * inlined, then dBgActor_c's dBgW_KcMbg and Model, then dActor_c. */
daObjKanban_c::~daObjKanban_c()
{
}

// @symbol _ZN13daObjKanban_c16CleanupResourcesEv
/* One release. A wall sign owns exactly one shared file -- the sign
 * model -- and gives it back on the way out. */
int daObjKanban_c::CleanupResources()
{
    data_ov085_02130858.Release();
    return 1;
}

// @symbol _ZN13daObjKanban_c6RenderEv
int daObjKanban_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjKanban_c8BehaviorEv
int daObjKanban_c::Behavior()
{
    Player *tgt = mTalkingPlayer;
    Vector3 mine, pos, tpos;

    if (tgt != 0) {
        mine.x = mPosX;
        mine.y = mPosY;
        mine.z = mPosZ;

        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;

        pos.x += (int)(((s64)data_02082214[(*(u16 *)&mAngleY >> 4) << 1] * 0x5a000 + 0x800) >> 12);
        pos.z += (int)(((s64)data_02082214[((*(u16 *)&mAngleY >> 4) << 1) + 1] * 0x5a000 + 0x800) >> 12);

        {
            int *p = (int *)&tgt->mPosX;
            tpos.x = p[0];
            tpos.y = p[1];
            tpos.z = p[2];
        }

        switch (tgt->GetTalkState()) {
        case 0:
            switch (mState) {
            case 0:
                if (Vec3_HorzDist(&tpos, &pos) < 0x32000) {
                    mState += 1;
                } else if (_Z14ApproachLinearRsss((s16 *)((char *)tgt + 0x8e), Vec3_HorzAngle(&tpos, &pos), 0x800)) {
                    mState += 1;
                    func_ov002_020bec9c(tgt, 1, 0, 0x1000, 0);
                }
                break;
            case 1:
                if (Vec3_ApproachHorz((Vector3 *)&tgt->mPosX, &pos, 0xa000)) {
                    func_ov002_020bec9c(tgt, 0, 0, 0x1000, 0);
                    mState += 1;
                }
                break;
            case 2:
                if (_Z14ApproachLinearRsss((s16 *)((char *)tgt + 0x8e), (s16)(mAngleY + 0x8000), 0x800)) {
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
        void *other;
        Vector3 opos;
        int isPlayer;
        int *pO;

        if ((mdCcAcPos_c.hitFlags & 0x8000000) != 0) {
            other = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
            if (other != 0) {
                isPlayer = (((dActor_c *)other)->actorID == 0xbf);
                if (isPlayer != 0) {
                    pO = (int *)&((dActor_c *)other)->mPosX;
                    opos.x = pO[0];
                    opos.y = pO[1];
                    opos.z = pO[2];
                    if (AngleDiff(Vec3_HorzAngle((Vector3 *)&mPosX, &opos), mAngleY) < 0x4000) {
                        if (((Player *)other)->StartTalk(*this, 0))
                            mTalkingPlayer = (Player *)other;
                    }
                }
            }
        }
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN13daObjKanban_c13InitResourcesEv
int daObjKanban_c::InitResources()
{
  struct Vector3 v;
  void* file = Model::LoadFile(data_ov085_02130858);
  mModel.SetFile((BMD_File *)file, 1, -1);
  UpdateModelPosAndRotY();
  v.x = data_ov085_0212f29c.x;
  v.y = data_ov085_0212f29c.y;
  v.z = data_ov085_0212f29c.z;
  _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char*)this) + 0x320, ((char*)this), &v, 0x78000, 0x64000, 0x4800002, 0);
  return 1;
}

// @symbol daObjKanban_c_classInit
/* The registry factory behind the KANBAN profile. `return new
 * daObjKanban_c()` is the whole body: the synthesized constructor is
 * what runs dBgActor_c's base step, stores the vptr and then
 * constructs the dCcAcPos_c at 0x320, in that order, and `operator
 * new` is fBase_c's. */
extern "C" daObjKanban_c *daObjKanban_c_classInit(void)
{
    return new daObjKanban_c();
}
