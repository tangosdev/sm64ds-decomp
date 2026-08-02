//cpp
// @symbol _ZN6Player19St_CrazedCrate_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
/* _ZN6Player19St_CrazedCrate_InitEv at 0x020e0fac (ov002), size 0x74
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
}

int Player::St_CrazedCrate_Init()
{
  mJumpedFromQuicksand = 0;
  mIsInAirState = 1;
  mIsFallScreaming = 0;
  mJumpComboStage = 0;
  mIsAirborne = 1;
  mLandSoundPlayed = 0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x43, 0x40000000, 0x1000, 0);
  func_ov002_020e0f38(((char*)this), mJumpComboStage);
  mPrevAngleY = mAngleY;
  *(int*)(((long long)(int)((char*)&mBodyClsnFlags))) |= 0x20;
  return 1;
}
