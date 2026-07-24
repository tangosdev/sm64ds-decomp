//cpp
extern "C" {
int RandomIntInternal(int *seed);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *thisPtr, void *file, int i, int fix, unsigned int flags);
void func_ov006_020e6df0(int a0, int a1, int a2);
void func_ov006_020cbd7c(char *c);
}

extern int data_ov006_02140544;
extern int data_0209e650;
extern int data_ov006_02140558;
extern int data_ov006_02140578;
extern void *data_ov006_0213b22c[];
extern int data_ov006_0213b1dc[2];

struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void m4();
};

extern "C" void func_ov006_020cc198(char *c)
{
    int *p38;
    int b;

    *(int *)(c + 0x40) = data_ov006_02140544;
    *(int *)(c + 0x34) = data_ov006_02140558 +
        (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19);

    b = *(int *)(c + 0x1c);
    if (b > 0)
        *(int *)(c + 0x34) = -*(int *)(c + 0x34);

    p38 = (int *)(((long long)(int)(c + 0x38)) & 0xFFFFFFFFFFFFFFFFLL);
    *p38 += (int)((((long long)data_ov006_02140578 << 11) + 0x800) >> 12);

    ((VtObj *)c)->m4();

    *(int *)(c + 0x58) = 0;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        (void *)(c + 0x6c),
        *(void **)data_ov006_0213b22c[*(int *)(c + 0x58)],
        0x40000000, 0x800, 0);

    *(int *)(c + 0xc4) = 0;
    func_ov006_020e6df0(0, *(int *)(c + 0x58), *(int *)(c + 0x1c));

    {
        int a = data_ov006_0213b1dc[0];
        int b2 = data_ov006_0213b1dc[1];
        a = b2 ? a : a;
        *(int *)(c + 0x64) = a;
        *(int *)(c + 0x68) = b2;
    }

    func_ov006_020cbd7c(c);
}
