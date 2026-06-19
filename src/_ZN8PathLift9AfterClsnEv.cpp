//cpp
extern "C" {
int func_ov002_020efedc(void *c);
int DecIfAbove0_Byte(void *p);
void func_02012694(int a, void *p);
void func_ov002_020efa54(void *c, int a);
void _ZN8PathLift9AfterClsnEv(char *c) {
    if (func_ov002_020efedc(c) != 0 &&
        *(int*)(c + 0x44c) == 0 &&
        DecIfAbove0_Byte(c + 0x42b) == 0) {
        int b = *(unsigned short *)(c + 0xc) == 0x1f;
        if (b) {
            func_02012694(0x6f, c + 0x74);
        }
        func_ov002_020efa54(c, 1);
    }
    *(unsigned char *)(c + 0x42a) = 1;
}
}
