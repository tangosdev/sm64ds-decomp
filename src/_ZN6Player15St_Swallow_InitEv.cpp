//cpp
// @symbol _ZN6Player15St_Swallow_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
struct State; extern State data_ov002_02110034;
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,State*);
extern int func_ov002_020c9e40(void*);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_Swallow_Init()
{
  if(*(void**)((char*)&mObjInMouth)){
    void* o=*(void**)((char*)&mObjInMouth);
    int(*f)(void*)=*(int(**)(void*))(*(char**)o+0x48);
    if(f(o)==4){
      func_ov002_020c9e40(((char*)this));
    }
    int eq = *(unsigned short*)(*(char**)((char*)&mObjInMouth)+0xc)==0xbf;
    if(eq!=0){
      _ZN6Player11ChangeStateERNS_5StateE(((char*)this),&data_ov002_02110034);
      return 1;
    }
  }
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x70,0x40000000,0x1000,0);
  return 1;
}
