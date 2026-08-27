extern void func_ov006_020cd1e0(void *c);
struct P2 { int a, b; };
extern struct P2 data_ov006_0213fac8;
void func_ov006_021218c4(char *c) {
    func_ov006_020cd1e0(c);
    *(int*)(c + 0x5d90) = 0x5a;
    *(struct P2*)(c + 0x5004) = data_ov006_0213fac8;
}
