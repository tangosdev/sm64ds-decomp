extern unsigned char data_020a0e40[];
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern int func_02054d88(void);
extern void func_0205a448(unsigned short a, int b, int c);
extern int data_ov006_0213faa8[];

void func_ov006_02121778(char *c)
{
    int idx, b;
    *(int *)(((long long)(int)(c + 0x5d90)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    if (*(int *)(c + 0x5000 + 0xd90) != 0)
    {
        idx = data_020a0e40[0];
        b = 0;
        if (data_020a0de8[idx * 4] != 0 && data_020a0de9[idx * 4] != 0)
            b = 1;
        if (b == 0)
            return;
    }
    {
        int r;
        volatile unsigned short h;
        r = func_02054d88();
        h = 0;
        func_0205a448(h, r, 0x6000);
    }
    *(short *)(c + 0x5dc2) = 1;
    {
        int w0 = data_ov006_0213faa8[0];
        int w1 = data_ov006_0213faa8[1];
        w0 = w1 ? w0 : w0;
        *(int *)(c + 0x5004) = w0;
        *(int *)(c + 0x5008) = w1;
    }
}
