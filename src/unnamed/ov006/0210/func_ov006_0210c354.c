void func_ov006_0210c354(char *p)
{
    *(int *)(p + 0x1c) = 0;
    *(int *)(p + 0x20) = 0;
    if (*(int *)(p + 0x14) >= 1)
        *(unsigned char *)(p + 0x11) = 1;
}
