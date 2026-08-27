//cpp
extern int ApproachLinear(int &, int, int);
extern "C" void func_ov004_020b4080(char *c) {
    if (ApproachLinear(*(int*)(c + 0x24), *(int*)(c + 0x28), 0x1000)) {
        *(int*)(c + 0x24) = 98304;
    }
}
