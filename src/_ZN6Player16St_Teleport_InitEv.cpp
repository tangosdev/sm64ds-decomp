//cpp
// @symbol _ZN6Player16St_Teleport_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020c9e40(char*c);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned,void*);
}

int Player::St_Teleport_Init()
{
  func_ov002_020c9e40(((char*)this));
  mStateWork=0;
  mStateStep=0;
  mHorzSpeed=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x47,0,0x1000,0);
  _ZN5Sound9PlayBank0EjRK7Vector3(0x19,(char*)((char*)this)+0x74);
  return 1;
}
