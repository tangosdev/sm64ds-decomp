#pragma opt_propagation off
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct P2 { int a, b; };

extern int _ZNK6Player14GetBodyModelIDEjb(void* self, u32 a, int b);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, int b, u32 d);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* ma, void* file, int a, int b, u32 d);

extern struct P2 data_ov002_0210a34c;
extern struct P2 data_ov002_0210a084;
extern struct P2 data_ov002_0210eb88;
extern u8 data_ov002_020ff0f8[];

void func_ov002_020d71ec(char* p, int arg1)
{
    int arr[4];
    char* model;
    char* sub;
    u32 shifted;
    int val;

    if (arg1 != 2) {
        model = *(char**)(p + _ZNK6Player14GetBodyModelIDEjb(p, *(u32*)(p + 8) & 0xff, 0) * 4 + 0xdc);
        sub = (char*)(((long long)(int)(model + 0x50)) & 0xFFFFFFFFFFFFFFFFLL);
        shifted = ((u32)*(int*)(sub + 8) << 4) >> 0x10;
        *(struct P2*)&arr[0] = data_ov002_0210a34c;
        *(struct P2*)&arr[2] = data_ov002_0210a084;
        _ZN6Player7SetAnimEji5Fix12IiEj(p, arr[arg1], 0x40000000, 0x1000, 0);

        if (*(u8*)(p + 0x714) == 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(*(void**)(p + 0x160), *(void**)((char*)(&arr[2])[arg1] + 4), 0x40000000, 0x1000, 0);
        }

        if (arg1 == 1) {
            val = data_ov002_020ff0f8[shifted];
            model = *(char**)(p + _ZNK6Player14GetBodyModelIDEjb(p, *(u32*)(p + 8) & 0xff, 0) * 4 + 0xdc);
            sub = (char*)(((long long)(int)(model + 0x50)) & 0xFFFFFFFFFFFFFFFFLL);
            val = ((u32)val << 0x10) >> 4;
            *(int*)(sub + 8) = val;
            *(int*)(*(int*)(p + 0x160) + 0x58) = val;
        }

        *(u8*)(p + 0x6e3) = 1;
        return;
    }

    _ZN6Player7SetAnimEji5Fix12IiEj(p, 0x6c, 0x40000000, -0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(*(void**)(p + 0x160), *(void**)((char*)&data_ov002_0210eb88 + 4), 0x40000000, -0x1000, 0);
    *(u8*)(p + 0x6e3) = 2;
    *(u16*)(p + 0x6a4) = 0xa;
}
