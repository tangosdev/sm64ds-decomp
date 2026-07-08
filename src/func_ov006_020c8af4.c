typedef long long s64;
extern int func_ov004_020ad674(void);
extern void func_ov004_020af68c(void* a0, int a1, int a2, int a3, int a4);
extern void **data_ov006_021404c8;
extern void **data_ov006_0213b0d8[];
extern short SINE_TABLE[];

void func_ov006_020c8af4(char *c)
{
    void *x;
    int t;
    int prod;
    int pos;
    if (*(short*)(c + 0x18) == 0) return;
    if (data_ov006_021404c8 != 0) {
        x = data_ov006_021404c8[*(short*)(c + 0x1a)];
    } else {
        int idx2 = func_ov004_020ad674();
        x = data_ov006_0213b0d8[idx2][*(short*)(c + 0x1a)];
    }
    t = SINE_TABLE[(*(unsigned short*)(c + 0x1c) >> 4) * 2];
    prod = (int)(((s64)*(int*)(c + 0x10) * t + 0x800) >> 12);
    pos = *(int*)c + prod;
    func_ov004_020af68c(x, pos >> 12, *(int*)(c + 4) >> 12, -1, -1);
}
