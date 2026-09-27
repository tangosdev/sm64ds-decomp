//cpp
#include "fBase_c.h"

extern "C" {
void* _ZN8dActor_c10FindWithIDEj(unsigned int);
void func_ov060_021180e0(char* c){
  char* a;
  a=(char*)_ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c+0x320));
  if(a==0){ ((fBase_c*)c)->MarkForDestruction(); return; }
  if(*(unsigned char*)(c+0x32b)!=0 && *(int*)(a+0x40c)==0xd){
    if((*(int*)(a+0x418)&0x10000)!=0) *(unsigned char*)(c+0x328)=2;
  }
  if(*(signed char*)(a+0x41e)==1){
    if(*(int*)(a+0x40c)==3 || *(int*)(a+0x410)!=0){
      *(unsigned char*)(c+0x32a)=1;
    }
  }
  if(*(unsigned char*)(c+0x32a)==0){
    *(short*)(c+0x324)=0;
    return;
  }
  {
    int idx=*(unsigned char*)(c+0x329)*0x14;
    unsigned short* p=(unsigned short*)(c+0x324);
    if(idx<*(unsigned short*)(c+0x300+0x24)) *(unsigned char*)(c+0x328)=2;
    *p+=1;
  }
}
}
