// NONMATCHING: register allocation (div=26). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12;
typedef unsigned int u32;

extern int data_ov006_02140328;      /* count */
extern char* data_ov006_02140324;    /* base pointer */
extern int data_ov006_0213afc8;      /* compare struct {a, b} */
extern char data_ov006_0212ccf0;     /* second array base */
extern int data_ov006_02140304;      /* ApproachLinear target */
extern int RNG_STATE;            /* rand seed */

extern int RandomIntInternal(int* seed);
extern void func_ov006_020c68f4(char* a, char* b);
extern void _Z14ApproachLinearRiii(int* x, int target, int step);

void func_ov006_020c4148(void) {
    int i = 0;
    char* e;
    int marker;
    if (data_ov006_02140328 <= 0) return;
    e = data_ov006_02140324;
    marker = data_ov006_0213afc8;
    for (;;) {
        int* p = (int*)(e + 0x30);
        if (p[0] == marker) {
            if (p[1] == (&data_ov006_0213afc8)[1] ||
                *(int*)(e + 0x30) == 0) {
                int r1 = RandomIntInternal(&RNG_STATE);
                int r2 = RandomIntInternal(&RNG_STATE);
                u32 v = (u32)(r1 & 0x7fffffff) >> 0x13;
                u32 u = (u32)(r2 & 0x7fffffff) >> 0x13;
                func_ov006_020c68f4(
                    data_ov006_02140324 + i * 0xf0,
                    &data_ov006_0212ccf0 + (((int)(v * 9)) >> 0xc) * 0xf0
                        + (((int)(u * 5)) >> 0xc) * 0x30);
                _Z14ApproachLinearRiii(&data_ov006_02140304, data_ov006_02140328, 1);
                return;
            }
        }
        i++;
        e += 0xf0;
        if (i >= data_ov006_02140328) return;
    }
}
