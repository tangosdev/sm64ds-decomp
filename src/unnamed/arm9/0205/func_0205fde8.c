extern unsigned short data_020a8104;
extern int data_020a8114[2];
extern short data_020a8138;
extern char data_020a8124;
extern int data_020a8110;
extern void func_0205fcfc(void);
extern void func_0205b858(void);
extern int func_0205ba3c(int bit, int word);
extern void func_0205ba64(int a, void *b);
extern void func_02058b9c(char *p);

#pragma opt_strength_reduction off
void func_0205fde8(void)
{
    int i;
    if (data_020a8104 != 0) return;
    data_020a8104 = 1;
    data_020a8114[0] = 0;
    data_020a8114[1] = 0;
    func_0205b858();
    while (func_0205ba3c(8, 1) == 0) ;
    func_0205ba64(8, (void *)func_0205fcfc);
    for (i = 0; i < 4; i++)
        *(short *)((char *)&data_020a8138 + (i << 3)) = 0;
    func_02058b9c(&data_020a8124);
    data_020a8110 = *(int *)0x27ffc3c;
}
