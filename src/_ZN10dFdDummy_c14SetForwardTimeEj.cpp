//cpp
// @symbol _ZN10dFdDummy_c14SetForwardTimeEj
/* dFdDummy_c::SetForwardTime -- recovered from vtable slot identity.
 * Writes speed = 0x1000, then makes a genuine virtual call through slot 6
 * (dFdDummy_c overrides none of slots 5-9, so this lands on
 * FaderColor/FaderBrightness's own inherited body). */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void m(); };
extern "C" void _ZN10dFdDummy_c14SetForwardTimeEj(Base *c){ *(int*)((char*)c+8)=0x1000; c->m(); }
