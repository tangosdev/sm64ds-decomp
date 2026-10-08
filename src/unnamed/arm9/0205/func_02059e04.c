extern void Crash(void);
extern char data_020a645c[];

void func_02059e04(int a, unsigned int v)
{
    if (((v & 0x7f00) << 8) >> 0x10 == 0x10)
    {
        *(short *)data_020a645c = 1;
        return;
    }
    Crash();
}
