int func_02042748(unsigned short *a, unsigned short *b)
{
    int i;
    for (i = 0; i < 3; i++) {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}
