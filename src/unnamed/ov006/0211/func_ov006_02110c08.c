struct S { int a, b; };

extern long long func_0203d5bc(int *p);
extern int func_ov006_02114590(void *a0, struct S *p, struct S *q0, struct S *q1, struct S *q2);

int func_ov006_02110c08(char *self, struct S *p) {
    struct S d;
    struct S s0, s1, s2, s3;
    struct S t0, t1, t2, t3;
    struct S u0, u1, u2, u3;
    struct S v0, v1, v2, v3;

    d.a = p->a;
    d.b = p->b;
    d.a -= *(int *)(self + 0x8);
    d.b -= *(int *)(self + 0xc);
    if (func_0203d5bc(&d.a) >= 0x400000) return 0;

    s0.a = p->a; s0.b = p->b;
    s1.a = *(int *)(self + 0x38); s1.b = *(int *)(self + 0x3c);
    s2.a = *(int *)(self + 0x40); s2.b = *(int *)(self + 0x44);
    s3.a = *(int *)(self + 0x48); s3.b = *(int *)(self + 0x4c);
    if (func_ov006_02114590(self, &s0, &s1, &s2, &s3)) return 1;

    t0.a = p->a; t0.b = p->b;
    t1.a = *(int *)(self + 0x50); t1.b = *(int *)(self + 0x54);
    t2.a = *(int *)(self + 0x40); t2.b = *(int *)(self + 0x44);
    t3.a = *(int *)(self + 0x48); t3.b = *(int *)(self + 0x4c);
    if (func_ov006_02114590(self, &t0, &t1, &t2, &t3)) return 1;

    u0.a = p->a; u0.b = p->b;
    u1.a = *(int *)(self + 0x58); u1.b = *(int *)(self + 0x5c);
    u2.a = *(int *)(self + 0x60); u2.b = *(int *)(self + 0x64);
    u3.a = *(int *)(self + 0x68); u3.b = *(int *)(self + 0x6c);
    if (func_ov006_02114590(self, &u0, &u1, &u2, &u3)) return 1;

    v0.a = p->a; v0.b = p->b;
    v1.a = *(int *)(self + 0x70); v1.b = *(int *)(self + 0x74);
    v2.a = *(int *)(self + 0x60); v2.b = *(int *)(self + 0x64);
    v3.a = *(int *)(self + 0x68); v3.b = *(int *)(self + 0x6c);
    return func_ov006_02114590(self, &v0, &v1, &v2, &v3) != 0;
}
