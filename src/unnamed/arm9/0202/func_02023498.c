extern void func_0203bc50(void);
extern void func_0203bb14(void);
extern void ResetInput(void);

extern unsigned char data_0209f1e0;
extern unsigned char data_0209f1dc;
extern unsigned char data_0209f1d8;
extern unsigned char data_020a0e44;

void func_02023498(void)
{
    if (data_0209f1e0 != 0) {
        func_0203bc50();
        func_0203bb14();
        ResetInput();
        return;
    }
    if (data_0209f1dc != 0) {
        if (data_020a0e44 == 0)
            data_0209f1dc = 0;
        return;
    }
    if (data_0209f1d8 != 0)
        return;
    if (data_020a0e44 != 0) {
        data_0209f1e0 = 1;
        data_0209f1dc = 1;
    }
}
