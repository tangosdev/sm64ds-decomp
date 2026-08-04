//cpp
struct C;
typedef void (C::*PMF)(int);
extern "C" PMF data_ov006_02143050[];

extern "C" void func_ov006_0212a224(char *base, int idx)
{
    unsigned char state = *(unsigned char *)(base + idx * 0x24 + 0xbeb0);
    (((C *)base)->*data_ov006_02143050[state])(idx);
}
