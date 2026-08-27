extern short data_ov006_0212e044;
extern short data_ov006_0212e048;
struct P2 { int a, b; };
extern struct P2 data_ov006_0213fbd8;

void func_ov006_02124228(char* c) {
    short a, b;
    *(int*)(c + 0x7b84) = 0x5a;
    a = data_ov006_0212e044;
    *(short*)(c + 0x7ba0) = a;
    b = data_ov006_0212e048;
    *(short*)(c + 0x7ba2) = b;
    *(short*)(c + 0x7b9c) = a;
    *(short*)(c + 0x7b9e) = b;
    *(int*)(c + 0x7b8c) = 0;
    *(int*)(c + 0x7b90) = 0x3e8;
    *(struct P2*)(c + 0x5004) = data_ov006_0213fbd8;
}
