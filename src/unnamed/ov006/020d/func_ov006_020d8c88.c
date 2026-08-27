int func_ov006_020d8c88(char *c)
{
    int i;
    unsigned char (*arr)[0x40];
    i = 0;
    arr = (unsigned char (*)[0x40])c;
    do {
        if (arr[i][0x4698] == 1 && arr[i][0x4697] == 6)
            return 1;
        i++;
    } while (i < 0x70);
    return 0;
}
