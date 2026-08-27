//cpp
struct C; typedef int (C::*PMF)();
struct C { char pad[0x364]; PMF *pp; };
extern "C" int Chuckya_ChangeState(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
