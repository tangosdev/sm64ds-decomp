//cpp
#include "types.h"
// @symbol _ZN6Bowser8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Bowser.h"
struct Actor {
    Actor* ClosestPlayer();
    static Actor* FindWithActorID(unsigned id, Actor* a);
};
struct Animation { void Advance(); };
struct CylinderClsn { void Clear(); void Update(); };
struct MovingCylinderClsnWithPos { void SetPosRelativeToActor(const Vector3& v); };

Actor* Actor::ClosestPlayer();
Actor* Actor::FindWithActorID(unsigned, Actor*);
void Animation::Advance();
void CylinderClsn::Clear();
void CylinderClsn::Update();
void MovingCylinderClsnWithPos::SetPosRelativeToActor(const Vector3&);

extern "C" {
extern int RandomIntInternal(int* seed);
extern s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern Fix12 Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern int data_0209e650;
extern char* data_0209f318;
}

int Bowser::Behavior()
{
    RandomIntInternal(&data_0209e650);
    mTargetPlayer = (int)((Actor*)((char*)this))->ClosestPlayer();
    if (*(Actor**)((char*)&mTargetPlayer) != 0) {
        unk_406 = Vec3_HorzAngle((Vector3*)((char*)&mPosX), (Vector3*)((char*)*(Actor**)((char*)&mTargetPlayer) + 0x5c));
        unk_3ec = Vec3_HorzDist((Vector3*)((char*)&mPosX), (Vector3*)((char*)*(Actor**)((char*)&mTargetPlayer) + 0x5c));
    } else {
        unk_406 = unk_08e;
        unk_3ec = ~0x80000000;
    }
    func_ov060_02112434(((char*)this));
    func_ov060_02111a28(((char*)this));
    unk_094 = unk_08e;
    unk_130 = unk_3f8;
    ((Animation*)((char*)&mAnimation))->Advance();
    func_ov060_0211577c(((char*)this));
    *(char**)(data_0209f318 + 0x114) = ((char*)this);
    ((CylinderClsn*)((char*)&mMovingCylinderClsnWithPos))->Clear();
    Vector3 v;
    v.z = 0x50000;
    v.x = 0;
    v.y = 0;
    ((MovingCylinderClsnWithPos*)((char*)&mMovingCylinderClsnWithPos))->SetPosRelativeToActor(v);
    ((CylinderClsn*)((char*)&mMovingCylinderClsnWithPos))->Update();
    if (unk_42b != 0) {
        Actor* f = Actor::FindWithActorID(0x10d, 0);
        if (f == 0) unk_42b = 0;
    }
    return 1;
}
