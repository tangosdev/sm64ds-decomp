typedef unsigned char u8;
typedef int s32;
extern void *func_ov096_021357b4(void *c);
void func_ov096_0213670c(void *c) {
    s32 r4 = (s32)c;
    void *ret = func_ov096_021357b4(c);
    u8 r0 = *(u8*)((char*)ret + 0x3a8);
    if (r0 != 0) r0 = 0; else r0 = 0x5a;
    *(u8*)((char*)(s32)r4 + 0x3ac) = r0;
    r0 = *(u8*)((char*)(s32)r4 + 0x3ac);
    if (r0 == 0) {
        *(s32*)((char*)(s32)r4 + 0x80) = 0x1000;
        *(s32*)((char*)(s32)r4 + 0x84) = 0x1000;
        *(s32*)((char*)(s32)r4 + 0x88) = 0x1000;
    }
    *(s32*)((char*)(s32)r4 + 0x38c) = 1;
}
