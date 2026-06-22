extern char data_020a1fc0[];
extern void func_02041e14(void *p);

void func_02042160(void)
{
    char *g = data_020a1fc0;
    if (*(int *)(g + 0xc) == 0)
        return;
    if (*(int *)g == 0)
        return;
    {
        char *sub = g + 0x2000;
        *(int *)(sub + 0xb54) = 0;
        func_02041e14(sub);
    }
}
