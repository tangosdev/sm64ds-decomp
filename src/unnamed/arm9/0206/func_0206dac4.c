extern int func_020589d4(void *p);
extern void func_02058b08(void *p);
extern void func_02058a94(void *p);
extern void func_0206e06c(int x);
extern void func_02071784(void);

extern int data_020aa020;
extern int data_020a6134[];
extern int data_020a9fd8;
extern int data_020a9ffc;
extern volatile int data_020a9ed0;
extern void (*volatile data_020a9ed8[])(void);
extern void (*data_020a9ec8)(void);

void func_0206dac4(void)
{
    if (func_020589d4(&data_020aa020) == 0) {
        data_020a9fd8 = *(int *)((char *)data_020a6134[2] + 0x6c);
        data_020a9ffc = 1;
    } else if (data_020a9fd8 == *(int *)((char *)data_020a6134[2] + 0x6c)) {
        data_020a9ffc++;
    } else {
        func_02058b08(&data_020aa020);
        data_020a9fd8 = *(int *)((char *)data_020a6134[2] + 0x6c);
        data_020a9ffc = 1;
    }

    while (data_020a9ed0 > 0) {
        int n = data_020a9ed0 - 1;
        data_020a9ed0 = n;
        data_020a9ed8[n]();
    }

    data_020a9ffc--;
    if (data_020a9ffc == 0) {
        func_02058a94(&data_020aa020);
    }
    if (data_020a9ec8 != 0) {
        data_020a9ec8();
        data_020a9ec8 = 0;
    }
    func_0206e06c(0);
    func_02071784();
}
