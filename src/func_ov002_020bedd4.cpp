//cpp
extern "C" {
extern unsigned int _ZNK6Player14GetBodyModelIDEjb(char* c, unsigned int a, int b);
extern void _ZN9Animation7AdvanceEv(char* anim);
extern int _ZN6Player6IsAnimEj(char* c, unsigned int a);

void func_ov002_020bedd4(char* self){
  unsigned int id = _ZNK6Player14GetBodyModelIDEjb(self, *(int*)(self+8) & 0xff, 0);
  _ZN9Animation7AdvanceEv((char*)*(int*)(self + (id << 2) + 0xdc) + 0x50);
  if (_ZN6Player6IsAnimEj(self, 0x9a) || _ZN6Player6IsAnimEj(self, 0x19) ||
      _ZN6Player6IsAnimEj(self, 0xac) || _ZN6Player6IsAnimEj(self, 0x6b) ||
      _ZN6Player6IsAnimEj(self, 0x85) || _ZN6Player6IsAnimEj(self, 0x94) ||
      _ZN6Player6IsAnimEj(self, 0x98) || _ZN6Player6IsAnimEj(self, 0x99)) {
    *(int*)(self + *(int*)(self+8) * 0x14 + 0x1e4) = 0;
  }
  _ZN9Animation7AdvanceEv(self + 0x1dc + *(int*)(self+8) * 0x14);
}
}
