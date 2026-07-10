
typedef unsigned char u8;
typedef short s16;
typedef int s32;
extern int func_0201267c(int a, void *b);
void func_ov025_021119f4(char *c)
{
    *(u8*)(((int)c + 0x39e) & 0xFFFFFFFFFFFFFFFFLL) =
        *(u8*)(((int)c + 0x39e) & 0xFFFFFFFFFFFFFFFFLL) - 1;
    if (*(u8 *)(c + 0x39e) != 0) return;
    if (*(u8 *)(c + 0x39f) != 4) {
        *(s32 *)(c + 0x398) = 5;
        *(s32 *)(c + 0xa8) = 0x3c000;
        func_0201267c(0xf4, c + 0x74);
        return;
    }
    *(s32 *)(c + 0x398) = 7;
    *(s16 *)(c + 0x39c) = (s16)(*(s16 *)(c + 0x8e) + 0x8000);
}
