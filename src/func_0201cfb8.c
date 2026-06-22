typedef unsigned char u8;
typedef unsigned short u16;

extern u16 data_0209d6d4;
extern u8 data_0209d660;
extern u8 data_0209d668;
extern u8 data_0209d6a8;

extern void func_0201eaac(void);
extern void func_0201d418(int a, int b);

void func_0201cfb8(int arg0)
{
    data_0209d6d4 = (u16)arg0;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d668 = 1;
    data_0209d6a8 = 0;
    func_0201d418(0xcf, 0x68);
}
