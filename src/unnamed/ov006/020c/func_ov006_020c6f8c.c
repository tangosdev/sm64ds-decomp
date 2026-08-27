typedef int s32;

extern s32 data_ov006_0213b008;
extern s32 data_ov006_0213b00c;
extern s32 data_ov006_0213b010;
extern s32 data_ov006_0213b014;
extern s32 data_ov006_0213b018;
extern s32 data_ov006_0213b01c;

extern int func_020531a4(int a);

void func_ov006_020c6f8c(int a) {
    int b;
    if (a > 0x3000) a = 0x3000;
    b = func_020531a4(a);
    data_ov006_0213b010 = (int)(((long long)b * -0x2400 + 0x800) >> 12);
    data_ov006_0213b018 = (int)(((long long)a * 0xe0 + 0x800) >> 12);
    data_ov006_0213b00c = (int)(((long long)b * 0x4c00 + 0x800) >> 12);
    data_ov006_0213b01c = (int)(((long long)b * 0x200 + 0x800) >> 12);
    data_ov006_0213b014 = (int)(((long long)b * 0x180 + 0x800) >> 12);
    data_ov006_0213b008 = (int)(((long long)b * 0x5000 + 0x800) >> 12);
}
