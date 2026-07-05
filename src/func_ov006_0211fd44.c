extern void func_ov006_0211d5a8(char *c);
extern void func_ov006_0211f6fc(char *c);
extern void func_ov006_0211e184(char *c);
extern void func_ov004_020b0a54(int x);
extern void func_ov004_020adb1c(int self);
extern int func_ov004_020adbe0(void);
extern void func_ov004_020b0cac(int a, int b, int c, int d, int e, short f);

struct B { unsigned char pad[0xb4]; int b4; int b8; };
extern struct B *func_020beb68;

void func_ov006_0211fd44(char *c)
{
    func_ov006_0211d5a8(c);
    func_ov006_0211f6fc(c);
    func_ov006_0211e184(c);
    if (*(unsigned short *)(c + 0x4c0c) == 0) return;
    *(unsigned short *)(((int)c + 0x4c0c) & 0xFFFFFFFFFFFFFFFF) -= 1;
    if (*(short *)(c + 0x4c0c) > 0) return;
    *(unsigned short *)(c + 0x4c0c) = 0;
    if (*(unsigned char *)(c + 0x4000 + 0xc1f) != 0) {
        struct B *p;
        func_ov004_020b0a54(0);
        p = func_020beb68;
        if (p != 0) {
            if (p->b4 < 0x270f) {
                *(int *)(((int)p + 0xb4) & 0xFFFFFFFFFFFFFFFF) += 1;
            }
            if (p->b4 > p->b8) {
                p->b8 = p->b4;
            }
        }
        func_ov004_020adb1c(func_020beb68 ? func_020beb68->b4 : 0);
    } else {
        if (func_ov004_020adbe0() != 0) {
            *(unsigned char *)(c + 0x4000 + 0xc1b) = 0;
        }
        func_ov004_020b0cac(8, 0x80, 0x60, 1, -1, 0xd);
        func_ov004_020b0a54(0x12);
    }
    *(unsigned char *)(c + 0xc3) = 0;
}
