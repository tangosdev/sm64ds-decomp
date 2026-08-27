//cpp
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_02142d50[];

#define F1E(b,i) (*(unsigned char*)((char*)(b) + 0x51d1 + (i)*0x24))

extern "C" void func_ov006_0211b398(char *base, int idx)
{
    unsigned char state = F1E(base, idx);
    (((C*)base)->*data_ov006_02142d50[state].pmf)(idx);
}
