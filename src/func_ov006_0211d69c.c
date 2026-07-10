extern void SetBg3Offset(int a, int b);
extern unsigned short data_ov006_0212efdc[];

void func_ov006_0211d69c(char *obj)
{
    if (*(unsigned char *)(obj + 0x4c24) >= 8)
    {
        return;
    }
    (*(unsigned short *)(((long long)(int)(obj + 0x4c18)) & 0xFFFFFFFFFFFFFFFFLL))++;
    if (*(unsigned short *)(obj + 0x4c18) >= data_ov006_0212efdc[*(unsigned char *)(obj + 0x4c24)])
    {
        (*(unsigned char *)(((long long)(int)(obj + 0x4c24)) & 0xFFFFFFFFFFFFFFFFLL))++;
        *(unsigned short *)(obj + 0x4c18) = 0;
        if (*(unsigned char *)(obj + 0x4c24) & 1)
        {
            SetBg3Offset(0, 0);
        }
        else
        {
            SetBg3Offset(0, 0x100);
        }
    }
    if (*(unsigned char *)(obj + 0x4c24) >= 8)
    {
        SetBg3Offset(0, 0);
    }
}
