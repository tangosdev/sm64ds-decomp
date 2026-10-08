extern int func_ov007_020bf630(int* self, int* p1, int p2, int p3, int arg5);
extern int _ZN4cstd3modEii(int, int);
extern int _ZN4cstd3divEii(int, int);
extern void func_ov007_020ca578(int* a, int b, int c, int d);
extern int *data_ov007_02104bd4;
extern int *data_ov007_0210342c;

void func_ov007_020bed60(int arg0, int arg1, int arg2)
{
    int *self = (int*)*data_ov007_02104bd4;
    int *p1 = (int*)(data_ov007_0210342c[0x30 / 4]);
    int res = func_ov007_020bf630(self, p1, arg0, arg1, -1);
    if (res == -1) return;
    {
        int m = _ZN4cstd3modEii(res, *(int*)(self[1]));
        int d = _ZN4cstd3divEii(res, *(int*)(self[1]));
        func_ov007_020ca578(self, m, d, arg2);
    }
    {
        int *p = (int*)(data_ov007_02104bd4[0xc / 4]);
        p[1] = 0x78;
    }
}
