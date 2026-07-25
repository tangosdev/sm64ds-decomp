typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;

extern int func_ov002_020cf700(void* p);
extern int func_ov002_020e0478(void* p);
extern int AngleDiff(int a, int b);
extern int _ZN6Player7IsStateERNS_5StateE(void* thiz, void* st);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* thiz, void* st);
extern int func_ov002_020c19d0(void* self, int a, int b);
extern char data_ov002_02110694;
extern char data_ov002_021103ac;
extern char data_ov002_021101b4;

int func_ov002_020c1c84(char* self, int arg)
{
    int a;
    int p;

    if ((u16)(*(u16*)(self + 0x6ce) & 0x200) ||
        !*(u8*)(self + 0x6de) ||
        func_ov002_020cf700(self) ||
        *(u8*)(self + 0x703) ||
        func_ov002_020e0478(self) ||
        *(s32*)(self + 0xd0) ||
        *(u8*)(self + 0x706))
        return 0;

    if (AngleDiff(arg, *(s16*)(self + 0x94)) < 0x6000)
        return 0;

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110694) ||
        *(u8*)(self + 0x708) ||
        *(u8*)(self + 0x709))
        return 0;

    if (*(s32*)(self + 0x98) < 0x3000)
        return 0;

    if (*(s32*)(self + 0xa8) > 0)
        return 0;

    a = arg + 0x8000;
    p = *(s16*)(self + 0x94);
    *(s16*)(self + 0x69c) = (s16)(p - (s16)(p - a) * 2);
    *(s16*)(self + 0x8e) = (s16)a;
    *(s16*)(self + 0x94) = *(s16*)(self + 0x8e);

    if (*(s32*)(self + 8) == 0) {
        if (func_ov002_020c19d0(self, 0x3c, 0x78))
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021103ac);
    } else {
        if (func_ov002_020c19d0(self, 0x64, 0x96)) {
            *(s32*)(self + 0x98) = -0x12000;
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021101b4);
        } else {
            *(s32*)(self + 0x98) = 0;
        }
    }
    return 1;
}
