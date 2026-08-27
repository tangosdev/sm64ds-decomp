extern void *data_020a648c;
int func_0205af20(void)
{
    int n = 0;
    void *p = data_020a648c;
    while (p) { p = *(void **)p; n++; }
    return n;
}
