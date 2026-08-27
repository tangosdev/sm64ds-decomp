// NONMATCHING: different op / idiom (div=14). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int data_0209d4b8;
extern int data_ov006_0212eb2c[];
int RandomIntInternal(int* seed);

void func_ov006_020fc718(char *thiz, int n)
{
    int rnd = RandomIntInternal(&data_0209d4b8);
    int off = n * 0x1c;
    unsigned int x = ((unsigned)rnd >> 16) & 0x7fff;
    unsigned int idx = (x << 1) >> 0xf;
    int v = data_ov006_0212eb2c[idx];
    unsigned char *p = (unsigned char*)(thiz + 0x4eb3);
    char *q;
    p[off] = (unsigned char)v;
    q = thiz + off + 0x4000;
    *(unsigned char*)(q + 0xeb4) = 0;
    if (p[off] == 2) {
        *(unsigned char*)(q + 0xeb9) = 1;
        *(int*)(q + 0xea8) = 0x1000;
    } else {
        *(unsigned char*)(q + 0xeb9) = 0;
        *(int*)(q + 0xea8) = -0x1000;
    }
    *(short*)(thiz + off + 0x4e00 + 0xb0) = 0;
    *(unsigned char*)(thiz + off + 0x4000 + 0xeb6) = 0;
    *(unsigned char*)(thiz + off + 0x4000 + 0xeb8) = 1;
}
