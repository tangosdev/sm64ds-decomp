//cpp
#include "EnemySpawner.h"

extern "C" {
extern int _ZN5Event6GetBitEj(unsigned int bit);
extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
  unsigned int id, unsigned int a2, void* pos, void* rot, int a5, int a6);
extern void func_ov102_0214ad14(void* c);
}

int EnemySpawner::Behavior()
{
    if (_ZN5Event6GetBitEj(mEventBit) && mPreviousEventBit == 0) {
        void *actor = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            mActorToSpawn, 4, &mPosX, &mPrevAngleX, mAreaId, -1);
        if (actor != 0)
            func_ov102_0214ad14(actor);
    }
    mPreviousEventBit = _ZN5Event6GetBitEj(mEventBit);
    return 1;
}
