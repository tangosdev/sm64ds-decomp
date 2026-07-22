extern void func_0203da2c(int v);
extern void func_02030500(void);
extern void func_02019a58(void);
extern void func_02012790(int v);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int v);
extern void func_020132d8(void);
extern void func_0201ffcc(void);
extern void func_020199a4(void);
extern unsigned char data_02092778;
extern int data_0209fc68;
extern int data_0209277c;
extern unsigned char data_0209e64c;
extern unsigned char data_0209b480;

void func_02030790(void)
{
    data_0209fc68 = 6;
    if (data_02092778 == 0)
        return;
    func_0203da2c(0);
    func_02030500();
    data_0209277c = 0;
    func_02019a58();
    if (data_0209e64c != 0 && data_0209b480 != 0) {
        func_02012790(0x123);
        _ZN5Sound22StopLoadedMusic_Layer1Ej(4);
        func_020132d8();
    }
    func_0201ffcc();
    func_020199a4();
}
