//cpp
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_02142d20[];

extern "C" void func_ov006_0211a7ac(char *base, int idx) {
    unsigned char state = *(unsigned char*)((char*)base + idx * 0x24 + 0x51d1);
    (((C*)base)->*data_ov006_02142d20[state].pmf)(idx);
}
