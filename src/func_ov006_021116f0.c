typedef int s32;
typedef unsigned char u8;

extern void func_ov006_02114ec0(void *self);

void func_ov006_021116f0(void *arg0)
{
    char *c = (char *)arg0;

    *(s32 *)(c + 0x10) = *(s32 *)(c + 8);
    *(s32 *)(c + 0x14) = *(s32 *)(c + 0xc);
    if (*(s32 *)(c + 0x34) <= 0)
        return;
    *(s32 *)(((int)c + 0x34)) =
        *(s32 *)(((int)c + 0x34)) - 1;
    if (*(s32 *)(c + 0x34) > 0)
        return;
    func_ov006_02114ec0(*(void **)(c + 4));
    *(u8 *)(c + 0x32) = 1;
}
