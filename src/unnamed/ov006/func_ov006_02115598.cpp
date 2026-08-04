//cpp
extern "C" int func_ov004_020adbc0(void);
extern "C" void func_ov004_020adb1c(int self);

typedef struct {
    char pad[0x478c];
    int A[5];
    int B[5];
    int C[5];
    int D[5][2];
    int E[5];
} S;

extern "C" void func_ov006_02115598(S* c, int* src, int v2, int v3, int v5)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (c->A[i] > 0) continue;
        if (v3 > 0) {
            c->E[i] = v3;
        } else {
            func_ov004_020adb1c(v2 + func_ov004_020adbc0());
            if ((unsigned int)func_ov004_020adbc0() > 0xf423f) {
                func_ov004_020adb1c(0xf423f);
            }
        }
        c->A[i] = v2;
        c->B[i] = v5;
        c->C[i] = 0;
        c->D[i][0] = src[0];
        c->D[i][1] = src[1];
        return;
    }
    func_ov004_020adb1c(v2 * v5 + func_ov004_020adbc0());
    if ((unsigned int)func_ov004_020adbc0() > 0xf423f) {
        func_ov004_020adb1c(0xf423f);
    }
}
