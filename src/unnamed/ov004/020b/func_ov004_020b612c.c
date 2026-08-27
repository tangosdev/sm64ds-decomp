extern int _Z14ApproachLinearRiii(int* r, int a, int b);
extern int func_ov004_020b04c0(void);
extern void func_ov004_020b5108(char* c, int* in);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);

extern int data_ov004_020bfa18;
extern int data_ov004_020bfa00;
extern int data_ov004_020bfa04;
extern char data_ov004_020bfa34[];
extern short data_ov004_020bf9e4;
extern int data_ov004_020bfa20;

void func_ov004_020b612c(void)
{
    int in[2];
    int n;
    int idx;
    if (data_ov004_020bfa18 <= data_ov004_020bfa00) return;
    if (_Z14ApproachLinearRiii(&data_ov004_020bfa04, 0, 1) == 0) return;
    n = -func_ov004_020b04c0();
    idx = data_ov004_020bfa00;
    n = (n - 0x10) << 12;
    in[0] = (idx * 16 + 8) << 12;
    in[1] = n;
    func_ov004_020b5108(data_ov004_020bfa34 + idx * 0x24, in);
    _ZN5Sound12PlayBank2_2DEj(0x14d);
    {
        int b = data_ov004_020bfa00;
        if (b == data_ov004_020bfa18 - 1) {
            data_ov004_020bf9e4 = 1;
            data_ov004_020bfa20 = 0;
            return;
        }
        data_ov004_020bfa00 = b + 1;
        data_ov004_020bfa04 = 0x18;
    }
}
