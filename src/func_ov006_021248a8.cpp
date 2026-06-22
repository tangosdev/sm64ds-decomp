//cpp
extern "C" void _ZN5ModelC1Ev(void*);
extern "C" void _ZN18TextureTransformerC1Ev(void*);
extern "C" void func_020733a8(void*, int, int, void*, void*);
extern void func_0203d47c(void);
extern void func_0203d738(void);
extern "C" void* func_ov006_021248a8(char* c){
  _ZN5ModelC1Ev(c + 0xa0);
  _ZN5ModelC1Ev(c + 0xf0);
  _ZN5ModelC1Ev(c + 0x140);
  _ZN18TextureTransformerC1Ev(c + 0x194);
  func_020733a8(c + 0x1a8, 5, 8, (void*)&func_0203d738, (void*)&func_0203d47c);
  return c;
}
