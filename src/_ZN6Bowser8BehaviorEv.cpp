//cpp
// @symbol _ZN6Bowser8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header
 *
 * One frame of the fight: pick the closest player and record the angle and distance
 * to him, run the two state workers, advance the animation, publish `this` into the
 * global at data_0209f318+0x114, then rebuild the body cylinder 0x50000 in front of
 * the actor.
 *
 * FOUR STAND-IN STRUCTS ARE GONE -- `Actor`, `Animation`, `CylinderClsn` and
 * `MovingCylinderClsnWithPos`, each declared here with just the one or two methods
 * this file called, then "defined" again below with a set of bodyless declarations
 * that existed only to stop the compiler mangling them differently. All four are
 * the real classes now, and the casts that reached them go with them:
 * mMovingCylinderClsnWithPos IS a CylinderClsn by inheritance
 * (MovingCylinderClsnWithPos -> MovingCylinderClsn -> CylinderClsn), so Clear() and
 * Update() are called directly.
 *
 * The two fields this used to spell as its own were the ModelAnim's: `mAnimation`
 * at 0x124 is the Animation base inside mModelAnim at +0x50, and `unk_130` at 0x130
 * is that base's `speed` at +0x0c. Advancing "the animation" is advancing the model.
 */
#include "types.h"
#include "decl_common.h"
#include "Bowser.h"

extern "C" {
extern int RandomIntInternal(int* seed);
extern s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern int data_0209e650;
extern char* data_0209f318;
}

int Bowser::Behavior()
{
    RandomIntInternal(&data_0209e650);
    mTargetPlayer = (int)ClosestPlayer();
    if (*(Actor**)((char*)&mTargetPlayer) != 0) {
        unk_406 = Vec3_HorzAngle((Vector3*)((char*)&mPosX), (Vector3*)((char*)*(Actor**)((char*)&mTargetPlayer) + 0x5c));
        unk_3ec = Vec3_HorzDist((Vector3*)((char*)&mPosX), (Vector3*)((char*)*(Actor**)((char*)&mTargetPlayer) + 0x5c));
    } else {
        unk_406 = mAngleY;
        unk_3ec = ~0x80000000;
    }
    func_ov060_02112434(((char*)this));
    func_ov060_02111a28(((char*)this));
    mPrevAngleY = mAngleY;
    mModelAnim.speed = unk_3f8;
    mModelAnim.Advance();
    func_ov060_0211577c(((char*)this));
    *(char**)(data_0209f318 + 0x114) = ((char*)this);
    mMovingCylinderClsnWithPos.Clear();
    Vector3 v;
    v.z = 0x50000;
    v.x = 0;
    v.y = 0;
    mMovingCylinderClsnWithPos.SetPosRelativeToActor(v);
    mMovingCylinderClsnWithPos.Update();
    if (unk_42b != 0) {
        Actor* f = Actor::FindWithActorID(0x10d, 0);
        if (f == 0) unk_42b = 0;
    }
    return 1;
}
