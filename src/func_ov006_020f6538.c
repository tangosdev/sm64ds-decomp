extern void func_ov004_020b67f8(void);
extern void func_ov004_020b0a54(int);
extern void func_ov004_020adb1c(int self);
extern void func_ov006_020c0c80(void *c);
extern void func_ov006_020c0d68(char *c);

struct B {
    char pad[0xb4];
    int b4;
    int b8;
};

extern struct B *func_020beb68;

void func_ov006_020f6538(char *c)
{
    struct B *p;

    if (*(unsigned short *)(c + 0x53e4) == 0)
        goto zero;

    *(unsigned short *)(((int)c + 0x53e4) & 0xffffffffffffffffLL) =
        *(unsigned short *)(((int)c + 0x53e4) & 0xffffffffffffffffLL) - 1;
    if (*(short *)(c + 0x53e4) > 0)
        return;

    *(unsigned short *)(c + 0x53e4) = 0;

    if (*(unsigned char *)(c + 0x5405) >= *(unsigned short *)(c + 0x53ea)) {
        if (*(unsigned char *)(c + 0x5408) == 0)
            func_ov004_020b67f8();
        func_ov004_020b0a54(4);
        p = func_020beb68;
        if (p != 0) {
            if (p->b4 < 0x270f)
                *(int *)(((int)p + 0xb4) & 0xffffffffffffffffLL) += 1;
            if (p->b4 > p->b8)
                p->b8 = p->b4;
        }
        func_ov004_020adb1c(func_020beb68 != 0 ? func_020beb68->b4 : 0);
        func_ov006_020c0c80(c + 0x4f38);
        return;
    }

    func_ov004_020b0a54(5);
    p = func_020beb68;
    if (p != 0) {
        if (p->b4 > 0)
            *(int *)(((int)p + 0xb4) & 0xffffffffffffffffLL) -= 1;
    }
    func_ov006_020c0d68(c + 0x4f38);
    return;

zero:
    *(unsigned char *)(c + 0xc3) = 0;
    *(unsigned short *)(c + 0x53e2) = 0x10;
}
