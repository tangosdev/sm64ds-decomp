//cpp
// @symbol _ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3hh
/* recovered: named members + shared header, real C++ method
 *
 * Starts a message. Already talking is success only at sub-step 0 or 3;
 * otherwise a short list of states may be interrupted, plus the no-control
 * state for all but a handful of mNoCtrlKind values. The last two parameters
 * are unsigned char -- the ROM reads all three of their uses with ldrb, and
 * the symbol was corrected from jj to hh two commits ago.
 */
#include "Player.h"
extern "C" {

extern Player::State data_ov002_0211046c;
extern Player::State data_ov002_0211013c;
extern Player::State data_ov002_02110364;
extern Player::State data_ov002_0211043c;
extern Player::State data_ov002_02110154;
extern Player::State data_ov002_02110424;
extern Player::State data_ov002_021104e4;
extern Player::State data_ov002_02110514;
extern Player::State data_ov002_0211022c;

extern int data_ov002_0210e15c;
extern int data_ov002_0210f224[3];
extern int data_ov002_0210f1ac;
extern void _ZN7Vector3D1Ev(void);
extern void *data_0209f318;

void _ZN6Player11ChangeStateERNS_5StateE(void *thiz, Player::State *st);
void func_020731dc(void *a, void *b, void *node);
void Vec3_RotateYAndTranslate(Vector3 *out, const Vector3 *in, int angle, const Vector3 *src);
void _ZN6Camera9SetFlag_3Ev(void *self);

int Player::ShowMessage2(ActorBase & actor_, unsigned int msg, const Vector3 * pos, unsigned char d, unsigned char e)
{
    ActorBase *actor = &actor_;
    int r7;

    if (IsState(data_ov002_0211046c)) {
        u8 sub = mStateStep;
        if (sub == 0 || sub == 3)
            goto finish;
        return 0;
    }

    r7 = 0;
    if (IsState(data_ov002_0211013c) ||
        IsState(data_ov002_02110364) ||
        IsState(data_ov002_0211043c) ||
        IsState(data_ov002_02110154) ||
        IsState(data_ov002_02110424) ||
        IsState(data_ov002_021104e4) ||
        IsState(data_ov002_02110514)) {
        r7 = 1;
    }

    if (IsState(data_ov002_0211022c)) {
        u8 v = mNoCtrlKind;
        if (!(v == 0 || v == 1 || v == 3 || v == 2 || v == 0xd)) {
            if (v != 8)
                r7 = 1;
        }
    }

    if (!r7)
        return 0;

    _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211046c);
    *(ActorBase **)(&mTalkActor) = actor;

finish:
    mStateWork = 0;
    mStateArg = d & 3;
    *(u8 *)(((long long)(&mStateArg))) |= (e & 3) << 2;

    if (!(data_ov002_0210e15c & 1)) {
        data_ov002_0210f224[0] = 0;
        data_ov002_0210f224[1] = 0x64000;
        data_ov002_0210f224[2] = 0x64000;
        func_020731dc(data_ov002_0210f224, (void *)_ZN7Vector3D1Ev, &data_ov002_0210f1ac);
        data_ov002_0210e15c |= 1;
    }

    Vec3_RotateYAndTranslate((Vector3 *)(&unk_744), (Vector3 *)(&mPosX), mAngleY, (Vector3 *)data_ov002_0210f224);

    if (pos) {
        *(Vector3 *)(&unk_744) = *pos;
    }

    mHorzSpeed = 0;
    mStateStep = 1;
    *(unsigned int*)&mAttachOffsetY = msg;

    if (e == 2) {
        _ZN6Camera9SetFlag_3Ev(data_0209f318);
    }

    return 1;
}
}
