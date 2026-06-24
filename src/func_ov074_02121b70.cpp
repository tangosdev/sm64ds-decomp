//cpp
extern "C" {
extern int func_ov074_021222e0(void* c);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* a, void* b);
extern void _ZN15MaterialChanger6UpdateER15ModelComponents(void* a, void* b);
extern void _ZN18TextureTransformer6UpdateER15ModelComponents(void* a, void* b);
}
struct Sub {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void v3(); virtual void v4(); virtual void m(void*);
};
extern "C" int func_ov074_02121b70(char* c){
  if(*(int*)(c+8)==0x1111) return func_ov074_021222e0(c);
  if(*(unsigned char*)(c+0x60a)==0) return 1;
  Sub* s = (Sub*)(c+0x210);
  s->m(c+0x80);
  _ZN15TextureSequence6UpdateER15ModelComponents(c+0x3e4, c+0x218);
  _ZN15MaterialChanger6UpdateER15ModelComponents(c+0x3d0, c+0x218);
  _ZN18TextureTransformer6UpdateER15ModelComponents(c+0x3f8, c+0x218);
  return 1;
}
