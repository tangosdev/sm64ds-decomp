extern void func_ov004_020b51e4(char *p);
extern char data_ov004_020bc7e8[];
struct W2 { int a, b; };
void func_ov004_020b5334(char *c) {
    func_ov004_020b51e4(c);
    *(short*)(c+0x20) = 0;
    *(struct W2*)c = *(struct W2*)data_ov004_020bc7e8;
}
