//cpp
int ApproachLinear(int&, int, int);

extern "C" {
extern int func_ov004_020b04c0(void);
extern void func_ov004_020b5108(char* c, int* in);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);

extern int data_ov004_020bfa00;
extern int data_ov004_020bfa04;
extern int data_ov004_020bfa10;
extern int data_ov004_020bfa18;
extern int data_ov004_020bfa1c;
extern int data_ov004_020bfa20;
extern short data_ov004_020bf9e4;
extern char data_ov004_020bfa34[];
}

extern "C" void func_ov004_020b5f6c(void)
{
    int inA[2];
    int inB[2];
    if (data_ov004_020bfa18 <= data_ov004_020bfa00)
        return;
    if (data_ov004_020bfa00 < data_ov004_020bfa1c) {
        if (ApproachLinear(data_ov004_020bfa04, 0, 1) == 0)
            return;
        {
            int v1 = (-func_ov004_020b04c0() - 0x10) << 12;
            int v0 = (data_ov004_020bfa00 * 16 + 8) << 12;
            inA[0] = v0;
            inA[1] = v0 ? v1 : v1;
        }
        func_ov004_020b5108(&data_ov004_020bfa34[data_ov004_020bfa00 * 0x24], inA);
        _ZN5Sound12PlayBank2_2DEj(0x14d);
        data_ov004_020bfa04 = 0x18;
        if (data_ov004_020bfa00 == data_ov004_020bfa18 - 1) {
            data_ov004_020bfa10 = 0;
            data_ov004_020bf9e4 = 1;
            data_ov004_020bfa20 = 0;
            return;
        }
        data_ov004_020bfa00 = data_ov004_020bfa00 + 1;
        return;
    }
    {
        int v1 = (-func_ov004_020b04c0() - 0x10) << 12;
        int v0 = (data_ov004_020bfa00 * 16 + 8) << 12;
        inB[0] = v0;
        inB[1] = v0 ? v1 : v1;
        func_ov004_020b5108(&data_ov004_020bfa34[data_ov004_020bfa00 * 0x24], inB);
    }
    data_ov004_020bfa10 = 0;
    if (data_ov004_020bfa00 == data_ov004_020bfa18 - 1) {
        data_ov004_020bf9e4 = 1;
        data_ov004_020bfa20 = 0;
        return;
    }
    data_ov004_020bfa00 = data_ov004_020bfa00 + 1;
}
