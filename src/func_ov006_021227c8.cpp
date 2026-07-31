//cpp
extern "C" {
extern void __destroy_arr(void* a, int b, int c, void* d);
extern void NullDestructor_0203d47c(void);
extern int _ZN18TextureTransformerD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
int func_ov006_021227c8(char* c){
  __destroy_arr(c+0x1a8, 5, 8, (void*)NullDestructor_0203d47c);
  _ZN18TextureTransformerD1Ev(c+0x194);
  _ZN5ModelD1Ev(c+0x140);
  _ZN5ModelD1Ev(c+0xf0);
  _ZN5ModelD1Ev(c+0xa0);
  return (int)c;
}
}
