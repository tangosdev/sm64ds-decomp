//cpp
struct Obj { int dummy; };
typedef void (Obj::*PMF)(int);

extern "C" {
    void func_ov006_020dde28(void *c, int index);
    void func_ov006_020dd2cc(void *c);
    void func_ov006_020dca04(void *c);
    void func_ov006_020dc348(void *c);
    void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
    void func_ov004_020ae274(void *c);
    void func_ov004_020adb1c(int arg);
}
extern PMF data_ov006_02141840[];
extern char *func_020beb68;

extern "C" void func_ov006_020de26c(char *self) {
    int count;
    int i;
    char *r5;
    int j;
    char *r2;
    char *g;

    count = 0;
    for (i = 0, r5 = self; i < 0x28; i++) {
        if (*(unsigned char *)(r5 + 0x4677)) {
            int idx = *(unsigned char *)(r5 + 0x4675);
            (((Obj *)self)->*data_ov006_02141840[idx])(i);
            if (*(unsigned char *)(r5 + 0x4675) != 4) count++;
            if (*(unsigned char *)(r5 + 0x4676) != 0) func_ov006_020dde28(self, i);
        }
        r5 += 0x1c;
    }
    r2 = self;
    for (j = 0; j < 0x18; j++) {
        if (*(unsigned char *)(r2 + 0x4ad0) == 1 && *(unsigned char *)(r2 + 0x4ad1) != 0)
            count++;
        r2 += 0x18;
    }
    func_ov006_020dd2cc(self);
    func_ov006_020dca04(self);
    if (count != 0) return;

    if (((int *)(self + 0x5000))[0x73] != 0) {
        *(int *)(((long long)(int)(self + 0x51cc)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
        if (((int *)(self + 0x5000))[0x73] == 0x20 && ((unsigned char *)(self + 0x5000))[0x1df] == 0)
            func_ov006_020dc348(self);
        if (((int *)(self + 0x5000))[0x73] == 0x20 && ((unsigned char *)(self + 0x5000))[0x1df] != 0) {
            func_ov004_020b0cac(6, 0x80, -0x80, -1, -1, 0xd);
            func_ov004_020ae274(0);
        }
        if (((int *)(self + 0x5000))[0x73] <= 0) ((int *)(self + 0x5000))[0x73] = 0;
        return;
    }
    g = func_020beb68;
    func_ov004_020adb1c(g != 0 ? *(int *)(g + 0xa8) : 0);
    ((int *)(self + 0x5000))[0x73] = 0x70;
    ((int *)(self + 0x5000))[0x72] = 4;
}
