//cpp
// @symbol _ZN6Player18TurnOffToonShadingEj
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int, int);
extern int _ZN5Model14SetPolygonModeEi(void*, int);
extern int func_ov002_020e6b74(void*, int);
}

void Player::TurnOffToonShading(unsigned int j)
{
  void* m;
  m = mBodyModels[_ZNK6Player14GetBodyModelIDEjb(((char*)this), j, 0)];
  _ZN5Model14SetPolygonModeEi(m, 0);
  func_ov002_020e6b74(m, unk_27c[j]);
  m = unk_154[j];
  _ZN5Model14SetPolygonModeEi(m, 0);
  func_ov002_020e6b74(m, unk_28c[j]);
  m = unk_154[j + 4];
  _ZN5Model14SetPolygonModeEi(m, 0);
  func_ov002_020e6b74(m, unk_28c[j + 4]);
}
