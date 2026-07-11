//cpp
typedef unsigned char u8;
struct C;
typedef void (C::*PMF)();
extern "C" {
void func_ov006_02104c60(char *c);
}
extern PMF data_ov006_021427ec[];
void func_ov006_02104c60(char *c)
{
    unsigned short *ip;
    if (*(u8*)(c + 0x4000 + 0x684) == 0) return;
    ip = (unsigned short*)(((int)c + 0x4680) & 0xFFFFFFFFFFFFFFFFLL);
    *ip = *ip - 1;
    if (*(short*)(c + 0x4600 + 0x80) <= 0) {
        *(short*)(c + 0x4600 + 0x80) = 0;
        *(u8*)(c + 0x4000 + 0x684) = 0;
        *(u8*)(c + 0x4000 + 0x685) = 0;
        return;
    }
    {
        C *cc = (C*)c;
        (cc->*data_ov006_021427ec[*(u8*)(c + 0x4000 + 0x686)])();
    }
}
