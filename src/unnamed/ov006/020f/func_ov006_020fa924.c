extern int data_ov006_0212eb44[];

void func_ov006_020fa924(char *p)
{
    int i;
    char *s = p;
    for (i = 0; i < 3; i++, p += 0x18)
    {
        if (*(unsigned char *)(p + 0x4e6c) == 0)
        {
            *(unsigned char *)(p + 0x4e6c) = 1;
            *(unsigned char *)(p + 0x4e6d) = 1;
            *(int *)(p + 0x4e58) = (data_ov006_0212eb44[i] << 12) - *(int *)(s + 0x5bfc);
            *(int *)(p + 0x4e5c) = -*(int *)(s + 0x5c00);
            *(short *)(p + 0x4e6a) = 0;
            *(short *)(p + 0x4e68) = 0;
            *(unsigned char *)(p + 0x4e6e) = 0;
        }
    }
}
