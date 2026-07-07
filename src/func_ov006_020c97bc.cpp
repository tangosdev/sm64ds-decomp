//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern "C" {
void func_ov006_020ca2ec(void *c);
void func_ov006_020bfec0(void *a0, void *a1, short *a2);
int _Z14ApproachLinearRiii(int *v, int step, int rate);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisPtr, void *file, int i, int fix, unsigned int flags);
void func_ov006_020e6df0(int a0, int a1, int a2);
int _Z14ApproachLinearRsss(short *v, short step, short rate);
}

extern void *data_ov006_02141a40;
extern int data_ov006_02140574;
extern int data_ov006_021405a8;
extern void *data_ov006_0213b22c[];
extern int data_ov006_021405b4;

struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void m4();
};

extern "C" void func_ov006_020c97bc(char *c)
{
    int v24;

    if (*(int *)(c + 0x40) <= 0) {
        func_ov006_020ca2ec(c);
        return;
    }

    func_ov006_020bfec0(data_ov006_02141a40, (void *)(c + 0x24), (short *)(c + 0x56));

    v24 = *(int *)(c + 0x24);

    if (v24 >= -0x68000) goto L5c;
    if (*(int *)(c + 0x3c) < 0) goto L70;

L5c:
    if (v24 <= 0x68000) goto L230;
    if (*(int *)(c + 0x3c) <= 0) goto L230;

L70:
    {
        int v = v24;
        if (v < -0x90000) v = -0x90000;
        else if (v > 0x90000) v = 0x90000;
        *(int *)(c + 0x24) = v;
    }

    if (*(s16 *)(c + 0x22) != 0) goto L1a4;
    if (_Z14ApproachLinearRiii((int *)(c + 0x64), 5, 1) != 0) goto L1a4;
    {
        int t = (int)(((long long)(-*(int *)(c + 0x3c)) * 0x1200 + 0x800) >> 12);
        int v40;

        if (t < -0x1400) t = -0x1400;
        else if (t > 0x1400) t = 0x1400;
        *(int *)(c + 0x3c) = t;

        *(int *)(((int)c + 0x40) & 0xFFFFFFFFFFFFFFFF) +=
            (int)(((long long)data_ov006_02140574 * 0x400 + 0x800) >> 12);

        v40 = data_ov006_021405a8;
        if (*(int *)(c + 0x40) <= v40) v40 = *(int *)(c + 0x40);
        *(int *)(c + 0x40) = v40;

        *(int *)(c + 0x60) = 2;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x78),
            *(void **)data_ov006_0213b22c[*(int *)(c + 0x60)], 0x40000000, 0x800, 0);

        *(int *)(c + 0xd0) = 0;
        func_ov006_020e6df0(0, *(int *)(c + 0x60), *(int *)(c + 0x24));
    }
    goto L260;

L1a4:
    {
        int t = (int)(((long long)(-*(int *)(c + 0x3c)) * 0xf00 + 0x800) >> 12);
        int pos;
        int hi;
        int lo;
        int v40;

        *(int *)(c + 0x3c) = t;

        pos = *(int *)(c + 0x24);
        if (pos < -0x68000) pos = -0x68000;
        else if (pos > 0x68000) pos = 0x68000;
        *(int *)(c + 0x24) = pos;

        hi = data_ov006_021405a8;
        lo = data_ov006_021405b4;
        v40 = *(int *)(c + 0x40);
        if (v40 < lo) {
            v40 = lo;
        } else {
            if (v40 <= hi) hi = v40;
            v40 = hi;
        }
        *(int *)(c + 0x40) = v40;
    }
    goto L260;

L230:
    if (*(u16 *)(c + 0x18) == 3) {
        ((VtObj *)c)->m4();
        *(int *)(((int)c + 0x3c) & 0xFFFFFFFFFFFFFFFF) *= -1;
    }

L260:
    {
        int v = *(int *)(c + 0x3c);
        if (v > 0x2000) {
            _Z14ApproachLinearRsss((short *)(c + 0x52), 0x2000, 0xc00);
        } else if (v < -0x2000) {
            _Z14ApproachLinearRsss((short *)(c + 0x52), -0x2000, 0xc00);
        } else {
            _Z14ApproachLinearRsss((short *)(c + 0x52), (short)v, 0xc00);
        }
    }
}
