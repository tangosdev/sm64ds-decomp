//cpp
// @symbol _ZN6Player16St_WallJump_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern int func_ov004_020afdd0(int a,int b,int c,int d,int e);

void Player::St_WallJump_Init()
{
  if(*(unsigned char*)(((char*)this)+0x4ee5)==0) return;
  int x=*(int*)(((char*)this)+0x4eb0);
  int y=*(int*)(((char*)this)+0x4eb4);
  func_ov004_020afdd0((int)data_ov006_0213c2e4,(x>>12)-0x20,(y>>12)-8,-1,0);
}
