//cpp
// @symbol _ZN6Player12Unk_020ca8f8Ev
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern Player::State data_ov002_0211064c;
extern Player::State data_ov002_02110664;

int Player::Unk_020ca8f8()
{
  if(IsState(data_ov002_0211064c)) return 1;
  if(IsState(data_ov002_02110664)) return 2;
  return 0;
}
