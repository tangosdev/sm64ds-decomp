typedef short s16;
typedef unsigned short u16;

extern s16 data_02082214[];

void func_ov080_0212758c(char *a0, char *a1)
{
    int t;
    int half;
    char *p;

    t = (*(volatile u16*)(a1 + 0xc) == 0xbf);
    if (t != 0) {
        a1 = (char*)(int)(((long long)(int)(a1 + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        *(int*)(a0 + 0x32c) = *(int*)a1;
        *(int*)(a0 + 0x330) = *(int*)(a1 + 4);
        *(int*)(a0 + 0x334) = *(int*)(a1 + 8);
        *(unsigned char*)(a0 + 0x342) = 1;
        *(unsigned char*)(a0 + 0x33e) = 0x78;

        half = (*(int*)(a0 + 0x98)) >> 1;

        {
            int idx = (*(u16*)(a0 + 0x8e)) >> 4;
            int sc = data_02082214[idx * 2];
            int v = *(int*)a1;
            *(int*)a1 = v + (int)(((long long)half * sc + 0x800) >> 12);
        }

        a1 = (char*)(int)(((long long)(int)(a1 + 8)) & 0xFFFFFFFFFFFFFFFFLL);
        {
            int idx = (*(u16*)(a0 + 0x8e)) >> 4;
            int sc = data_02082214[idx * 2 + 1];
            int v = *(int*)a1;
            *(int*)a1 = v + (int)(((long long)half * sc + 0x800) >> 12);
        }
    }
}
