//cpp
// @symbol _ZN6Player20RegisterEggCoinCountEjbb
/* recovered: named members + shared header, real C++ method */
#include "Player.h"


void Player::RegisterEggCoinCount(unsigned int count, bool b2, bool b3)
{
	mEggParams = (count & 0xf) << 2;
	if (b3)
		*(unsigned char *)(((long long)(int)((char*)&mEggParams))) |= 0x40;
	if (b2)
		*(unsigned char *)(((long long)(int)((char*)&mEggParams))) |= 0x80;
}
