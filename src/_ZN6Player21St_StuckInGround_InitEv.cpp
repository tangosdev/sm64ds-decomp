//cpp
// @symbol _ZN6Player21St_StuckInGround_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020c9e40(void*);
extern int func_ov002_020dab14(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int,void*);
extern int func_ov002_020c5444(void*);
}

int Player::St_StuckInGround_Init()
{
  func_ov002_020c9e40(((void*)this));
  func_ov002_020dab14(((void*)this));
  unsigned char idx = *(unsigned char*)((char*)&mStateStep);
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), data_ov002_0210a560[idx], 0x40000000, 0x1000, 0);
  _ZN5Sound9PlayBank0EjRK7Vector3(*(int*)((char*)&mGroundSoundType)+0x80, (char*)((void*)this)+0x74);
  func_ov002_020c5444(((void*)this));
  *(int*)((char*)&mVertSpeed)=0;
  *(int*)((char*)&mHorzSpeed)=0;
  *(unsigned char*)((char*)&mStateWork)=0;
  return 1;
}
