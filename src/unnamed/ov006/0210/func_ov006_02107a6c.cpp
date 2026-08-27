//cpp
extern "C" {
    int _Z14ApproachLinearRiii(int& v, int a, int b);
    void func_02012790(int a);
}

extern int data_ov006_021428d0;
extern int data_ov006_0213e2dc;
extern char* data_ov006_02142aa4[];

extern "C" int func_ov006_02107a6c(void)
{
    if (_Z14ApproachLinearRiii(data_ov006_021428d0, 0, 1) != 0) {
        int idx;
        data_ov006_0213e2dc = data_ov006_0213e2dc + 1;
        idx = data_ov006_0213e2dc;
        data_ov006_021428d0 = 0x78;
        while (idx < 4) {
            if (*(short*)(data_ov006_02142aa4[idx] + 0x16) != 0) break;
            data_ov006_0213e2dc = data_ov006_0213e2dc + 1;
            idx = data_ov006_0213e2dc;
        }
        if (idx < 4) {
            func_02012790(0x26);
        }
    }
    return (data_ov006_0213e2dc >= 4) ? 1 : 0;
}
