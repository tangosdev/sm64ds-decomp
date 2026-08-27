//cpp
extern "C" int RandomIntInternal(int *seed);
extern int data_0209d4b8;

struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov006_02141988[];

extern "C" void func_ov006_020e4800(char *o)
{
    int i;
    char *e = o;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(e + 0x48dc) != 0) {
            unsigned char idx = *(unsigned char *)(e + 0x48dd);
            (((C *)o)->*data_ov006_02141988[idx])(i);
            if ((*(int *)(e + 0x48c4) >> 0xc) >= 0xc8) {
                *(int *)(e + 0x48c0) = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf << 0xf;
                *(int *)(e + 0x48c4) = -0x8000;
                *(unsigned char *)(e + 0x48de) = 0;
                *(unsigned char *)(e + 0x48dd) = 0;
                *(unsigned char *)(e + 0x48df) = 0;
            }
        }
        e += 0x24;
    }
}
