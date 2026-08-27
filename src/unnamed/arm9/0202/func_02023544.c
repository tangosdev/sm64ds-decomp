extern unsigned char data_0209f1e0;
extern unsigned char data_0209b2fc;
extern unsigned char data_0209b300;
extern void func_0202fd2c(void);
extern void func_0202ed48(void);
extern void func_02011c8c(void);
extern int GetSoundMode(void);
extern void SetSoundMode(int mode);

void func_02023544(void)
{
    data_0209f1e0 = 0;
    func_0202fd2c();
    data_0209b2fc = 0;
    data_0209b300 = 0;
    func_0202ed48();
    func_02011c8c();
    SetSoundMode(GetSoundMode());
}
