extern void SetBg0Offset(int a, int b);

void func_ov006_020d634c(char *c, int i)
{
    char *p = c + i * 16;
    unsigned char st = *(unsigned char *)(p + 0x628c);
    if (st != 0) {
        if (st == 2) {
            *(short *)(p + 0x6288) = 0x20;
        } else {
            *(unsigned char *)(p + 0x628c) = 1;
        }
        return;
    }
    *(unsigned char *)(p + 0x628c) = 1;
    SetBg0Offset(0x100, 0);
}
