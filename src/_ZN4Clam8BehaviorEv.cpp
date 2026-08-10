//cpp
// @symbol _ZN4Clam8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * The clam's two-state open/shut cycle. State 0 is shut: on animation frame
 * 0x19 it plays sound 0x27, puffs particle 0x10c above itself, arms a
 * 10-frame counter and CLEARS the collision bit; otherwise, once the state
 * timer passes 150 frames and the player is within 0x1f4000 (500.0), it
 * plays sound 0x26, puffs particle 0x10b and switches to state 1 on the
 * other animation. State 1 runs 150 frames and goes back, setting the
 * collision bit on animation frame 0xf but NOT on frame 8 -- the bit is
 * live only for the part of the lunge that can connect.
 *
 * Whatever the state, a touched actor whose ID is 0xbf (the player) is hurt
 * for 2 with 0xc000 of knockback, every frame the contact persists.
 *
 * mModelAnim carries the Animation base at +0x50, so the ROM's calls on
 * `this + 0x124` are member calls on `this + 0xd4`: the compiler applies
 * the same adjustment the hand-spelt offsets did.
 *
 * THE `(int)` INSIDE THE `(long long)` ROUND-TRIP BELOW IS LOAD-BEARING, and
 * it is C++ that makes it so. In C, `*(u16 *)(p + 0xc) == 0xbf` is already an
 * `int`, so `(int)(long long)(...)` widens an int; compiled as C++ the
 * comparison is a `bool` and the round-trip widens THAT instead, which moves
 * the sixth argument of the Hurt call into a different register -- 17 words
 * out of 155. It was the only one of six candidate spellings that matched,
 * so a .c file carrying this idiom cannot be renamed .cpp without
 * re-measuring it.
 *
 * The function's four OTHER round-trips were not load-bearing -- the two on
 * the collision word, the counter decrement and the timer increment are all
 * free, and are gone. One compile settles each; the idiom is per-site.
 */
#include "Clam.h"
#include "CylinderClsn.h"

extern "C" {
extern char data_ov064_0211c9cc[];
extern char data_ov064_0211c9bc[];

extern void func_0201267c(int soundId, Fix12i *camSpacePos);
extern int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern int _ZN5Actor13DistToCPlayerEv(char *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *ma, int file, int flags, int speed, unsigned int startFrame);
extern char *_ZN5Actor10FindWithIDEj(int id);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char *player, int *src, unsigned int damage, int knockback, unsigned int a, unsigned int b, unsigned int c);
}

int Clam::Behavior()
{
    struct {
        int shutPuff[3];
        int lungePuff[3];
        int hurtFrom[3];
    } u;

    switch (mState) {
    case 0:
        if (mModelAnim.WillHitFrame(0x19)) {
            func_0201267c(0x27, &mCamSpacePosX);
            u.shutPuff[0] = mPosX;
            u.shutPuff[1] = mPosY;
            u.shutPuff[2] = mPosZ;
            u.shutPuff[1] = u.shutPuff[1] + 0x32000;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10c,
                u.shutPuff[0], u.shutPuff[1], u.shutPuff[2]);
            unk_170 = 0xa;
            mStateTimer = 0;
            *(int *)&unk_150 &= ~1;
        } else {
            if (mStateTimer > 0x96 &&
                _ZN5Actor13DistToCPlayerEv((char *)this) < 0x1f4000) {
                func_0201267c(0x26, &mCamSpacePosX);
                u.lungePuff[0] = mPosX;
                u.lungePuff[1] = mPosY;
                u.lungePuff[2] = mPosZ;
                u.lungePuff[1] = u.lungePuff[1] + 0x32000;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10b,
                    u.lungePuff[0], u.lungePuff[1], u.lungePuff[2]);
                mStateTimer = 0;
                mState = 1;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&mModelAnim,
                    *(int *)(data_ov064_0211c9cc + 4), 0x40000000, 0x1000, 0);
            } else {
                if (unk_170 != 0)
                    unk_170--;
            }
        }
        break;
    case 1:
        if (mStateTimer > 0x96) {
            mState = 0;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&mModelAnim,
                *(int *)(data_ov064_0211c9bc + 4), 0x40000000, 0x1000, 0);
            mStateTimer = 0;
        } else {
            if (!mModelAnim.WillHitFrame(8)) {
                if (mModelAnim.WillHitFrame(0xf))
                    *(int *)&unk_150 |= 1;
            }
        }
        break;
    }

    mStateTimer++;

    if (unk_15c != 0) {
        char *touched = _ZN5Actor10FindWithIDEj(unk_15c);
        if (touched != 0) {
            int isPlayer = (int)(*(u16 *)(touched + 0xc) == 0xbf);
            if (isPlayer != 0) {
                u.hurtFrom[0] = mPosX;
                u.hurtFrom[1] = mPosY;
                u.hurtFrom[2] = mPosZ;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(touched, u.hurtFrom,
                    2, 0xc000, 1, 0, 1);
            }
        }
    }

    mModelAnim.Advance();
    ((CylinderClsn *)&mMovingCylinderClsn)->Clear();
    ((CylinderClsn *)&mMovingCylinderClsn)->Update();
    return 1;
}
