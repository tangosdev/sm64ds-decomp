//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;

extern "C" {
extern u8 CONNECTION_ERROR;
extern u8 data_0209fc94;
extern u8 data_0209fc78;
void func_02034504(void);
int func_02054d88(void);
void MultiStore_Int(int val, int *dst, int len);
char* func_02054fb0(void);
void MultiStore16(u16 val, char *dst, int nbytes);
}
namespace G2S { char* GetBG0CharPtr(); char* GetBG0ScrPtr(); }

extern "C" void func_02033390(int sub)
{
    CONNECTION_ERROR = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;

    if (sub != 0) {
        volatile u16 fill;
        volatile s32 v;
        int *dst = (int*)(func_02054d88() + 0x4000);
        v = 0;
        MultiStore_Int(v, dst, 0x2000);
        MultiStore16(fill = 0x2ff, func_02054fb0(), 0x800);
    } else {
        volatile u16 fill;
        volatile s32 v;
        int *dst = (int*)((int)G2S::GetBG0CharPtr() + 0x4000);
        v = 0;
        MultiStore_Int(v, dst, 0x2000);
        MultiStore16(fill = 0x2ff, G2S::GetBG0ScrPtr(), 0x800);
    }
}
