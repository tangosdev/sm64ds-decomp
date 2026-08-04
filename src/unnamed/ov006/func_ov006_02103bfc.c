extern void func_ov004_020adb1c(int self);
extern void func_ov006_02102564(char (*c)[64]);
extern void func_ov006_02100058(unsigned char (*c)[0x20]);

#pragma opt_strength_reduction off
void func_ov006_02103bfc(char *c)
{
    int i;
    int j;
    char *p;
    *(int *)(c + 0x5660) = 0;
    for (i = 0; i < 0x30; i++) {
        char *r = c + (i << 6);
        *(int *)(r + 0x4660) = 0;
        *(int *)(r + 0x4664) = 0;
        *(int *)(r + 0x4678) = 0;
        *(int *)(r + 0x467c) = 0;
        *(int *)(r + 0x4680) = 0;
        *(int *)(r + 0x4684) = 0;
        *(unsigned short *)(r + 0x4694) = 0;
        *(unsigned short *)(r + 0x4696) = 0;
        *(unsigned char *)(r + 0x4698) = 0;
        *(unsigned char *)(r + 0x4699) = 0;
        *(unsigned char *)(r + 0x469a) = 0;
    }
    p = c;
    for (j = 0; j < 0x10; j++) {
        *(unsigned char *)(p + 0x5336) = 0;
        *(unsigned char *)(p + 0x5334) = 0;
        p += 0x18;
    }
    *(unsigned short *)(c + 0x566e) = 0;
    *(unsigned short *)(c + 0x5672) = 0;
    *(unsigned char *)(c + 0x5677) = 0;
    *(unsigned char *)(c + 0x5678) = 0;
    *(unsigned char *)(c + 0x5679) = 0;
    *(unsigned char *)(c + 0x567a) = 0;
    *(unsigned short *)(c + 0x5670) = 0;
    func_ov004_020adb1c(0);
    func_ov006_02102564((char (*)[64])c);
    func_ov006_02100058((unsigned char (*)[0x20])c);
}
