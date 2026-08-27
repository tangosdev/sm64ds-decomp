//cpp
struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov002_02110e64[];

extern "C" void func_ov002_020f5848(C* self, int idx) {
    unsigned char sel = *((unsigned char*)((char*)self + idx * 0x4c + 0x47));
    (self->*data_ov002_02110e64[sel])(idx);
}