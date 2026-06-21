//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned, int, int, unsigned);
void func_ov002_020cd39c(char *r0) {
    char *r4 = r0;
    _ZN6Player7SetAnimEji5Fix12IiEj(r0, 0xaa, 0x40000000, 0x1000, 0);
    *(unsigned char*)(r4 + 0x6e3) = 4;
    *(unsigned short*)(r4 + 0x600 + 0xa4) = 7;
}
}
