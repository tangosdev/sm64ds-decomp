extern int ApproachLinear2(int* a, int b, int c);
extern void func_ov006_020cf124(char* c);

void func_ov006_020cf790(char* c) {
    if (ApproachLinear2((int*)(c + 0x329), 0, 1) != 0) {
        *(unsigned char*)(c + 0x328) = 0;
        return;
    }
    *(int*)(((int)c + 0x2C)) += 0x80;
    *(int*)(((int)c + 0x30)) += 0x80;
    *(int*)(((int)c + 0x34)) += 0x80;
    func_ov006_020cf124(c);
}
