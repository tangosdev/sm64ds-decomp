//cpp
struct C;
typedef int (C::*PMF)();
struct Entry { char pad[0x8]; PMF pmf; };
struct C { char pad[0x84]; Entry *ep; };
extern "C" int func_ov075_0211b418(C *c) {
  (c->*c->ep->pmf)();
  return 1;
}
