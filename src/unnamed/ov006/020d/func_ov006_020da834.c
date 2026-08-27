int func_ov006_020da834(char *p)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (*(int *)(p + 0x24) < 0x4000)
            return 0;
        p += 0x30;
    }
    return 1;
}
