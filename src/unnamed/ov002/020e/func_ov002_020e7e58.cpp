//cpp
extern "C" {
int _ZN7fBase_c18MarkForDestructionEv(void*);
void* _ZN8dActor_c10FindWithIDEj(unsigned int);
void func_ov002_020e7e58(char* c){
  unsigned int id;
  void* a;
  if(*(unsigned char*)(c+0x49c)==0) return;
  id=*(unsigned int*)(c+0x430);
  if(id==0) return;
  a=_ZN8dActor_c10FindWithIDEj(id);
  if(a!=0){
    if(*(unsigned char*)(c+0x49c)==1) *(short*)((char*)a+0xde)=0;
    else _ZN7fBase_c18MarkForDestructionEv(a);
  }
  *(int*)(c+0x430)=0;
}
}
