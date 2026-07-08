extern unsigned short CURR_MSG_ID;
extern unsigned char data_0209d668;
extern int func_0201d850(signed char a);
extern int func_0201e220(int);

void _ZN7Message21DisplayLevelClearTextEta(unsigned short a, signed char b)
{
    func_0201d850(b);
    CURR_MSG_ID = a;
    data_0209d668 = 1;
    func_0201e220(1);
}
