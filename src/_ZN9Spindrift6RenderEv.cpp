//cpp
struct Obj { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual void m5(int); };
extern "C" int _ZN9Spindrift6RenderEv(char* r0) {
  int b = (*(unsigned int*)(r0 + 0xb0) & 0x40000) ? 1 : 0; if (b) return 1;
  Obj* o = (Obj*)(r0 + 0x110);
  o->m5(0);
  return 1;
}
