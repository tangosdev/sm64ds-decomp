//cpp
// @symbol _ZN6Player9IsOnShellEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern Player::State data_ov002_02110304;

int Player::IsOnShell()
{
  if(IsState(data_ov002_02110304)) return 1;
  *(int*)((char*)&mRidingShell)=0;
  return 0;
}
