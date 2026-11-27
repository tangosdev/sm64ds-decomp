/* storage 0x023842F8 runtime 0x037FC190 size 0xC8 (listed 0xC0 + pool) wram ARM.
 * Channel sweep: for each set bit in a1 (16 channels), if the channel is not
 * yet allocated (*A4 bit clear), run the entry callback if present, release
 * via fn_02383634, clear fields, and mark byte 3 done. Returns last status.
 * Three pools, three calls (one indirect). The four zero-valued entities are
 * distinct source variables (copy-initialized); the 1 for (1 << i) is hoisted. */

extern int unk_38075C4[];
extern int unk_38075A4;
extern int fn_02383634(int a1, int a2);
extern int fn_023843C0(int *a1);

int fn_023842F8(int a1) {
    unsigned v1 = (unsigned)a1;
    unsigned char *v3;
    int i = 0;
    int cb_z = i;
    int sub_z = i;
    int s34_z = i;
    int s4_z = i;
    int v4;
    int result;
    for (i = 0; i < 16 && v1 != 0; ++i, v1 >>= 1) {
        result = (int)(v1 & 1);
        if (result != 0) {
            v3 = (unsigned char *)unk_38075C4 + 84 * i;
            result = unk_38075A4 & (1 << i);
            if (result == 0) {
                v4 = ((int *)v3)[18];
                if (v4 != 0)
                    ((void (*)(unsigned char *, int, int))v4)(v3, cb_z, ((int *)v3)[19]);
                fn_02383634(i, sub_z);
                v3[34] = (char)s34_z;
                fn_023843C0((int *)v3);
                v3[4] = (char)s4_z;
                {
                    unsigned char *b3 = v3 + 3;
                    result = (int)(*b3 & ~1);
                    *b3 = (unsigned char)result;
                }
            }
        }
    }
    return result;
}
