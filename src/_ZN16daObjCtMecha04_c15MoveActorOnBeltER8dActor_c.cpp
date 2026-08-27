//cpp
// @symbol _ZN16daObjCtMecha04_c15MoveActorOnBeltER8dActor_c
/* Inferred descriptive name. The collision callback supplies this conveyor as
 * owner and the actor whose X/Z position should advance with the belt. */
#include "daObjCtMecha04_c.h"

extern "C" s16 data_02082214[];

void daObjCtMecha04_c::MoveActorOnBelt(dActor_c &actor)
{
  u16 ang = (u16)mAngleY;
  int idx = ang >> 4;
  int v = mBeltSpeed << 2;
  int c = data_02082214[idx * 2];
  int res1 = (int)((((s64)v * c) + 0x800) >> 12);
  s32 *actorPosX = &actor.mPosX;
  int oldX = *actorPosX;
  *actorPosX = oldX + res1;

  u16 ang2 = (u16)mAngleY;
  int idx2 = ang2 >> 4;
  int v2 = mBeltSpeed << 2;
  s32 *actorPosZ = actorPosX + 2;
  int s = data_02082214[(idx2 * 2) + 1];
  int oldZ = *actorPosZ;
  int res2 = (int)((((s64)v2 * s) + 0x800) >> 12);
  *actorPosZ = oldZ + res2;
}
