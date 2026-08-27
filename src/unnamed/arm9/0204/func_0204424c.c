extern short func_02044284(void *c, int x);
int func_0204424c(char *c)
{
    if (*(short *)(c + 4) == 1)
        *(short *)(c + 4) = func_02044284(c, 0);
    if (*(short *)(c + 4) == 1)
        return 0;
    return 1;
}
