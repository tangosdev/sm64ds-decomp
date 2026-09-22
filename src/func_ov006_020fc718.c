// Resets one row of the ov006 table (rows are 0x1c bytes apart) after rolling a
// random kind from data_ov006_0212eb2c. The row base `thiz + off` is spelled twice
// on purpose: the ROM computes it once for the +0x4000 stores and again, right after
// the kind reload and before the predicated if/else, for the tail stores. With
// common-subexpression elimination on, mwccarm folds the second into the first and
// sinks the add below the if/else, which recolours the whole tail.
#pragma opt_common_subs off
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
    q = thiz + off;
    if (p[off] == 2) {
        *(unsigned char*)(q + 0x4000 + 0xeb9) = 1;
        *(int*)(q + 0x4000 + 0xea8) = 0x1000;
    } else {
        *(unsigned char*)(q + 0x4000 + 0xeb9) = 0;
        *(int*)(q + 0x4000 + 0xea8) = -0x1000;
    }
    *(short*)(thiz + off + 0x4e00 + 0xb0) = 0;
    *(unsigned char*)(thiz + off + 0x4000 + 0xeb6) = 0;
    *(unsigned char*)(thiz + off + 0x4000 + 0xeb8) = 1;
}
