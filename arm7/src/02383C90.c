int fn_02383C90(int a1)
{
    unsigned char *p = (unsigned char *)a1 + 0x4000000;
    return (p[0x508] & 0x80) != 0;
}
