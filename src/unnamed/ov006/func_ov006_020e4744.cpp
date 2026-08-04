//cpp
struct C;
typedef void (C::*PMF)(int);
extern "C" PMF data_ov006_021419b8[];

extern "C" void func_ov006_020e4744(char *o, int i)
{
    unsigned char idx = *(unsigned char *)(o + i * 0x24 + 0x48de);
    (((C *)o)->*data_ov006_021419b8[idx])(i);
}
