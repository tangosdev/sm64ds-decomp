typedef unsigned char u8;
typedef unsigned short u16;

extern u16 data_ov006_0212ee28[];

void func_ov006_02119b00(char *o)
{
    if (*(u8 *)(o + 0x55f4) == 0)
        return;
    if (*(u8 *)(o + 0x55f6) >= 2)
        return;
    {
        u16 *p = (u16 *)(((int)o + 0x55f0) & 0xFFFFFFFFFFFFFFFF);
        *p = *p + 1;
    }
    if (*(u16 *)(o + 0x55f0) < data_ov006_0212ee28[*(u8 *)(o + 0x55f6)])
        return;
    *(u16 *)(o + 0x55f0) = 0;
    {
        u8 *q = (u8 *)(((int)o + 0x55f6) & 0xFFFFFFFFFFFFFFFF);
        *q = *q + 1;
    }
}
