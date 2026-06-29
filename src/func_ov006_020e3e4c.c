typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

void func_ov006_020e3e4c(char *base, int i)
{
    int n = i * 0x24;
    char *a48c8 = base + 0x48c8;
    char *a48c0 = base + 0x48c0;
    char *a48c4 = base + 0x48c4;
    *(int*)(a48c0 + n) = *(int*)(a48c0 + n) + *(int*)(a48c8 + n);
    *(int*)(a48c4 + n) = *(int*)(a48c4 + n) + *(int*)(base + n + 0x48cc);
    if (*(u16*)(base + n + 0x48d4) != 0) {
        char *a48d6 = base + 0x48d6;
        *(u16*)(a48d6 + n) = *(u16*)(a48d6 + n) - 1;
        if (*(s16*)(a48d6 + n) < 0) *(s16*)(a48d6 + n) = 0;
        return;
    }
    if (*(int*)(a48c8 + n) > 0) {
        *(int*)(a48c8 + n) = *(int*)(a48c8 + n) - 8;
        if ((s16)*(int*)(a48c8 + n) < 0) *(int*)(a48c8 + n) = 0;
        return;
    }
    if (*(int*)(a48c8 + n) < 0) {
        *(int*)(a48c8 + n) = *(int*)(a48c8 + n) + 8;
        if (*(int*)(a48c8 + n) > 0) *(int*)(a48c8 + n) = 0;
        return;
    }
    *(u8*)(base + n + 0x48de) = 0;
}
