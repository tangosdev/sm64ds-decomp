//cpp
extern "C" {
extern void _ZN6Player16IncMegaKillCountEv(void*);
extern void func_02012694(int a, void* b);
extern void _ZN8Platform14KillByMegaCharER6Player(void* self, void* p);
void func_ov091_021310fc(char* self, void* p){
  unsigned short h = *(unsigned short*)(self+0xc);
  int eq = (h == 0x1e);
  if(eq) return;
  _ZN6Player16IncMegaKillCountEv(p);
  func_02012694(0x1e, self+0x74);
  _ZN8Platform14KillByMegaCharER6Player(self, p);
  *(short*)(self+0x8e)=*(short*)(self+0x94);
}
}
