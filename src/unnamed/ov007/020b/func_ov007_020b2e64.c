extern int* data_ov007_02103360;
extern int* data_ov007_0210342c;
extern int data_ov007_02103364;
extern int func_ov007_020c1da0(int i);
extern int _ZN4cstd3modEii(int a, int b);
extern void func_ov007_020c92d0(int q);
extern int func_ov007_020b3360(void);
extern void func_ov007_020b3e84(short* r0);
extern int func_ov007_020b3190(void);

void func_ov007_020b2e64(char* self)
{
    unsigned int* r4;
    int r2;
    int r4v;

    data_ov007_02103360[0x10 / 4] = 0;
    r4 = (unsigned int*)(*(int*)((char*)data_ov007_0210342c + 0x54) + 4);

    if (func_ov007_020c1da0(0) != 0) {
        r2 = -1;
    } else if (r4[0] == 1 || (r4[0] >= 0x14 && _ZN4cstd3modEii(r4[0] - 0x14, 0xa) == 0)) {
        r2 = 0;
    } else if (r4[1] == 1 || (r4[1] >= 0x14 && _ZN4cstd3modEii(r4[1] - 0x14, 0xa) == 0)) {
        r2 = 1;
    } else if (r4[2] == 1 || (r4[2] >= 0x14 && _ZN4cstd3modEii(r4[2] - 0x14, 0xa) == 0)) {
        r2 = 2;
    } else if (r4[3] == 1 || (r4[3] >= 0x14 && _ZN4cstd3modEii(r4[3] - 0x14, 0xa) == 0)) {
        r2 = 3;
    } else {
        r2 = -1;
    }

    data_ov007_02103360[0x14 / 4] = r2;

    r4v = 0;
    if (func_ov007_020c1da0(0) == 0) {
        int* p = *(int**)((char*)data_ov007_0210342c + 0x54);
        int flag = 1;
        if (p[0x14 / 4] != 1) {
            if (p[0x2c / 4] != 1) flag = r4v;
        }
        if (flag != 0) r4v = 1;
    }
    data_ov007_02103360[4] = r4v;
    func_ov007_020c92d0(data_ov007_02103360[0]);

    if (*(short*)(data_ov007_02103360[0]) != 0xff) {
        func_ov007_020b3360();
        if (data_ov007_02103360[0x10 / 4] == 0) {
            return;
        }
        data_ov007_02103364 = (data_ov007_02103364 == 0) ? 1 : 0;
        func_ov007_020b3e84((short*)self);
        return;
    }

    func_ov007_020b3190();
}
