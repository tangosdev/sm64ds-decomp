//cpp
extern "C" {
extern void _ZN9ActorBase18MarkForDestructionEv(void* c);
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN12CylinderClsn5ClearEv(void* a);
extern void _ZN12CylinderClsn6UpdateEv(void* a);

int _ZN10PyramidTag8BehaviorEv(char* c){
  if(*(int*)(c+0xf8) != 0){
    unsigned int id = *(unsigned int*)(c+0x108);
    if(id == 0){
      _ZN9ActorBase18MarkForDestructionEv(c);
      return 1;
    }
    char* a = _ZN5Actor10FindWithIDEj(id);
    if(a){
      unsigned char* p = (unsigned char*)(((int)a + 0x3b6) & 0xFFFFFFFFFFFFFFFF);
      *p += 1;
    }
    _ZN9ActorBase18MarkForDestructionEv(c);
    return 1;
  }
  _ZN12CylinderClsn5ClearEv(c+0xd4);
  _ZN12CylinderClsn6UpdateEv(c+0xd4);
  return 1;
}
}
