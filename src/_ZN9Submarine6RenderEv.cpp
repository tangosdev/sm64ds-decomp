//cpp
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Base { char pad[0x114]; Sub sub; };
extern "C" void _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);
extern "C" int _ZN9Submarine6RenderEv(Base *c) { _ZN18TextureTransformer6UpdateER15ModelComponents((char *)c + 0x178, (char *)c + 0x11c); Sub *b = &c->sub; b->m(0); return 1; }
