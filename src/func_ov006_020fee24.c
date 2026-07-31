typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

extern void FreeGfxSlotsById(int arg);
extern void func_ov006_020feba8(void *c);
extern void func_ov006_020fc844(u8 *c);
extern void func_ov006_020fae90(u8 *c);
extern void func_ov006_020fb7e0(char *thiz);
extern void func_ov006_020fdd40(void *c);
extern void func_ov006_020fe2bc(char *c);
extern void func_ov006_020fea70(char *o);
extern void func_ov006_020fe248(char *c);
extern void func_ov006_020fda7c(char *c);
extern void func_ov006_020fb670(char *obj);
extern void func_ov006_020fb60c(char *c);
extern void func_ov006_020fc7d0(char *c);
extern void func_ov006_020fbad4(char *c);
extern void func_ov006_020fb97c(char *c);
extern void func_ov004_020b0a54(void *c);
extern void func_ov006_020fad34(char *c);

s32 func_ov006_020fee24(char *c)
{
    switch (*(s32 *)(c + 0x5c10)) {
    case 0:
        FreeGfxSlotsById(0x1d);
        func_ov006_020feba8(c);
        func_ov006_020fc844((u8 *)c);
        func_ov006_020fae90((u8 *)c);
        *(s32 *)(c + 0x5c10) = 1;
        *(u16 *)(c + 0x5c1c) = 0x10;
        *(u16 *)(c + 0x5c24) = 0x60;
        break;
    case 1:
        if (*(u8 *)(c + 0xc4) == 0) {
            *(u8 *)(c + 0xc3) = 1;
            *(u8 *)(c + 0xc4) = 1;
            *(u16 *)(c + 0xc0) = 0;
        }
        if (*(u16 *)(c + 0x5c2a) != 0) {
            func_ov006_020fb7e0(c);
            (*(u16 *)(((long long)(int)(c + 0x5c2a)) & 0xffffffffffffffffLL))--;
        } else {
            func_ov006_020fdd40(c);
            func_ov006_020fe2bc(c);
            func_ov006_020fea70(c);
            func_ov006_020fe248(c);
            func_ov006_020fda7c(c);
            func_ov006_020fb670(c);
            func_ov006_020fb60c(c);
            func_ov006_020fc7d0(c);
            func_ov006_020fbad4(c);
            func_ov006_020fb97c(c);
            func_ov006_020fb7e0(c);
        }
        break;
    case 2:
        if (*(u16 *)(c + 0x5c18) != 0) {
            (*(u16 *)(((long long)(int)(c + 0x5c18)) & 0xffffffffffffffffLL))--;
            if (*(s16 *)(c + 0x5c18) <= 0) {
                func_ov004_020b0a54((void *)0x10);
                *(u8 *)(c + 0xc3) = 0;
            }
        }
        func_ov006_020fb7e0(c);
        func_ov006_020fda7c(c);
        func_ov006_020fb60c(c);
        func_ov006_020fc7d0(c);
        func_ov006_020fbad4(c);
        break;
    }
    func_ov006_020fad34(c);
    return 1;
}
