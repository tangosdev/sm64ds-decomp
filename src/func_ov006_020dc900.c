typedef unsigned short u16;
typedef unsigned char u8;
typedef short s16;

void func_ov006_020dc900(char *c)
{
    int i;
    for (i = 0; i < 0x18; i++) {
        char *p = c + (int)(((long long)i) & 0xFFFFFFFFFFFFFFFFLL) * 0x10;
        if (*(u8 *)(p + 0x5020) != 0 && *(u16 *)(p + 0x501c) != 0) {
            *(u16 *)((int)(((long long)(int)(p + 0x501c)) & 0xFFFFFFFFFFFFFFFFLL)) -= 1;
            if (*(s16 *)((int)(((long long)(int)(p + 0x501c)) & 0xFFFFFFFFFFFFFFFFLL)) <= 0)
                *(u8 *)(p + 0x5021) = 1;
        }
    }
}
