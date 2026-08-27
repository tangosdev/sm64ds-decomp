extern void func_0205ffb0(unsigned short c);
extern void func_0205ff90(unsigned short t);
extern unsigned int data_020a805c;

int func_0205da94(int a, int b)
{
    switch (b) {
    case 9:
        func_0205ffb0((unsigned short)data_020a805c);
        return 0;
    case 0xa:
        func_0205ff90((unsigned short)data_020a805c);
        return 0;
    case 1:
        return 4;
    }
    return 8;
}
