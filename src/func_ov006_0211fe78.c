typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern void func_ov006_0211d5a8(char *c);
extern void func_ov006_0211f9fc(char *c);
extern void func_ov006_0211e0c8(char *c);
extern void func_ov006_0211e7d8(char *self);
extern void func_ov006_0211dec0(void *arg);
extern void func_ov006_0211f6fc(char *c);
extern int func_ov006_0211de7c(char *c);
extern void func_ov006_0211d69c(char *obj);
extern void func_ov006_0211de54(char *p);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int x);
extern void func_ov006_0211cc2c(unsigned char *self);
extern unsigned char data_0209d454[];

void func_ov006_0211fe78(char *c)
{
    func_ov006_0211d5a8(c);
    func_ov006_0211f9fc(c);
    if (*(u16 *)(c + 0x4c0c) != 0) {
        {
            u16 *p = (u16 *)(((int)c + 0x4c0c) & 0xFFFFFFFFFFFFFFFFULL);
            *p = *p - 1;
        }
        if (*(s16 *)(c + 0x4c0c) > 0)
            return;
        if (*(u8 *)(c + 0x4c1f) != 0) {
            func_ov006_0211e0c8(c);
            *(s16 *)(c + 0x4c0e) = 0x60;
            return;
        }
        func_ov006_0211e0c8(c);
        func_ov006_0211e7d8(c);
        *(s16 *)(c + 0x4c0e) = 0x60;
        return;
    }
    func_ov006_0211dec0(c);
    func_ov006_0211f6fc(c);
    if (func_ov006_0211de7c(c) != 0)
        return;
    if (*(u16 *)(c + 0x4c0e) != 0) {
        func_ov006_0211d69c(c);
        {
            u16 *q = (u16 *)(((int)c + 0x4c0e) & 0xFFFFFFFFFFFFFFFFULL);
            *q = *q - 1;
        }
        if (*(s16 *)(c + 0x4c0e) < 0)
            *(s16 *)(c + 0x4c0e) = 0;
        return;
    }
    func_ov006_0211de54(c);
    *(int *)(c + 0x4be8) = 4;
    *(s16 *)(c + 0x4c0c) = 0x60;
    *(int *)0x4001000 = *(int *)0x4001000 & ~0xe000;
    data_0209d454[0] = data_0209d454[0] & ~1;
    _ZN5Sound12PlayBank2_2DEj(0x1f7);
    func_ov006_0211cc2c((unsigned char *)c);
    if (*(u8 *)(c + 0x4c1f) == 0)
        return;
    _ZN5Sound12PlayBank2_2DEj(0x1f2);
}
