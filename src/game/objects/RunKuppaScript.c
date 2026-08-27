extern void *data_0209fc48;
extern unsigned char data_0209f2d8;
extern unsigned short data_0209b274;
extern unsigned int data_0209b454;
extern void ProcessKuppaScript(void);

int RunKuppaScript(void *arg0)
{
    int cond;
    unsigned char flag;
    void **p = &data_0209fc48;

    if (*p != 0) return 0;

    flag = data_0209f2d8;
    data_0209b274 = 0;
    cond = 0;
    if (flag == 2) cond = 1;
    *p = arg0;
    if (cond == 0) {
        data_0209b454 |= 0x20000000;
    }
    ProcessKuppaScript();
    return 1;
}
