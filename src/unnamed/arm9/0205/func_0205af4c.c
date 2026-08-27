extern void *data_020a6484;
int func_0205af4c(void)
{
    int n = 0;
    void *p = data_020a6484;
    while (p) { p = *(void **)p; n++; }
    return n;
}
