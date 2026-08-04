extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_02100084(char *c)
{
    unsigned int a, b;
    if (*(int *)(c + 0xbc) != 0) {
        a = ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6) >> 0xf;
        if (*(unsigned int *)(c + 0x5668) == a) {
            b = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 0xf) + 1;
            a = a + b;
            if ((int)a >= 6) a -= 6;
        }
        *(unsigned int *)(c + 0x5668) = a;
        return;
    }
    *(unsigned int *)(c + 0x5668) = ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6) >> 0xf;
}
