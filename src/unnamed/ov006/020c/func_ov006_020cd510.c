extern int func_020531a4(int a);
extern int data_ov006_02140548;
extern int data_ov006_02140544;
extern int data_ov006_02140558;
extern int data_ov006_02140578;
extern int data_ov006_02140570;

void func_ov006_020cd510(int a)
{
    int v;
    if (a > 0x2800) a = 0x2800;
    v = func_020531a4(a);
    data_ov006_02140548 = (int)(((long long)v * -0x2400 + 0x800) >> 12);
    data_ov006_02140544 = (int)(((long long)a * 0xe0 + 0x800) >> 12);
    data_ov006_02140558 = (int)(((long long)v * 0xc00 + 0x800) >> 12);
    data_ov006_02140578 = (int)(((long long)v * 0x4800 + 0x800) >> 12);
    data_ov006_02140570 = (int)(((long long)v * 0x5400 + 0x800) >> 12);
}
