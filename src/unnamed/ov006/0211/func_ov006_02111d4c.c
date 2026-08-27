void func_ov006_02111d4c(char *p)
{
    *(int *)(p + 0x108) = *(int *)(p + 8);
    *(int *)(p + 0x10c) = *(int *)(p + 0xc);
    *(unsigned char *)(p + 0x110) = 1;
    *(unsigned char *)(p + 0x3b) = 1;
}
