extern int dat_03805990[];
int *fn_02387C98(int a1)
{
    int *i;
    for (i = dat_03805990; ; i += 10) {
        if (*i == a1) return i;
        if (*i == 0) return 0;
    }
}
