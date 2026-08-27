extern void func_0203f650(int x);
extern int func_02062240(int a0, int a1, int a2);
extern void func_0203f6a4(int v);
extern int data_02099e20;
extern int data_020a0f90;
extern int func_0203f920(void);

void func_0203f9d4(int a0, int a1)
{
    data_020a0f90 = a1;
    if (data_02099e20 != 3) {
        func_0203f650(-10);
        return;
    }
    if (func_02062240(a0, (int)&func_0203f920, 1) != 2) {
        func_0203f650(-1);
        return;
    }
    func_0203f6a4(0);
}
