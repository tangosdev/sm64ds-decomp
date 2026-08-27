//cpp
// @symbol _ZN6Player17St_Headstand_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct Camera;
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void *c, unsigned int a, int b, int f, unsigned int d);
extern void func_0200d580(struct Camera *thiz, int playerID);
}
extern struct Camera *data_0209f318;

int Player::St_Headstand_Init()
{
  _ZN6Player7SetAnimEji5Fix12IiEj(((void *)this), 0x1e, 0x40000000, 0x1000, 0);
  *(unsigned char*)((char*)&mStateStep)=2;
  *(int*)((char*)&mVertAccel)=0;
  *(int*)((char*)&mHorzSpeed)=0;
  *(int*)((char*)&mVertSpeed)=0;
  func_0200d580(data_0209f318, *(unsigned char*)((char*)&mPlayerNo));
  *(unsigned char*)((char*)&unk_717)=1;
  return 1;
}
