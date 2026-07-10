//cpp
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void *); };

extern "C" void _ZN15TextureSequence6UpdateER15ModelComponents(void *, void *);

extern "C" int _ZN13RacingPenguin6RenderEv(char *c) {
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x138, c + 0xdc);
    Sub *s = (Sub *)(c + 0xd4);
    s->m(c + 0x80);
    return 1;
}
