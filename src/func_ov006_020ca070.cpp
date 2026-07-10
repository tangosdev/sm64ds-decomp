//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern "C" {
void func_ov006_020c9aa0(char *c);
void func_ov006_020c94e0(char *c);
void func_ov006_020bfec0(void *a0, void *a1, short *a2);
int _Z14ApproachLinearRiii(int *v, int step, int rate);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisPtr, void *file, int i, int fix, unsigned int flags);
void func_ov006_020e6df0(int a0, int a1, int a2);
int _Z14ApproachLinearRsss(short *v, short step, short rate);
void func_ov006_020c9fe4(char *c);
}

extern void *data_ov006_02141a40;
extern void *data_ov006_0213b22c[];

struct S8 { int a, b; };
extern S8 data_ov006_0213b14c;

struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void m4();
};

extern "C" void func_ov006_020ca070(char *c)
{
    u16 state = *(u16 *)(c + 0x18);
    int v24;

    if (state == 1) {
        func_ov006_020c9aa0(c);
        return;
    }
    if (state == 2) {
        func_ov006_020c94e0(c);
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

    *(int *)(((int)c + 0x3c) & 0xFFFFFFFFFFFFFFFFLL) *= -1;
    if (*(int *)(c + 0x40) <= -0x800) goto L260;

    if (*(s16 *)(c + 0x22) != 0) goto L1a4;
    if (_Z14ApproachLinearRiii((int *)(c + 0x64), 5, 1) != 0) goto L1a4;
    {
        *(int *)(c + 0x40) = 0x800;
        *(int *)(c + 0x60) = 2;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x78),
            *(void **)data_ov006_0213b22c[*(int *)(c + 0x60)], 0x40000000, 0x800, 0);

        *(int *)(c + 0xd0) = 0;
        func_ov006_020e6df0(0, *(int *)(c + 0x60), *(int *)(c + 0x24));

        {
            long long ll = *(long long *)&data_ov006_0213b14c;
            *(int *)(c + 0x70) = (int)ll;
            *(int *)(c + 0x74) = (int)((unsigned long long)ll >> 32);
        }
    }
    return;

L1a4:
    {
        int t = (int)(((long long)(-*(int *)(c + 0x3c)) * 0xf00 + 0x800) >> 12);
        int pos;

        *(int *)(c + 0x3c) = t;

        pos = *(int *)(c + 0x24);
        if (pos < -0x68000) pos = -0x68000;
        else if (pos > 0x68000) pos = 0x68000;
        *(int *)(c + 0x24) = pos;
    }
    goto L260;

L230:
    if (*(u16 *)(c + 0x18) == 3) {
        ((VtObj *)c)->m4();
        *(int *)(((int)c + 0x3c) & 0xFFFFFFFFFFFFFFFFLL) *= -1;
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

    if (*(int *)(c + 0x28) >= -0xc8000) return;

    func_ov006_020c9fe4(c);
}
