struct S { int a, b; };

extern int func_ov006_02114590(void *a0, struct S *p, struct S *q0, struct S *q1, struct S *q2);

int func_ov006_021126b4(char *self, struct S *arg) {
    int z, z2, x;

    if (*(unsigned char *)(self + 0x3a) == 1) {
        int dz = arg->b - *(int *)(self + 0x48);
        int dx = arg->a - *(int *)(self + 0x44);
        int dxsq = (int)(((long long)dx * dx + 0x800) >> 12);
        int dzsq = (int)(((long long)dz * dz + 0x800) >> 12);
        return dxsq + dzsq >= 0x100000;
    }

    x = arg->a;
    if (x < 0x8000) return 1;
    if (x >= 0xf8000) return 1;
    if (x >= 0xd8000 && x < 0xe0000) {
        z = arg->b;
        if (z >= -0x80000) {
            if (z < 0x78000) return 1;
        }
        if (*(unsigned char *)(self + 0x38) != 0) {
            if (z >= 0x78000) return 1;
        }
        if (*(unsigned char *)(self + 0x129) == 0) {
            if (z < -0x80000) return 1;
        }
    }

    z2 = arg->b;
    if (z2 < 0x80000) goto L1dc;
    if (z2 >= 0x88000) goto L14c;
    if (x < 0xe0000) return 1;
L14c:
    {
        struct S p, q0, q1, q2;
        p.a = x; p.b = arg->b;
        q0.a = 0x8000;  q0.b = 0x98000;
        q1.a = 0x8000;  q1.b = 0xa2000;
        q2.a = 0xe0000; q2.b = 0xa2000;
        if (func_ov006_02114590(self, &p, &q0, &q1, &q2)) return 1;
    }
    if (arg->a < 0x8000) goto L238;
    if (arg->a >= 0xe0000) goto L238;
    if (arg->b < 0xa2000) goto L238;
    return 1;
L1dc:
    if (z2 >= -0x70000) goto L238;
    {
        int u = x - 0x80000;
        int v = z2 + 0x70000;
        int usq = (int)(((long long)u * u + 0x800) >> 12);
        int vsq = (int)(((long long)v * v + 0x800) >> 12);
        if (usq + vsq >= 0x3840000) return 1;
    }
L238:
    return 0;
}
