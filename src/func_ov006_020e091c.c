typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

void func_ov006_020e091c(char *base, int i)
{
    int n = i * 0x24;
    char *a4794 = base + 0x4794;
    char *a478c = base + 0x478c;
    char *a4790 = base + 0x4790;
    *(int*)(a478c + n) = *(int*)(a478c + n) + *(int*)(a4794 + n);
    *(int*)(a4790 + n) = *(int*)(a4790 + n) + *(int*)(base + n + 0x4798);
    if (*(u16*)(base + n + 0x47a0) != 0) {
        char *a47a2 = base + 0x47a2;
        *(u16*)(a47a2 + n) = *(u16*)(a47a2 + n) - 1;
        if (*(s16*)(a47a2 + n) < 0) *(s16*)(a47a2 + n) = 0;
        return;
    }
    if (*(int*)(a4794 + n) > 0) {
        *(int*)(a4794 + n) = *(int*)(a4794 + n) - 8;
        if ((s16)*(int*)(a4794 + n) < 0) *(int*)(a4794 + n) = 0;
        return;
    }
    if (*(int*)(a4794 + n) < 0) {
        *(int*)(a4794 + n) = *(int*)(a4794 + n) + 8;
        if (*(int*)(a4794 + n) > 0) *(int*)(a4794 + n) = 0;
        return;
    }
    *(u8*)(base + n + 0x47aa) = 0;
}
