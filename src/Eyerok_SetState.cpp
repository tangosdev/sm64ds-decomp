//cpp
struct C; typedef int (C::*PMF)();
struct C { char pad[0x48c]; PMF *pp; };
extern "C" int Eyerok_SetState(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
