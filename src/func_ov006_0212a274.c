extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int data_ov006_0212f3b4[];

void func_ov006_0212a274(char *self, int idx)
{
    unsigned int v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) << 3 >> 0xf;
    int b = 0;
    if (v == 5)
        b = 1;
    *(unsigned char *)(self + idx * 0x24 + 0xbeaf) = data_ov006_0212f3b4[b];
    *(unsigned char *)(self + idx * 0x24 + 0xbeb0) = 0;
}
