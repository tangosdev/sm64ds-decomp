//cpp
extern "C" int RandomIntInternal(int *seed);
extern int data_0209d4b8;

struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov006_021418c0[];

extern "C" void func_ov006_020e12d0(char *o)
{
    int i;
    char *e = o;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(e + 0x47a8) != 0) {
            unsigned char idx = *(unsigned char *)(e + 0x47a9);
            (((C *)o)->*data_ov006_021418c0[idx])(i);
            if ((*(int *)(e + 0x4790) >> 0xc) >= 0xc8) {
                *(int *)(e + 0x478c) = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf << 0xf;
                *(int *)(e + 0x4790) = -0x8000;
                *(unsigned char *)(e + 0x47aa) = 0;
                *(unsigned char *)(e + 0x47a9) = 0;
                *(unsigned char *)(e + 0x47ab) = 0;
            }
        }
        e += 0x24;
    }
}
