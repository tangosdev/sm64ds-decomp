extern unsigned char data_0209d4d4;
extern int data_0209d508;
extern int data_0209d510;
extern void func_0205f8e0(int *a, int *b);
extern int func_0205f9ac(int a, int b);

void func_02019e60(void)
{
    if (data_0209d4d4) return;
    func_0205f8e0(&data_0209d508, &data_0209d510);
    func_0205f9ac(2, 0);
    data_0209d4d4 = 1;
}
