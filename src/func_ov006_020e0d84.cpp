//cpp
struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov006_02141930[];
extern PMF data_ov006_021418d8[];
extern "C" void func_ov006_020e0d84(char *c, int i)
{
    C *self = (C *)c;
    int idx = i * 0x24;
    unsigned char k0 = *(unsigned char *)(c + idx + 0x4000 + 0x7aa);
    (self->*data_ov006_02141930[k0])(i);
    unsigned char k1 = *(unsigned char *)((char *)self + idx + 0x4000 + 0x7ab);
    (self->*data_ov006_021418d8[k1])(i);
}
