//cpp
extern "C" {
int func_ov002_020f63a0(void* thiz);
int func_ov002_020f23d0(void* c);
}
struct Obj { virtual void v00(); virtual void m04(); };
extern "C" int _ZN8MugenBgm16CleanupResourcesEv(char* c){
  int r1 = *(int*)(c + 8);
  if (r1 == 0x2e) return func_ov002_020f63a0(c);
  if (r1 == 0x2f) return func_ov002_020f23d0(c);
  Obj* a = *(Obj**)(c + 0xdc);
  if (a) if (a) a->m04();
  Obj* b = *(Obj**)(c + 0xe0);
  if (b) if (b) b->m04();
  return 1;
}
