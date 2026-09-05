//cpp
// @symbol _ZN12daStarGate_c17IsInFrontOfPlayerEP6Player
#include "daStarGate_c.h"
#include "Player.h"

extern "C" int AngleDiff(int a, int b);

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
