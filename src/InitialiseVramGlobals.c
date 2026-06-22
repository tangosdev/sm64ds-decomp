extern int func_02053e34(void);
extern int func_02053e1c(void);
extern int data_020a4bdc;
extern int data_020a4be4;
extern int data_020a4bc8;
extern int data_020a4be0;
extern int data_020a4be8;
extern int data_020a4bd8;
extern int data_020a4bcc;

void InitialiseVramGlobals(void)
{
    int r = func_02053e34();
    data_020a4be4 = 0;
    data_020a4bc8 = 0;
    data_020a4be0 = 0x20000;
    data_020a4be8 = 0x20000;
    data_020a4bdc = r;
    data_020a4bd8 = func_02053e1c();
    data_020a4bcc = 0;
}
