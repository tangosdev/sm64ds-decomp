extern int func_02043098(void *a, void *b);
int func_02042ffc(void *a, void *b)
{
    if (b == 0) return 0;
    return func_02043098(a, (char *)b + 0x14);
}
