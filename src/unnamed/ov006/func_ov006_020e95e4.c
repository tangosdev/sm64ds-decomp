extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020e95e4(char *c)
{
    int v = *(int *)(c + 0xbc);
    int mult = 2;
    if (v >= 0xf) {
        *(int *)(c + 0x5544) = 3;
    } else if (v >= 0xa) {
        *(int *)(c + 0x5544) = 2;
    } else if (v >= 5) {
        *(int *)(c + 0x5544) = 1;
    } else {
        *(int *)(c + 0x5544) = 0;
        mult = 3;
    }
    *(unsigned char *)(c + 0x5552) =
        (unsigned char)((mult * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
}
