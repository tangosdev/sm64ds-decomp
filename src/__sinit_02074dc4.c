extern int data_0209d4b8;
extern int data_0209d4bc;
extern int TIME_TIMER;
extern void func_0201973c(void);

extern void func_0203b9b4(int *p, int v);
extern void func_020731dc(int a, int b, void **node);
extern int func_0201964c(int *__this);

void __sinit_02074dc4(void)
{
    func_0203b9b4(&data_0209d4b8, 1);
    func_020731dc((int)&data_0209d4b8, (int)&func_0201973c, (void**)&data_0209d4bc);
    func_0201964c(&TIME_TIMER);
}
