extern void *data_0208f3b4;
extern void *data_0208f3c4;
void func_020226a4(char *p)
{
    *(void **)p = &data_0208f3b4;
    *(void **)p = &data_0208f3c4;
    *(short *)(p + 4) = 0;
}
