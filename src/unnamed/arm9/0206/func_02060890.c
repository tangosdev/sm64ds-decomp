extern void func_0206002c(void);
extern void func_02061138(void);
extern void func_02060a64(void);
extern char data_020a8180[];
extern void *data_020a8780;

void func_02060890(void)
{
    char *g = data_020a8180;
    if (*(int *)(g + 0x34) != 0)
        return;
    *(int *)(g + 0x34) = 1;
    *(int *)(g + 0x20) = 0;
    *(int *)(g + 0x1c) = *(int *)(g + 0x20);
    *(int *)(g + 0x18) = *(int *)(g + 0x1c);
    *(int *)(g + 0x24) = -1;
    *(int *)(g + 0x28) = 0;
    *(int *)(g + 0x2c) = 0;
    func_0206002c();
    data_020a8780 = (void *)func_02060a64;
    func_02061138();
}
