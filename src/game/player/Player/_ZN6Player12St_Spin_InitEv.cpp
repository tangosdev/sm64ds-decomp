//cpp
// @symbol _ZN6Player12St_Spin_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
typedef int Fix12i;
struct Camera;
extern int* data_0209f318;
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, Fix12i d, unsigned int e);
extern void func_ov002_020e25f0(void* c, int i);
extern void func_0200d678(Camera* thiz, unsigned char pid);
}

int Player::St_Spin_Init()
{
  mJumpedFromQuicksand = 0;
  mIsInAirState = 1;
  mIsFallScreaming = 0;
  mJumpComboStage = 0;
  mIsAirborne = 1;
  mLandSoundPlayed = 0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x5f, 0, 0x1000, 0);
  mTerminalVelocity = -0x10000;
  if (mStatePhase) {
    mVertSpeed = 0x50000;
    func_ov002_020e25f0(((char*)this), 2);
  }
  int* p = (int*)((int)((char*)this) + 0x2ec);
  int old = *p;
  int** cam_ptr_ptr = &data_0209f318;
  *p = old | 0x20;
  unsigned char pid = mPlayerNo;
  func_0200d678((Camera*)*cam_ptr_ptr, pid);
  return 1;
}
