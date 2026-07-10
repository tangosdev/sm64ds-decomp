//cpp
extern "C" {
struct Base{ virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void*); };
int _ZN13MontyMoleRock6RenderEv(char*o){
  Base*b=(Base*)(o+0x110);
  b->m(o+0x80);
  return 1;
}
}
