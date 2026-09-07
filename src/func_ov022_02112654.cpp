//cpp
extern "C" {
extern void* _ZN8dActor_c10FindWithIDEj(unsigned int);
extern void _ZN6Player4BurnEv(void*);
extern void _ZN7fBase_c18MarkForDestructionEv(void*);
void func_ov022_02112654(char* c){
  if(*(unsigned int*)(c+0xf8)==0) return;
  void* a=_ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c+0xf8));
  if(a==0) return;
  unsigned int b=*(unsigned short*)((char*)a+0xc)==0xbf;
  if(b==0) return;
  _ZN6Player4BurnEv(a);
  _ZN7fBase_c18MarkForDestructionEv(c);
}
}
