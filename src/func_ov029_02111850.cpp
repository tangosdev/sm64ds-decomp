//cpp
extern "C" {
char* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);
void _ZN7fBase_c18MarkForDestructionEv(void*);
void func_ov029_02111850(char* c){
  char* a;
  if(*(int*)(c+0x158) != 0) return;
  a = 0;
  for(;;){
    a = _ZN8dActor_c15FindWithActorIDEjPS_(0x65, a);
    if(a == 0) goto done;
    if(*(unsigned char*)(c+0x15c) == *(unsigned char*)(a+0x338)) break;
  }
  *(int*)(c+0x158) = *(int*)(a+4);
done:
  if(*(int*)(c+0x158) != 0) return;
  _ZN7fBase_c18MarkForDestructionEv(c);
}
}
