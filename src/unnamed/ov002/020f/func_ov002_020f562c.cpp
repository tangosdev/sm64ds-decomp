//cpp
struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov002_02110eec[];

extern "C" void func_ov002_020f562c(C* self, int idx) {
    unsigned char sel = *((unsigned char*)((char*)self + idx * 0x4c + 0x47));
    (self->*data_ov002_02110eec[sel])(idx);
}