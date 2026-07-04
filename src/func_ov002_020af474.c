typedef short s16;
typedef unsigned short u16;
extern s16 data_02082214[];

void func_ov002_020af474(char* o)
{
    int a;

    if (*(u16*)(o + 0x100) < 5) {
        *(int*)(o + 0xa8) = 0x28000;
        return;
    }

    {
        s16* p = (s16*)(((long long)(int)(o + 0x92)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p - 0x1000;
    }

    a = (int)*(u16*)(o + 0x92) >> 4;
    *(int*)(o + 0xa8) = (s16)data_02082214[a * 2 + 1] * (s16)0x1e + 0x2000;

    a = (int)*(u16*)(o + 0x92) >> 4;
    *(int*)(o + 0x98) = (s16)data_02082214[a * 2] * (s16)-0x1e;
}
