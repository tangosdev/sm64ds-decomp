extern void func_ov096_02135800(void *);
int func_ov096_0213640c(char *c)
{
    func_ov096_02135800(c);
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0x38c) = 3;
    return 1;
}
