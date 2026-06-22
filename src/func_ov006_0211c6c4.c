extern void func_ov004_020b1e34(void* c, int a, int b, int d);
extern void func_ov006_02119c74(void* c);
extern void func_ov006_02119bdc(void* c);
extern void func_ov006_02119bc4(void* c);
extern void func_ov006_021199c0(void* c);
extern void func_ov006_02119aa8(void* c);
extern void func_ov006_020c29dc(void* c);
int func_ov006_0211c6c4(char* c) {
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);
    func_ov006_02119c74(c);
    func_ov006_02119bdc(c);
    func_ov006_02119bc4(c);
    func_ov006_021199c0(c);
    func_ov006_02119aa8(c);
    func_ov006_020c29dc(c + 0x4f38);
    return 1;
}
