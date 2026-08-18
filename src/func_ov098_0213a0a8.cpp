//cpp
extern "C" {
int _ZN7fBase_c18MarkForDestructionEv(void*);
void* _ZN8dActor_c10FindWithIDEj(unsigned int);
int func_ov098_0213a0a8(char* c){
  void* a = _ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c+0x330));
  if (a == 0) return _ZN7fBase_c18MarkForDestructionEv(c);
  int v = *(int*)((char*)a+0x440);
  if (v == 4) { v = 0; *(unsigned char*)(c+0x342) = 0; }
  return v;
}
}
