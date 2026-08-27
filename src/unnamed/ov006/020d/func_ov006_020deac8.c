int func_ov006_020deac8(char *p)
{
    int n = 0;
    do {
        if (*(unsigned char *)(p + 0x15) == 0)
            break;
        n++;
        p += 0x18;
    } while (n < 0x20);
    return n;
}
