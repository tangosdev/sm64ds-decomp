//cpp
// @symbol _ZN6Player16St_Climb_CleanupEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020caf68(void *c);
}
extern int data_ov002_021106f4[];

int Player::St_Climb_Cleanup()
{
  int v = *(int*)((char*)&unk_378);
  if(v != (int)data_ov002_021106dc && v != (int)data_ov002_021106f4){
    func_ov002_020caf68(((void *)this));
    *(unsigned short*)((char*)((void *)this)+0x600+0xb6)=8;
  }
  *(unsigned short*)((char*)((void *)this)+0x600+0xa8)=0;
  return 1;
}
