//cpp
typedef struct { int a, b, c; } Vec3;
extern "C" {

extern void func_ov081_0212777c(char *c, int a);
extern void func_ov081_021265c8(void *c);

int func_ov081_02126fa4(char *self)
{
    int b;
    b = (*(int *)(self + 0xb0) & 0x40000) != 0;
    if (b) {
        int *sv = (int *)(((long long)(int)(*(char **)(self + 0xd0) + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        *(int *)(self + 0x5c) = sv[0];
        *(int *)(self + 0x60) = sv[1];
        *(int *)(self + 0x64) = sv[2];
    }
    b = (*(int *)(self + 0xb0) & 0x80000) != 0;
    if (b) {
        func_ov081_0212777c(self, 7);
    } else {
        b = (*(int *)(self + 0xb0) & 0x20000) != 0;
        if (!b) {
            b = (*(int *)(self + 0xb0) & 0x40000) != 0;
            if (!b) {
                func_ov081_021265c8(self);
            }
        }
    }
    return 1;
}

}
