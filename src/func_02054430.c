extern int func_02054c80(int arg, unsigned short *p);

int func_02054430(int arg)
{
    unsigned short *p = (unsigned short *)0x020a6088;
    *p |= arg;
    return func_02054c80(arg, p);
}
