// NONMATCHING: register allocation (div=9). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;

extern void func_ov006_020f1dbc(void);
extern int data_ov006_0212e8c8[];
extern s16 SINE_TABLE[];

void func_ov006_020f1cb4(char *r0, int r1)
{
    u8 *flag = (u8*)(r0 + 0x5275 + r1);
    int idx, k, t, mat;
    if (*flag == 0) {
        *(u16*)(r0 + r1 * 2 + 0x4f7c) = 0x6000;
        *flag = *flag + 1;
        return;
    }
    idx = *(u16*)(r0 + r1 * 2 + 0x4f7c) >> 4;
    k = *(u8*)(r0 + 0x5365 + r1);
    t = SINE_TABLE[idx * 2 + 1];
    mat = data_ov006_0212e8c8[k];
    *(int*)(r0 + 0x47f8 + r1 * 4) = *(int*)(r0 + 0x47f8 + r1 * 4)
        + (int)(((s64)t * mat + 0x800) >> 12);
    idx = *(u16*)(r0 + r1 * 2 + 0x4f7c) >> 4;
    k = *(u8*)(r0 + 0x5365 + r1);
    t = SINE_TABLE[idx * 2];
    mat = data_ov006_0212e8c8[k];
    *(int*)(r0 + 0x49d8 + r1 * 4) = *(int*)(r0 + 0x49d8 + r1 * 4)
        + (int)(((s64)t * mat + 0x800) >> 12);
    func_ov006_020f1dbc();
}
