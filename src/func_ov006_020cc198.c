//cpp
// NONMATCHING: different op / idiom (div=12). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12;
struct BCA_File;
struct ModelAnim { int pad; void SetAnim(BCA_File *f, int a, Fix12 b, unsigned int c); };
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
};
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_ov006_02140544[];
extern int RNG_STATE;
extern int data_ov006_02140558[];
extern int data_ov006_02140578[];
extern int *data_ov006_0213b22c[];
extern int data_ov006_0213b1dc[2];
extern void func_ov006_020e6df0(int a0, char *a1, void *a2);
extern void func_ov006_020cbd7c(char *c);
}

extern "C" void func_ov006_020cc198(char *c)
{
    int *p38;
    *(int *)(c + 0x40) = data_ov006_02140544[0];
    *(int *)(c + 0x34) = data_ov006_02140558[0] +
        (int)(((unsigned int)RandomIntInternal(&RNG_STATE) & 0x7fffffff) >> 19);
    p38 = (int *)(c + 0x38);
    if (*(int *)(c + 0x1c) > 0)
        *(int *)(c + 0x34) = -*(int *)(c + 0x34);
    *p38 += (int)((((long long)data_ov006_02140578[0] << 11) + 0x800) >> 12);
    ((Obj *)c)->v4();
    *(int *)(c + 0x58) = 0;
    ((ModelAnim *)(c + 0x6c))->SetAnim(
        (BCA_File *)*data_ov006_0213b22c[*(int *)(c + 0x58)], 0x40000000, 0x800, 0);
    *(int *)(c + 0xc4) = 0;
    func_ov006_020e6df0(0, (char *)*(int *)(c + 0x58), (void *)*(int *)(c + 0x1c));
    *(int *)(c + 0x64) = data_ov006_0213b1dc[0];
    *(int *)(c + 0x68) = data_ov006_0213b1dc[1];
    func_ov006_020cbd7c(c);
}
