typedef unsigned char u8;
typedef unsigned short u16;

extern void func_ov063_02119e38(void* self, int m, int a, int b);
extern int func_ov063_0211a0dc(void* self);
extern void func_02012694(int id, void* p);
extern void func_ov063_0211adfc(char* p);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, void* f, int a, int b, int e);
#define ModelAnim_SetAnim _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj
extern int RandomIntInternal(int* seed);

extern int data_ov063_0211edcc[];
extern int data_0209e650;

void func_ov063_0211873c(char* self)
{
    int a, b;
    int r;
    u8 st = *(u8*)(self + 0x5ca);
    if (st == 3) {
        a = 0x180;
        b = 0x999;
    } else if (st == 2) {
        a = 0x240;
        b = 0x1000;
    } else {
        a = 0x380;
        b = 0x1800;
    }
    if (*(u16*)(self + 0x500 + 0xc0) != 0) {
        a = 0;
        b = 0;
    }
    func_ov063_02119e38(self, -100, a, b);
    r = func_ov063_0211a0dc(self);
    if (r == -1) {
        *(u8*)(self + 0x5cc) = 2;
        return;
    }
    if (r == 1) {
        *(u8*)(self + 0x5cc) = 3;
        if (*(u8*)(self + 0x5ca) == 1)
            func_02012694(0x155, self + 0x74);
        else
            func_02012694(0x152, self + 0x74);
        return;
    }
    if (*(u16*)(self + 0x500 + 0xc4) == 0) {
        int* p = (int*)(((long long)(int)(self + 0x19c)) & 0xFFFFFFFFFFFFFFFFLL);
        *(u8*)(self + 0x5cc) = 6;
        *p |= 1;
        *(u8*)(self + 0x5c9) = 0;
        *(int*)(self + 0x98) = 0;
        func_ov063_0211adfc(self);
        *(u16*)(self + 0x500 + 0xc2) = 0x78;
        func_02012694(0x153, self + 0x74);
        return;
    }
    if (*(u16*)(self + 0x500 + 0xbe) != 0)
        return;
    *(u8*)(self + 0x5cc) = 4;
    *(int*)(self + 0xa8) = 0;
    *(int*)(self + 0x9c) = 0;
    func_02012694(0x159, self + 0x74);
    ModelAnim_SetAnim(self + 0x380, (void*)data_ov063_0211edcc[1], 0x40000000, 0x1000, 0);
    {
        unsigned int rnd = (unsigned int)RandomIntInternal(&data_0209e650);
        *(u8*)(self + 0x5d2) = (u8)((rnd >> 0x10) % 3);
    }
    *(int*)(self + 0x98) = 0;
}
