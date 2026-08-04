extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int data_ov006_0213af28[];

struct W2 { int a, b; };

void func_ov006_020c5370(char *c)
{
    unsigned int r;
    *(short *)(c + 0xe6) = 0;
    r = (unsigned int)RandomIntInternal(&data_0209e650);
    r &= 0x7fffffff;
    *(short *)(c + 0xec) = (short)(((int)((r >> 0x13) * 0x2d) >> 0xc) + 0xf);
    *(int *)(c + 0xb4) = *(int *)(c + 0x9c);
    *(int *)(c + 0xb8) = *(int *)(c + 0xa0);
    *(int *)(c + 0xbc) = *(int *)(c + 0xa4);
    *(int *)(c + 0xa8) = *(int *)(c + 0x4);
    *(int *)(c + 0xac) = *(int *)(c + 0x8);
    *(int *)(c + 0xb0) = *(int *)(c + 0xc);
    *(struct W2 *)(c + 0x30) = *(struct W2 *)data_ov006_0213af28;
}
