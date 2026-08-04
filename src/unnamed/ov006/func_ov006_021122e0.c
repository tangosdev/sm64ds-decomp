/* func_ov006_021122e0 at 0x021122e0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
struct S { int a, b; };

extern int func_ov006_02114590(void *a0, struct S *p, struct S *q0, struct S *q1, struct S *q2);
extern void func_ov006_02115680(int x, int y);

int func_ov006_021122e0(char *self, struct S *arg) {
    struct S p, q0, q1, q2;
    struct S d, e, f, g;
    if (arg->b >= 0) return 0;
    if (arg->a >= 0x8000 && arg->a < 0x28000) {
        if (arg->b >= -0x90000 && arg->b < -0x88000) {
            func_ov006_02115680(*(int *)(self + 4), 0);
            return 1;
        }
        p.a = arg->a; p.b = arg->b;
        q0.a = 0x8000;  q0.b = -0x92000;
        q1.a = 0x8000;  q1.b = -0x90000;
        q2.a = 0x28000; q2.b = -0x90000;
        if (func_ov006_02114590(self, &p, &q0, &q1, &q2)) {
            func_ov006_02115680(*(int *)(self + 4), 0);
            return 1;
        }
    }
    if (*(unsigned char *)(self + 0x129) == 1 && *(int *)(self + 0x24) >= 0 &&
        arg->a >= 0xd0000 && arg->a < 0xf8000) {
        d.a = arg->a; d.b = arg->b;
        e.a = 0xd8000; e.b = -0x80000;
        f.a = 0xf8000; f.b = -0x80000;
        g.a = 0xf8000; g.b = -0x88000;
        if (func_ov006_02114590(self, &d, &e, &f, &g)) {
            func_ov006_02115680(*(int *)(self + 4), 1);
            return 1;
        }
    }
    return 0;
}