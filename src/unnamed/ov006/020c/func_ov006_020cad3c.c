extern int func_020531a4(int a);
extern int data_ov006_021405b4;
extern int data_ov006_021405b0;
extern int data_ov006_0214053c;
extern int data_ov006_02140574;
extern int data_ov006_021405a8;
extern int data_ov006_021405a4;

void func_ov006_020cad3c(int a)
{
    int v;
    if (a > 0x2800) a = 0x2800;
    v = func_020531a4(a);
    data_ov006_021405b4 = (int)(((long long)v * -0x1c00 + 0x800) >> 12);
    data_ov006_021405b0 = (int)(((long long)a * 0xc0 + 0x800) >> 12);
    data_ov006_0214053c = (int)(((long long)v * 0xe00 + 0x800) >> 12);
    data_ov006_02140574 = (int)(((long long)v * 0x5800 + 0x800) >> 12);
    data_ov006_021405a8 = (int)(((long long)v * 0x6000 + 0x800) >> 12);
    data_ov006_021405a4 = (int)(((long long)v * 0x180 + 0x800) >> 12);
}
