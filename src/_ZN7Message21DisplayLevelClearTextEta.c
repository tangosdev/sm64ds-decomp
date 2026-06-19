extern unsigned short data_0209d6d4;
extern unsigned char data_0209d668;
extern int func_0201d850(signed char a);
extern int func_0201e220(int);

void _ZN7Message21DisplayLevelClearTextEta(unsigned short a, signed char b)
{
    func_0201d850(b);
    data_0209d6d4 = a;
    data_0209d668 = 1;
    func_0201e220(1);
}
