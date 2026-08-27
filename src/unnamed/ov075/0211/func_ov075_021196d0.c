extern void func_ov075_02115134(void* c);
extern void func_ov075_02116030(void* c, int v);
extern void func_020338b0(int a, int b, int c, int d);
extern void func_02012790(int a);
extern int func_0203da9c(void);
extern int func_ov075_02115098(void* c, int arg1);
extern unsigned char data_0209d454;
extern int data_0209b2e8;
extern char data_ov075_0211d71c[];

void func_ov075_021196d0(char* c)
{
    func_ov075_02115134(*(void**)(c + 0x50));
    data_0209d454 &= ~5;
    func_ov075_02116030(data_ov075_0211d71c, data_0209b2e8);
    data_0209b2e8 = 0;
    func_020338b0(0x225, 0xb0, 0x1d, -1);
    data_0209d454 |= 5;
    *(unsigned char*)(c + 0x280) = 0;
    *(int*)(c + 0x26c) = 0;
    *(int*)(c + 0x270) = 0x78;
    func_02012790(0x11e);
    *(unsigned char*)(c + 0x282) = func_ov075_02115098(*(void**)(c + 0x50), func_0203da9c());
}
