//cpp
extern int ApproachLinear(int&, int, int);
extern "C" {
extern int func_ov004_020b04c0(void);
extern void func_ov004_020b4b84(char* c, int* in);
}
extern char data_ov004_020bfa34;
extern int data_ov004_020bf9f8;
extern int data_ov004_020bfa04;
extern void* data_ov004_020bfa20;
extern "C" {
extern void func_ov004_020b53f0(void);
}
inline short load_s(short* p) { return *p; }
struct EI { char* e; int i; };

extern "C" void func_ov004_020b556c(void)
{
    int step, idx, n, limit;
    int cx, rx;
    int sp[2];
    struct EI ei;
    if (ApproachLinear(data_ov004_020bfa04, 0, 1) == 0)
        return;
    step = 8;
    n = data_ov004_020bf9f8;
    if (n > 0x14) limit = 0x14; else limit = n;
    if (n > 0x14) step >>= 1;
    idx = 0;
    if (limit > 0) {
        ei.e = &data_ov004_020bfa34;
        ei.i = idx;
        do {
            int row = idx / 5;
            int col = idx % 5;
            cx = col << 0x10;
            rx = -(row << 0x10);
            {
                int ret = -func_ov004_020b04c0();
                sp[1] = (ret - 0x10) << 0xc;
            }
            sp[0] = 0x60000;
            {
                int c2 = data_ov004_020bf9f8;
                if (c2 < 5)
                    sp[0] = (0x80 - ((c2 - 1) << 3)) << 0xc;
            }
            sp[0] = sp[0] + cx;
            sp[1] = sp[1] + rx;
            func_ov004_020b4b84(ei.e, sp);
            {
                short* p = (short*)(int)(ei.e + 0x20);
                int cur = load_s(p);
                *p = cur + ei.i;
            }
            idx++;
            ei.e += 0x24;
            ei.i += step;
        } while (idx < limit);
    }
    data_ov004_020bfa04 = 0;
    data_ov004_020bfa20 = (void*)func_ov004_020b53f0;
}
