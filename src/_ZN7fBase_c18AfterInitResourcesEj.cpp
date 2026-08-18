//cpp
// @symbol _ZN9ActorBase18AfterInitResourcesEj
/* recovered: named members + shared header, real C++ method */
#include "ActorBase.h"
extern "C" {
extern int data_020a4b88[]; extern int data_02099f24[]; extern int data_020a4b78[]; extern int data_020a4b98[];
extern void func_0203b27c(void*, void*); extern void func_0204405c(void*, void*);
}

void ActorBase::AfterInitResources(unsigned int a)
{
  if(a!=2) return;
  func_0203b27c(data_020a4b88, ((char*)this)+0x28);
  volatile int* p=data_02099f24; bool b=(p[0]==3); if(b){ *(bool*)((char*)&unk_010)=true; return; }
  func_0204405c(data_020a4b78, ((char*)this)+0x28);
  func_0204405c(data_020a4b98, ((char*)this)+0x38);
  *(bool*)((char*)&aliveState)=true;
}
