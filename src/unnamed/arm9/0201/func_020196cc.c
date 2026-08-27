extern int func_0205f8e0(int *a, int *b);
extern void func_0203b9b4(int *p, int v);
extern void SetSoundMode(int mode);
extern unsigned char data_0208ee3c[];
extern int data_0209d4b8[];
extern unsigned char data_0209d4b4[];

void func_020196cc(void)
{
    int a, b;
    if (func_0205f8e0(&a, &b) == 0 && a == 0) {
        if (b == 0) data_0208ee3c[0] = 0;
    }
    func_0203b9b4(data_0209d4b8, 0x4d2);
    SetSoundMode(data_0209d4b4[0]);
}
