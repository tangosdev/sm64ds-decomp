extern void func_0201a4d0(void);
extern int func_0205f650(void);
extern int func_0205f66c(int a);
extern void func_02011db4(void);

extern unsigned char data_0209d4f4;
extern unsigned char data_0209d4ec;

void func_02019a58(void)
{
    goto enter;
    do {
        func_0201a4d0();
    enter:
        if (func_0205f650() == 1) break;
    } while (func_0205f66c(1) != 1);

    if (data_0209d4f4 != 0) {
        func_02011db4();
        data_0209d4f4 = 0;
        func_0201a4d0();
    }
    data_0209d4ec = 7;
}
