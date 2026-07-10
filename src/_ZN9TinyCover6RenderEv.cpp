//cpp
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Base { char pad[0xd4]; Sub sub; };
extern "C" void _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);
extern "C" int _ZN9TinyCover6RenderEv(Base *c) { _ZN18TextureTransformer6UpdateER15ModelComponents((char *)c + 0x320, (char *)c + 0xdc); Sub *b = &c->sub; b->m(0); return 1; }
