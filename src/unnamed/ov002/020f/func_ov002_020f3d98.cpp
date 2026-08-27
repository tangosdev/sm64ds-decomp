//cpp
struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov002_02110e34[];

extern "C" void func_ov002_020f3d98(C* self, int idx) {
    unsigned char sel = *((unsigned char*)((char*)self + idx * 0x4c + 0x47));
    (self->*data_ov002_02110e34[sel])(idx);
}