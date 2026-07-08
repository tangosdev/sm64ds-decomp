extern unsigned char data_020a0e40[];
extern unsigned char TOUCH_INPUT_ARR[];
extern unsigned char data_020a0de9[];
extern int data_ov006_0213ca54[];

void func_ov006_020ed274(char *c)
{
    int idx;
    int b;
    int w0, w1;
    *(int *)(((int)c + 0x466c) & 0xFFFFFFFFFFFFFFFF) -= 1;
    if (*(int *)(c + 0x4000 + 0x66c) != 0)
    {
        idx = data_020a0e40[0];
        b = 0;
        if (TOUCH_INPUT_ARR[idx * 4] != 0 && data_020a0de9[idx * 4] != 0)
            b = 1;
        if (b == 0)
            return;
    }
    w0 = data_ov006_0213ca54[0];
    w1 = data_ov006_0213ca54[1];
    w0 = w1 ? w0 : w0;
    *(int *)(c + 0x4000 + 0x660) = w0;
    *(int *)(c + 0x4000 + 0x664) = w1;
}
