typedef unsigned int u32;
typedef int s32;
typedef short s16;
typedef unsigned char u8;

extern void _ZN6Player7SetAnimEji5Fix12IiEj(char* thiz, u32 anim, int a, int fix, u32 b);
extern int _ZNK6Player14GetBodyModelIDEjb(char* thiz, u32 a, int b);
extern void func_ov002_020d93ac(char* thiz);
extern void func_0200d89c(char* p);
extern u32 data_ov002_0210a6d4[];
extern char* data_0209f318;

int _ZN6Player12St_Hurt_InitEv(char* c)
{
    int mid;
    char* m;
    u8 old;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_0210a6d4[*(u8*)(c+0x6e3) & 7], 0x40000000, 0x1000, 0);
    mid = _ZNK6Player14GetBodyModelIDEjb(c, *(int*)(c+8) & 0xff, 0);
    m = *(char**)(c + mid*4 + 0xdc);
    m = (char*)(((unsigned int)m + 0x50) & 0xFFFFFFFFU);
    *(int*)(m + 8) = 0;
    *(u8*)(c+0x6e6) = 0;
    old = *(u8*)(c+0x6e5);
    *(u8*)(c+0x6e5) = 0;
    if ((*(u8*)(c+0x6e3) & 0xf0) == 0x10) *(u8*)(c+0x6e5) = old << 4;
    *(u8*)(c+0x70c) = 0;
    if (*(int*)(c+0x674) != 0) {
        *(u8*)(c+0x70c) = 2;
        func_ov002_020d93ac(c);
    }
    *(u8*)(c+0x708) = 1;
    *(int*)(c+0x640) = *(int*)(c+0xa8);
    func_0200d89c(data_0209f318);
    *(s16*)(c+0x600+0xa8) = 5;
    return 1;
}
