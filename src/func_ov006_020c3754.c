// NONMATCHING: register allocation (div=10). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int data_ov006_021402e0;
extern int data_ov006_021402e4;
extern int data_ov006_021402f0[3];
extern int RNG_STATE;
extern int data_02092768[4];
extern void func_020731dc(int a, int b, void **node);
extern void func_020072c0(void);
extern int RandomIntInternal(int *seed);
extern void func_0203cd80(void *q, int a);
extern void Quaternion_FromVector3(void *out, int *in, void *p);

void func_ov006_020c3754(int *r6, int *r5, int *r4)
{
    int *n;
    if (!(data_ov006_021402e0 & 1)) {
        n = data_ov006_021402f0;
        n[0] = 0;
        n[1] = 0x1000;
        n[2] = 0;
        func_020731dc((int)func_020072c0, (int)&data_ov006_021402e4, (void **)n);
        data_ov006_021402e0 |= 1;
    }
    r6[0] = r5[0];
    r6[1] = r5[1];
    r6[2] = r5[2];
    r6[3] = r4[0];
    r6[4] = r4[1];
    r6[5] = r4[2];
    r6[6] = 0;
    r6[7] = -0x40;
    r6[8] = 0;
    *(short *)((char *)r6 + 0x94) = 0xb4;
    r6[9] = data_ov006_021402f0[0];
    r6[10] = data_ov006_021402f0[1];
    r6[11] = data_ov006_021402f0[2];
    func_0203cd80((char *)r6 + 0x24, (short)RandomIntInternal(&RNG_STATE));
    r6[12] = data_02092768[0];
    r6[13] = data_02092768[1];
    r6[14] = data_02092768[2];
    r6[15] = data_02092768[3];
    Quaternion_FromVector3((char *)r6 + 0x30, (int *)((char *)r6 + 0x24), data_ov006_021402f0);
    *(int *)((char *)r6 + 0x40) = 1;
}
