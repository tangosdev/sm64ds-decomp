//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern "C" {
void func_ov006_020cbfd8(void *c);
void func_ov006_020cbaec(void *c);
void func_ov006_020bfec0(void *a0, void *a1, short *a2);
void func_ov006_020cb814(void *c);
void func_ov006_020cb690(void *c);
int func_ov006_020e6e3c(int a, int b);
int _Z14ApproachLinearRsss(short *v, short step, short rate);
void func_ov006_020cc37c(void *c);
}

extern void *data_ov006_02141a40;
extern int data_ov006_021405c8[2];

struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void m4();
};

extern "C" void func_ov006_020cc408(char *c)
{
    u16 state = *(u16 *)(c + 0x18);
    int v;

    if (state == 1) {
        func_ov006_020cbfd8(c);
        return;
    }
    if (state == 2) {
        func_ov006_020cbaec(c);
        return;
    }

    func_ov006_020bfec0(data_ov006_02141a40, (void *)(c + 0x1c), (short *)(c + 0x4e));

    v = *(int *)(c + 0x1c);

    if (v >= -0x5c000) goto L68;
    if (*(int *)(c + 0x34) < 0) goto L7c;

L68:
    if (v <= 0x5c000) goto L150;
    if (*(int *)(c + 0x34) <= 0) goto L150;

L7c:
    {
        int f0 = data_ov006_021405c8[0];
        int p = *(int *)(c + 0x20);

        if (p > f0) {
            *(s16 *)(c + 0x52) = 0;
            func_ov006_020cb814(c);
            return;
        }

        f0 -= 0x20000;
        if (p <= f0) goto Ld4;
        if (v <= 0) goto L130;
        *(s16 *)(c + 0x52) = 0;
        func_ov006_020cb690(c);
        return;

    Ld4:
        if (v <= 0) goto L130;
        {
            int f1 = data_ov006_021405c8[1];
            int hi1 = f1 + 0x30000;

            if (p >= hi1) goto L130;
            if (p <= f1) goto L10c;
            *(s16 *)(c + 0x52) = 1;
            func_ov006_020cb814(c);
            return;

        L10c:
            f1 -= 0x20000;
            if (p <= f1) goto L130;
            *(s16 *)(c + 0x52) = 1;
            func_ov006_020cb690(c);
            return;
        }
    }

L130:
    func_ov006_020e6e3c(0x1b3, v);
    *(int *)(((int)c + 0x34) & 0xFFFFFFFFFFFFFFFF) *= -1;
    goto L180;

L150:
    if (*(u16 *)(c + 0x18) == 3) {
        ((VtObj *)c)->m4();
        *(int *)(((int)c + 0x34) & 0xFFFFFFFFFFFFFFFF) *= -1;
    }

L180:
    {
        int r = *(int *)(c + 0x34);
        if (r > 0x4000) {
            _Z14ApproachLinearRsss((short *)(c + 0x4a), 0x4000, 0xc00);
        } else if (r < -0x8000) {
            _Z14ApproachLinearRsss((short *)(c + 0x4a), -0x4000, 0xc00);
        } else {
            _Z14ApproachLinearRsss((short *)(c + 0x4a), (short)r, 0xc00);
        }
    }

    if (*(int *)(c + 0x20) >= -0xc8000) return;

    func_ov006_020cc37c(c);
}
