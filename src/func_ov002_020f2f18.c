typedef int s32;
typedef unsigned char u8;

void func_ov002_020f2f18(char* c, int a, int b)
{
    int i = 0;
    do {
        if (*(u8*)(c + 0x170) != 0) {
            *(s32*)(((int)c + 0x160)) -= a;
            *(s32*)(((int)c + 0x164)) -= b;
        }
        i++;
        c += 0x14;
    } while (i < 5);
}
