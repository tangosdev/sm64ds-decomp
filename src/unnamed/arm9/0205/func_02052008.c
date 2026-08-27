extern int func_02050d2c(void);
extern unsigned char *func_02050ae8(unsigned int id);
extern void _ZN5Sound6Player19SetPlayableSeqCountEii(int a, int b);
extern int func_0204fa3c(int a, int b, int c);

int func_02052008(int arg)
{
    int i;
    func_02050d2c();
    for (i = 0; i < 0x20; i++) {
        unsigned char *p = func_02050ae8(i);
        if (p == 0)
            continue;
        _ZN5Sound6Player19SetPlayableSeqCountEii(i, p[0]);
        if (*(int *)(p + 4) == 0)
            continue;
        if (arg == 0)
            continue;
        {
            int j;
            for (j = 0; j < p[0]; j++) {
                if (func_0204fa3c(i, arg, *(int *)(p + 4)) == 0)
                    return 0;
            }
        }
    }
    return 1;
}
