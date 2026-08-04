extern void func_ov004_020adb1c(int self);
extern void func_ov006_020faeec(char *p);
extern void func_ov006_020fadfc(char *p);
extern void func_ov006_020fad90(char *c);

void func_ov006_020feba8(char *self)
{
    int i, j, k, l, m, n;
    char *p, *q, *r, *s, *t, *u;

    *(int *)(self + 0x5c10) = 0;

    p = self;
    for (i = 0; i < 30; i++) {
        *(int *)(p + 0x4cf0) = 0;
        *(int *)(p + 0x4cf4) = 0;
        *(short *)(p + 0x4cf8) = 0;
        *(short *)(p + 0x4cfa) = 0;
        p += 0xc;
    }

    q = self;
    for (j = 0; j < 3; j++) {
        *(char *)(q + 0x4e6c) = 0;
        *(char *)(q + 0x4e6d) = 0;
        q += 0x18;
    }

    r = self;
    for (k = 0; k < 2; k++) {
        *(int *)(r + 0x4ea0) = 0;
        *(int *)(r + 0x4ea4) = 0;
        *(int *)(r + 0x4ea8) = 0;
        *(int *)(r + 0x4eac) = 0;
        *(short *)(r + 0x4eb0) = 0;
        *(char *)(r + 0x4eb5) = 0;
        *(char *)(r + 0x4eb3) = 0;
        *(char *)(r + 0x4eb2) = 0;
        *(char *)(r + 0x4eb7) = 0;
        *(char *)(r + 0x4eb8) = 0;
        r += 0x1c;
    }

    s = self;
    for (l = 0; l < 0x30; l++) {
        *(int *)(s + 0x4ed8) = 0;
        *(int *)(s + 0x4edc) = 0;
        *(int *)(s + 0x4ef0) = 0;
        *(int *)(s + 0x4ef4) = 0;
        *(int *)(s + 0x4ef8) = 0;
        *(int *)(s + 0x4efc) = 0;
        *(short *)(s + 0x4f08) = 0;
        *(short *)(s + 0x4f0a) = 0;
        *(char *)(s + 0x4f0c) = 0;
        *(char *)(s + 0x4f0d) = 0;
        *(char *)(s + 0x4f0e) = 0;
        s += 0x38;
    }

    t = self;
    for (m = 0; m < 30; m++) {
        *(char *)(t + 0x468c) = 0;
        *(char *)(t + 0x468d) = 0;
        *(char *)(t + 0x468f) = 0;
        t += 0x38;
    }

    u = self;
    for (n = 0; n < 30; n++) {
        *(char *)(u + 0x5964) = 0;
        *(char *)(u + 0x5966) = 0;
        u += 0x14;
    }

    *(char *)(self + 0x5bc6) = 0;
    *(char *)(self + 0x5bc8) = 0;
    *(char *)(self + 0x5bc7) = 0;
    *(short *)(self + 0x5c18) = 0;
    *(short *)(self + 0x5c22) = 0;
    *(short *)(self + 0x5c1e) = 0;
    *(char *)(self + 0x5c30) = 0;
    *(char *)(self + 0x5c31) = 0;
    *(char *)(self + 0x5c32) = 0;
    *(short *)(self + 0x5c24) = 0;
    *(char *)(self + 0x5c34) = 0;
    *(short *)(self + 0x5c26) = 0;
    *(short *)(self + 0x5c28) = 0;
    *(short *)(self + 0x5c2a) = 0;
    *(short *)(self + 0x5c20) = 0;

    func_ov004_020adb1c(0);
    func_ov006_020faeec(self);
    func_ov006_020fadfc(self);
    func_ov006_020fad90(self);
}
