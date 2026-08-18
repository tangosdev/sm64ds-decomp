//cpp
extern "C" {
extern char* _ZN8dActor_c13SpawnSoundObjEj(void* self, unsigned int id);
extern int func_ov002_020ea3a4(void* c);
void func_ov002_020e6fbc(char* c, int arg){
  if (*(void**)(c+0x430) != 0) return;
  char* s = _ZN8dActor_c13SpawnSoundObjEj(c, 4);
  if (s == 0) return;
  *(void**)(c+0x430) = *(void**)(s+4);
  *(int*)(s+0xd4) = func_ov002_020ea3a4(c);
  *(int*)(s+0xd8) = arg;
}
}
