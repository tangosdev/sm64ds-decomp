extern void func_ov004_020b0cac(int a, int b, int c, int d, int e, int f);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern int data_ov004_020bc878;
extern int data_ov004_020bc890;
struct Pair2 { int a; int b; };
extern struct Pair2 data_ov004_020bc8f4;

void func_ov004_020b7b90(char* c) {
    *(int*)(c + 0x1c) = 0x78;
    func_ov004_020b0cac(0xa, data_ov004_020bc878, data_ov004_020bc890, -1, -1, 0xd);
    _ZN5Sound12PlayBank2_2DEj(0x137);
    *(struct Pair2*)(c + 8) = data_ov004_020bc8f4;
}
