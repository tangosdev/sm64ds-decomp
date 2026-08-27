//cpp
struct C;
typedef void (C::*PMF)(int);
extern "C" PMF data_ov006_02142d98[];

extern "C" void func_ov006_0211a4b0(char *o, int i)
{
    unsigned char idx = *(unsigned char *)(o + i * 0x24 + 0x51d1);
    (((C *)o)->*data_ov006_02142d98[idx])(i);
}
