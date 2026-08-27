//cpp
struct C; typedef void (C::*PMF)();
extern PMF data_ov002_02110f34[];
struct C { char pad[4]; };
extern "C" void func_ov002_020f5dd8(C* c, int idx) {
  (c->*data_ov002_02110f34[idx])();
}
