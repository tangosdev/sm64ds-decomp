//cpp
struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov006_021419f8[];
extern PMF data_ov006_021419a0[];
extern "C" void func_ov006_020e42b4(char *c, int i)
{
    C *self = (C *)c;
    int idx = i * 0x24;
    unsigned char k0 = *(unsigned char *)(c + idx + 0x4000 + 0x8de);
    (self->*data_ov006_021419f8[k0])(i);
    unsigned char k1 = *(unsigned char *)((char *)self + idx + 0x4000 + 0x8df);
    (self->*data_ov006_021419a0[k1])(i);
}
