typedef short s16;

extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern void func_0201f32c(int a);
extern void func_ov002_020c3f2c(void* player);
extern int func_ov085_0212e728(void* c, void* p);
extern unsigned char data_0209d66c;
extern int data_ov085_021307f0[];

int func_ov085_0212dbdc(char* c)
{
    char* p = (char*)_ZN5Actor13ClosestPlayerEv(c);
    int* pp;
    if (p == 0)
        return 1;
    pp = (int*)(((int)c + 0x2c8) & 0xFFFFFFFFFFFFFFFF);
    *pp = *pp + 1;
    switch (*(int*)(c + 0x2c8)) {
    case 1:
        func_0201f32c(2);
        break;
    case 0x14:
        func_ov002_020c3f2c(p);
        break;
    case 0x28:
        data_0209d66c = 1;
        break;
    case 0x8c:
        func_0201f32c(3);
        break;
    case 0xee:
        data_0209d66c = 1;
        break;
    case 0xf0:
        {
            s16 a = *(s16*)(c + 0x2d4);
            s16 b;
            *(s16*)(p + 0x8c) = 0;
            *(s16*)(p + 0x8e) = a;
            *(s16*)(p + 0x90) = 0;
            b = *(s16*)(c + 0x2d4);
            *(s16*)(p + 0x92) = 0;
            *(s16*)(p + 0x94) = b;
            *(s16*)(p + 0x96) = 0;
            func_ov085_0212e728(c, data_ov085_021307f0);
        }
        break;
    }
    return 1;
}
