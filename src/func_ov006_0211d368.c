extern unsigned short data_ov006_0212efc8[];
extern unsigned char data_ov006_0212efc4[];
extern unsigned char data_0209d45c;
extern int data_0209d4b8;
extern void SetBg3Offset(int a, int b);
extern int RandomIntInternal(int *seed);

void func_ov006_0211d368(char *obj, int i)
{
    unsigned char state = *(unsigned char *)(obj + 0x4be3 + i * 0x1c);
    if ((unsigned char)(state - 8) < 2)
    {
        (*(unsigned short *)(obj + 0x4bdc + i * 0x1c))++;
        if (*(unsigned short *)(obj + 0x4bdc + i * 0x1c) >= data_ov006_0212efc8[(unsigned char)(state - 8)])
        {
            *(unsigned short *)(obj + 0x4bdc + i * 0x1c) = 0;
            (*(unsigned char *)(obj + 0x4be3 + i * 0x1c))++;
        }
    }
    if (*(unsigned short *)(obj + 0x4bde + i * 0x1c) != 0)
    {
        (*(unsigned short *)(obj + 0x4bde + i * 0x1c))--;
        return;
    }
    data_0209d45c &= ~4;
    SetBg3Offset(0, 0x100);
    {
    int rnd = RandomIntInternal(&data_0209d4b8);
    *(unsigned char *)(obj + 0x4be1 + i * 0x1c) =
        data_ov006_0212efc4[((unsigned int)(((unsigned int)rnd >> 16) & 0x7fff) * 2) >> 15];
    }
    *(unsigned short *)(obj + 0x4bdc + i * 0x1c) = 0;
    *(unsigned char *)(obj + 0x4be3 + i * 0x1c) = 0;
    if (*(unsigned char *)(obj + 0x4be1 + i * 0x1c) == 2)
    {
        *(unsigned char *)(obj + 0x4be4 + i * 0x1c) = 0;
        *(int *)(obj + 0x4bd4 + i * 0x1c) = -0x4800;
        *(int *)(obj + 0x4bcc + i * 0x1c) = 0x180000;
    }
    else
    {
        *(unsigned char *)(obj + 0x4be4 + i * 0x1c) = 1;
        *(int *)(obj + 0x4bd4 + i * 0x1c) = 0x4800;
        *(int *)(obj + 0x4bcc + i * 0x1c) = -0x80000;
    }
    if (*(unsigned char *)(obj + 0x4be5 + i * 0x1c) != 0)
    {
        *(unsigned char *)(obj + 0x4be1 + i * 0x1c) = 5;
    }
}
