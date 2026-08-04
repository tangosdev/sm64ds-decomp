extern void func_ov006_02120a18(char* p, int v);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int n);
struct P2 { int a, b; };
extern struct P2 data_ov006_0213fac0;

void func_ov006_02121cf4(char* c) {
    *(int*)(c + 0x5d90) = 0x1e;
    *(short*)(c + 0x5db8) = 1;
    func_ov006_02120a18(c + 0x5d84, *(short*)(c + 0x5dba));
    *(int*)(c + 0x5dac) = 0;
    _ZN5Sound12PlayBank2_2DEj(0x1b6);
    *(struct P2*)(c + 0x5004) = data_ov006_0213fac0;
}
