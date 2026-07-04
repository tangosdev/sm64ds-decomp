//cpp
extern "C" int func_ov006_020c1718(int* r0);
extern "C" void func_ov004_020b0aa0(int arg);
extern "C" void func_ov004_020ad79c(int r0arg, int r1arg);

extern short data_ov004_020bf9e4;

extern "C" int func_ov006_0212527c(char* c, int mode)
{
    int st;
    short fl;
    if (*(int*)(c + 0x51b8) == 0xd) {
        if (func_ov006_020c1718((int*)(c + 0x4f38)) != 0) {
            if (mode == 4) {
                func_ov004_020b0aa0(0x1d);
                *(int*)(c + 0x51b8) = 9;
            } else if (mode == 5) {
                func_ov004_020b0aa0(0x1d);
                *(int*)(c + 0x51b8) = 0xa;
            }
        } else {
            return 0;
        }
    }
    st = *(int*)(c + 0x51b8);
    if (st >= 0xc) {
        fl = data_ov004_020bf9e4;
        if (fl == 1) goto do1;
    }
    return 0;
do1:
    if (st == 0xc && fl == 1) {
        func_ov004_020ad79c(*(int*)(c + 0xa8), *(int*)(c + 0xb4));
    }
    return 1;
}
