//cpp
extern "C" {
int _ZN7fBase_c18MarkForDestructionEv(void*);
void* _ZN8dActor_c10FindWithIDEj(unsigned int);
int func_ov010_0211139c(char *c){
  unsigned int id=*(unsigned int*)(c+0x3ac);
  if(id==0){ _ZN7fBase_c18MarkForDestructionEv(c); return 0; }
  void *a=_ZN8dActor_c10FindWithIDEj(id);
  if(a==0){ _ZN7fBase_c18MarkForDestructionEv(c); }
  return (int)a;
}
}
