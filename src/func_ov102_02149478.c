extern void func_ov102_02149684(void*, void*);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, void*, void*, int, int);
extern void _ZN9ActorBase18MarkForDestructionEv(void*);
void func_ov102_02149478(char* c){
  char local[12];
  func_ov102_02149684(local, c);
  _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, *(unsigned char*)(c+0x3f1)|0x40, local, (void*)0, *(signed char*)(c+0xcc), -1);
  _ZN9ActorBase18MarkForDestructionEv(c);
}
