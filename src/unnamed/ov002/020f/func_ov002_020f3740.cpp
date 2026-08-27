//cpp
struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov002_02110e9c[];

extern "C" void func_ov002_020f3740(C* self, int idx) {
    unsigned char sel = *((unsigned char*)((char*)self + idx * 0x30 + 0x15e));
    (self->*data_ov002_02110e9c[sel])(idx);
}
