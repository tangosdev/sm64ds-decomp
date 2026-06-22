typedef unsigned char u8;
typedef unsigned short u16;

extern u16 data_0209fce8;
extern u8 data_0209fc9c;
extern u8 data_0209fc94;
extern u8 data_0209fc78;

extern void func_02034504(void);
extern void func_02033e50(int a, int b);

void func_02033a80(int arg0)
{
    data_0209fce8 = (u16)arg0;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    func_02033e50(0xcf, 0x68);
}
