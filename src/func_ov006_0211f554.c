// NONMATCHING: different op / idiom (div=14). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_ov006_0211f454(char *c, int i);
extern void func_ov006_0211f34c(char *c, int i);
extern short SINE_TABLE[];

void func_ov006_0211f554(char *c, int i)
{
    char *e = c + i * 0x24;
    int v;
    *(unsigned short *)(e + 0x466e) = 0;
    func_ov006_0211f454(c, i);
    func_ov006_0211f34c(c, i);
    v = SINE_TABLE[(*(unsigned short *)(e + 0x466c) >> 4) * 2 + 1];
    if (v >= 0)
        *(unsigned char *)(e + 0x467e) = 1;
    else
        *(unsigned char *)(e + 0x467e) = 0;
}
