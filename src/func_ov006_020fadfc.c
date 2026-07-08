extern unsigned char TOP_SCREEN_RELATED;
void func_ov006_020fadfc(char *p)
{
    TOP_SCREEN_RELATED &= ~4;
    *(unsigned char *)(p + 0x5c0e) = 0;
}
