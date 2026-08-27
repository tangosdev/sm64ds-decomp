int func_0205ff20(int a, int b, int c);
void func_0205f8b0(int param);

struct D020a8114 { int w0; int w1; int w2; };
extern struct D020a8114 data_020a8114;

int func_0205fbd0(int a, int b, int c) {
    if (!func_0205ff20(a, b, c)) return 1;
    data_020a8114.w1 = b;
    data_020a8114.w2 = c;
    func_0205f8b0((int)(((unsigned int)a >> 0x10) & 0xff) | 0x2006300);
    func_0205f8b0((a & 0xffff) | 0x1010000);
    return 0;
}
