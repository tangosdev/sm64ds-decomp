typedef struct Ent {
    char _pad0[0x30];
    int x;
    int y;
    char _pad38[0xb8];
} Ent;

extern int data_ov006_02140328;
extern Ent *data_ov006_02140324;
extern int data_ov006_0213afc8[2];
extern char data_ov006_0212ccf0;
extern int data_ov006_02140304;
extern int data_0209e650;

extern int RandomIntInternal(int *seed);
extern void func_ov006_020c68f4(char *a, char *b);
extern void _Z14ApproachLinearRiii(int *x, int target, int step);

void func_ov006_020c4148(void)
{
    int i;
    for (i = 0; i < data_ov006_02140328; i++) {
        int *v = (int *)(((long long)(int)&data_ov006_02140324[i].x) & 0xFFFFFFFFFFFFFFFFLL);
        volatile int *q = (volatile int *)data_ov006_0213afc8;
        if (v[0] != data_ov006_0213afc8[0]
            || (v[1] != q[1] && data_ov006_02140324[i].x != 0))
            continue;
        {
            int r1 = RandomIntInternal(&data_0209e650);
            int r2 = RandomIntInternal(&data_0209e650);
            unsigned v0 = (unsigned)(r1 & 0x7fffffff) >> 0x13;
            unsigned u0 = (unsigned)(r2 & 0x7fffffff) >> 0x13;
            func_ov006_020c68f4(
                (char *)data_ov006_02140324 + i * 0xf0,
                &data_ov006_0212ccf0
                    + (((int)(v0 * 9)) >> 0xc) * 0xf0
                    + (((int)(u0 * 5)) >> 0xc) * 0x30);
            _Z14ApproachLinearRiii(&data_ov006_02140304, data_ov006_02140328, 1);
            return;
        }
    }
}
