//cpp
// @symbol _ZN6Player18St_CameraZoom_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct Camera;
extern "C" {
extern int func_0200d064(struct Camera* thiz, int playerID);
extern void func_0200d7e0(struct Camera* thiz, int playerID);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern struct Camera* data_0209f318;
}

int Player::St_CameraZoom_Init()
{
  *(int*)((char*)&mHorzSpeed)=0;
  *(int*)((char*)&mVertSpeed)=0;
  struct Camera* cam = data_0209f318;
  int pid = *(unsigned char*)((char*)&mPlayerNo);
  func_0200d064(cam, pid);
  func_0200d7e0(cam, *(unsigned char*)((char*)&mPlayerNo));
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this),0x47,0,0x1000,0);
  data_0209f28c=0;
  return 1;
}
