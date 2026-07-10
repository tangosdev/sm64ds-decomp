//cpp
struct Obj { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void*); };
extern "C" {
extern void _ZN9Animation7AdvanceEv(void* a);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* a, void* b);
int _ZN6Bowser6RenderEv(char* c){
  if(*(unsigned char*)(c+0x41c) < 8) return 1;
  _ZN9Animation7AdvanceEv(c+0x138);
  _ZN15TextureSequence6UpdateER15ModelComponents(c+0x138, c+0xdc);
  ((Obj*)(c+0xd4))->m(c+0x80);
  return 1;
}
}
