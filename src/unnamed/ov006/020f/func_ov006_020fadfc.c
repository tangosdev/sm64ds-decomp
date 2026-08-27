extern unsigned char data_0209d45c;
void func_ov006_020fadfc(char *p)
{
    data_0209d45c &= ~4;
    *(unsigned char *)(p + 0x5c0e) = 0;
}
