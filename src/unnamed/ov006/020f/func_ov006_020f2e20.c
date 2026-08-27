int RandomIntInternal(int* seed);

extern int data_0209d4b8;

void func_ov006_020f2e20(char* c)
{
    int v;
    *(unsigned short*)(c + 0x5176) = *(unsigned short*)(c + 0x5174);
    v = *(int*)(c + 0xbc);
    if (v >= 0x14) {
        int r4 = (int)(((((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xa) >> 0xf);
        if (r4 + 0xa == *(unsigned short*)(c + 0x5176)) {
            int add = (int)((((((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9) >> 0xf)) + 1;
            r4 += add;
            if (r4 >= 0xa) r4 -= 0xa;
        }
        v = r4 + 0xa;
    }
    *(unsigned short*)(c + 0x5174) = v;
}
