// NONMATCHING: extra logic (you do more) (div=26). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int RandomIntInternal(int *seed);
extern int RNG_STATE;
extern int data_ov006_0213c958;
extern short data_ov006_02141fd0;
extern short data_ov006_02141fd4;
extern unsigned short data_ov006_02141fdc[];

void func_ov006_020ebd7c(int count)
{
    int i, j;
    data_ov006_02141fd4 = (short)(RandomIntInternal(&RNG_STATE) & 0xf);
    data_ov006_02141fd0 = (short)count;
    if (count <= 0)
        return;
    for (i = 0; i < count; i++) {
        data_ov006_02141fdc[i] = (unsigned short)((int)((data_ov006_0213c958 * (int)((unsigned int)(RandomIntInternal(&RNG_STATE) & 0x7fffffff) >> 0x13))) >> 0xc);
        for (j = 0; j < i; j++) {
            if (data_ov006_02141fdc[j] == data_ov006_02141fdc[i]) {
                data_ov006_02141fdc[i] = (unsigned short)((int)((data_ov006_0213c958 * (int)((unsigned int)(RandomIntInternal(&RNG_STATE) & 0x7fffffff) >> 0x13))) >> 0xc);
                j = 0;
            }
        }
    }
}
