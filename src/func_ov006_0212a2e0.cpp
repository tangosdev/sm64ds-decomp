//cpp
typedef unsigned char u8;
typedef unsigned int u32;

class C { public: int dummy; };
typedef void (C::*PMF)(int);

extern "C" PMF data_ov006_02143038[];
extern "C" int RandomIntInternal(int *seed);
extern "C" int data_0209d4b8;

extern "C" void func_ov006_0212a2e0(char *o)
{
    int i;
    char *q;
    int mask;

    q = o;
    i = 0;
    mask = 0x7fff;
    do {
        if (*(u8 *)(q + 0xbeae) != 0) {
            u8 fidx = *(u8 *)(q + 0xbeaf);
            (((C *)o)->*data_ov006_02143038[fidx])(i);
            if ((*(int *)(q + 0xbe98) - *(int *)(o + 0xab6c)) >> 12 >= 0xc8) {
                u32 rnd = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & mask;
                rnd = (rnd << 5) >> 0xf;
                *(int *)(q + 0xbe94) = rnd << 0xf;
                *(int *)(q + 0xbe98) = *(int *)(o + 0xab6c) - 0x8000;
                *(u8 *)(q + 0xbeb0) = 0;
                *(u8 *)(q + 0xbeaf) = 0;
                *(u8 *)(q + 0xbeb1) = 0;
            }
        }
        i++;
        q += 0x24;
    } while (i < 0x32);
}
