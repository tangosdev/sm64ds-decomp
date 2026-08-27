//cpp
// @symbol _ZN6Player9GetHealthEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"

int Player::GetHealth()
{
  unsigned char i = mPlayerNo;
  return (signed char)(data_02092144[i] >> 8);
}
