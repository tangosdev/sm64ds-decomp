typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

extern void func_ov006_02100084(void *c);
extern void func_ov006_021024e0(void *c);
extern void func_ov006_020fffec(void *c);
extern void func_ov006_0210265c(void *c);
extern void func_ov006_02102ef4(void *c);
extern void func_ov006_02103ac0(void *c);
extern void func_ov006_02102e8c(void *c);
extern void func_ov006_0210246c(void *c);
extern void func_ov006_020fff84(void *c);
extern void func_ov006_02100380(void *c);
extern void func_ov006_0210258c(void *c);
extern void func_ov006_02100554(void *c);
extern void func_ov006_021001ac(void *c);
extern void func_ov004_020b0a54(void *c);

s32 func_ov006_02103d78(char *c)
{
    switch (*(s32 *)(c + 0x5660)) {
    case 0:
        func_ov006_02100084(c);
        func_ov006_021024e0(c);
        func_ov006_020fffec(c);
        *(u16 *)(c + 0x566c) = 0x10;
        *(s32 *)(c + 0x5660) = 1;
        break;
    case 1:
        if (*(u8 *)(c + 0xc4) == 0) {
            *(u8 *)(c + 0xc3) = 1;
            *(u8 *)(c + 0xc4) = 1;
            *(u16 *)(c + 0xc0) = 0;
        }
        func_ov006_0210265c(c);
        func_ov006_02102ef4(c);
        func_ov006_02103ac0(c);
        func_ov006_02102e8c(c);
        func_ov006_0210246c(c);
        func_ov006_020fff84(c);
        func_ov006_02100380(c);
        func_ov006_0210258c(c);
        break;
    case 2:
        func_ov006_02102e8c(c);
        func_ov006_0210246c(c);
        func_ov006_020fff84(c);
        func_ov006_02100380(c);
        func_ov006_02100554(c);
        func_ov006_021001ac(c);
        break;
    case 3:
        func_ov006_02102e8c(c);
        func_ov006_0210246c(c);
        func_ov006_020fff84(c);
        func_ov006_021001ac(c);
        if (*(u16 *)(c + 0x566e) != 0) {
            *(u16 *)(u32)(unsigned long long)(u32)(c + 0x566e) -= 1;
            if (*(s16 *)(c + 0x566e) <= 0) {
                func_ov004_020b0a54((void *)0x10);
                *(u8 *)(c + 0xc3) = 0;
            }
        }
        break;
    }
    return 1;
}
