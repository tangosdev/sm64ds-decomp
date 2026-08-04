//cpp
struct C; typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern Entry data_ov006_021426f4[];
struct C { char pad[1]; };
extern "C" void func_ov006_02102e8c(C *c);
#pragma opt_strength_reduction off
void func_ov006_02102e8c(C *c) {
    int i;
    for (i = 0; i < 0x30; i++) {
        if (*(unsigned char *)((char *)c + i * 0x40 + 0x4698) != 0) {
            unsigned char k = *(unsigned char *)((char *)c + i * 0x40 + 0x4699);
            (c->*data_ov006_021426f4[k].pmf)(i);
        }
    }
}
