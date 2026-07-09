extern unsigned char data_0209d454;

void func_ov006_0210c1a8(int *o)
{
    if (*o <= 0)
        return;
    *o = *o - 1;
    if ((*o & 7) != 0)
        return;
    *((unsigned char *)(((int)o + 4) & 0xFFFFFFFFFFFFFFFF)) ^= 1;
    if (*((unsigned char *)o + 4) != 0)
        data_0209d454 |= 2;
    else
        data_0209d454 &= ~2;
}