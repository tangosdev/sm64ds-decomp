//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern "C" {
void func_ov006_020cc618(void *c);
void func_ov006_020bfec0(void *a0, void *a1, short *a2);
void func_ov006_020cb814(void *c);
void func_ov006_020cb690(void *c);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisPtr, void *file, int i, int fix, unsigned int flags);
void func_ov006_020e6df0(int a0, int a1, int a2);
void func_02012718(int a, int b);
int _Z14ApproachLinearRsss(short *v, short step, short rate);
}

extern void *data_ov006_02141a40;
extern int data_ov006_021405c8[2];
extern void *data_ov006_0213b22c[];

struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void m4();
};

extern "C" void func_ov006_020cbd7c(char *c)
{
    int v;

    if (*(int *)(c + 0x38) <= 0) {
        func_ov006_020cc618(c);
        return;
    }

    func_ov006_020bfec0(data_ov006_02141a40, (void *)(c + 0x1c), (short *)(c + 0x4e));

    v = *(int *)(c + 0x1c);

    if (v >= -0x5c000) goto L5c;
    if (*(int *)(c + 0x34) < 0) goto L70;

L5c:
    if (v <= 0x5c000) goto L1ac;
    if (*(int *)(c + 0x34) <= 0) goto L1ac;

L70:
    {
        int f0 = data_ov006_021405c8[0];
        int p = *(int *)(c + 0x20);

        if (p > f0) {
            *(s16 *)(c + 0x52) = 0;
            func_ov006_020cb814(c);
            return;
        }

        f0 -= 0x20000;
        if (p <= f0) goto Ld0;
        if (v <= 0) goto L134;
        *(s16 *)(c + 0x52) = 0;
        func_ov006_020cb690(c);
        return;

    Ld0:
        if (v <= 0) goto L134;
        {
            int f1 = data_ov006_021405c8[1];
            int hi1 = f1 + 0x30000;

            if (p >= hi1) goto L134;
            if (p <= f1) goto L10c;
            *(s16 *)(c + 0x52) = 1;
            func_ov006_020cb814(c);
            return;

        L10c:
            f1 -= 0x20000;
            if (p <= f1) goto L134;
            *(s16 *)(c + 0x52) = 1;
            func_ov006_020cb690(c);
            return;
        }
    }

L134:
    if (*(int *)(c + 0x38) > 0) {
        *(int *)(c + 0x58) = 2;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x6c),
            *(void **)data_ov006_0213b22c[*(int *)(c + 0x58)], 0x40000000, 0x800, 0);

        *(int *)(c + 0xc4) = 0;
        func_ov006_020e6df0(0, *(int *)(c + 0x58), *(int *)(c + 0x1c));
    }

    func_02012718(0x1b3, *(int *)(c + 0x1c) + 0x80000);
    *(int *)(((int)c + 0x34) & 0xFFFFFFFFFFFFFFFF) *= -1;
    goto L1dc;

L1ac:
    if (*(u16 *)(c + 0x18) == 3) {
        ((VtObj *)c)->m4();
        *(int *)(((int)c + 0x34) & 0xFFFFFFFFFFFFFFFF) *= -1;
    }

L1dc:
    {
        int r = *(int *)(c + 0x34);
        if (r > 0x4000) {
            _Z14ApproachLinearRsss((short *)(c + 0x4a), 0x4000, 0xc00);
        } else if (r < -0x4000) {
            _Z14ApproachLinearRsss((short *)(c + 0x4a), -0x4000, 0xc00);
        } else {
            _Z14ApproachLinearRsss((short *)(c + 0x4a), (short)r, 0xc00);
        }
    }
}
