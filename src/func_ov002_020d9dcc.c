typedef int s32;
typedef short s16;
typedef long long s64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;

extern int AngleDiff(int a, int b);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);

extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern int data_ov002_02110604[];

int func_ov002_020d9dcc(char* c)
{
    char* p;

    if (*(int*)(c + 8) != 2)
        return 0;

    p = *(char**)(c + 0x358);
    if (p == 0 || (int)(((long long)(*(u16*)(p + 0xc) == 0xbf)) & 0xFFFFFFFFFFFFFFFFLL) == 0)
        return 0;

    if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) == 0 || *(u8*)(c + 0x6de) != 0) {
        *(u16*)(c + 0x6a4) = 0;
        return 0;
    }

    if (*(u16*)(c + 0x6a4) == 0) {
        *(u16*)(c + 0x6a4) = 0x3c;
        *(s16*)(c + 0x69c) = *(s16*)(c + 0x8e);
        *(u8*)(c + 0x6e5) = 0;
        return 0;
    }

    if (*(u8*)(c + 0x6e5) == 0) {
        if (*(s16*)(c + 0x8e) < *(s16*)(c + 0x69c))
            *(u8*)(c + 0x6e5) = 0xff;
        else
            *(u8*)(c + 0x6e5) = 1;
    }

    s16 diff = *(s16*)(c + 0x8e) - *(s16*)(c + 0x6d6);
    if (*(s8*)(c + 0x6e5) >= 0) {
        if (diff < 0) {
            *(u16*)(c + 0x6a4) = 0;
            return 0;
        }
        if (AngleDiff(*(s16*)(c + 0x8e), *(s16*)(c + 0x69c)) >= 0x4000) {
            *(u8*)(((int)c + 0x6e5) & 0xFFFFFFFFFFFFFFFF) += 1;
            *(s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFF) += 0x4000;
        }
    } else {
        if (diff > 0) {
            *(u16*)(c + 0x6a4) = 0;
            return 0;
        }
        if (AngleDiff(*(s16*)(c + 0x8e), *(s16*)(c + 0x69c)) >= 0x4000) {
            *(u8*)(((int)c + 0x6e5) & 0xFFFFFFFFFFFFFFFF) -= 1;
            *(s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFF) -= 0x4000;
        }
    }

    s8 v = *(s8*)(c + 0x6e5);
    if (v < 0)
        v = -v;
    if (v < 5)
        return 0;

    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110604);
    return 1;
}
