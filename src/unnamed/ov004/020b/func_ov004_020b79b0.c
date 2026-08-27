extern int data_ov004_020bc868;
extern int data_ov004_020bc858;
struct Pair { int a, b; };
extern struct Pair data_ov004_020bc9ac;
extern void func_ov004_020b0cac(int a, int b, int c, int d, int e, int f);

void func_ov004_020b79b0(char *c) {
    *(int *)(c + 0x1c) = 0x78;
    func_ov004_020b0cac(0xb, data_ov004_020bc868, data_ov004_020bc858, -1, -1, 0xd);
    *(struct Pair *)(c + 8) = data_ov004_020bc9ac;
}
