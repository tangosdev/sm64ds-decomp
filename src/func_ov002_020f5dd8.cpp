//cpp
struct C;
typedef void (C::*PMF)();
extern PMF data_ov002_02110f34[];

extern "C" void func_ov002_020f5dd8(C *self, int i) {
    (self->*data_ov002_02110f34[i])();
}
