//cpp
// @symbol _ZN6Player17St_WallSlide_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int Fix12i;
extern void func_ov002_020c2f64(void *c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *p, void *st);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(Fix12i x, Fix12i y, Fix12i z);
extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5);
extern void func_ov002_020bedd4(void *p);
extern unsigned char data_020a0e40;
extern unsigned char data_0209f49e[];
extern char data_ov002_0211013c;
extern char data_ov002_021101b4;

int Player::St_WallSlide_Main()
{
  *((int *) ((char *)&mPeakY)) = *((int *) ((char *)&mPosY));
  if ((*((unsigned char *) ((char *)&mIsAirborne))) == 0)
  {
    func_ov002_020c2f64(((char *)this));
    _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
  }
  else
  {
    if ((*((unsigned char *) ((char *)&mIsMega))) != 0)
    {
      func_02022b04(*((int *) ((char *)&mPosX)), *((int *) ((char *)&mPosY)), *((int *) ((char *)&mPosZ)));
    }
    else
    {
      _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*((int *) ((char *)&mPosX)), *((int *) ((char *)&mPosY)), *((int *) ((char *)&mPosZ)));
    }
    *((int *) ((char *)&mLoopingSoundHandle)) = func_0201226c(*((int *) ((char *)&mLoopingSoundHandle)), 0, (*((int *) ((char *)&mGroundSoundType))) + 0xe2, (int) ((char *)&mCamSpacePos), *((int *) ((char *)&mHorzSpeed)), 0);
    if ((*((unsigned short *) (&data_0209f49e[data_020a0e40 * 0x18]))) & 2)
    {
      char *p = ((char *)this) + 0x600;
      if (*((unsigned short *) (p + 0xa4)))
      {
        *((short *) ((char *)&mAngleY)) = *((short *) (((char *)&unk_600) + 0x9c));
      }
      _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_021103dc);
    }
    else
    {
      if ((*((unsigned char *) ((char *)&mClsnFlags))) & 2)
      {
        goto end;
      }
      _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_021101b4);
    }
  }
  end:
  func_ov002_020bedd4(((char *)this));
  return 1;
}
