/* storage 0x02384200 runtime 0x037FC098 size 0xF8 (listed 0xEC + pool) wram ARM.
 * Channel allocator: for each set bit in a1 (16 channels), if the channel is
 * not yet allocated (*A4 bit clear), run the entry callback if present,
 * release via fn_02383634, clear fields, and mark byte 3 done. Then set the
 * a1 bits into A0 (a2 odd) or A4 (a2 even); returns the touched word address.
 * Three pools, three calls (one indirect). Same four-zero-var shape as
 * fn_023842F8; the extra saved a2 spills two of them to the stack. */

extern int unk_38075C4[];
extern int unk_38075A4;
extern int unk_38075A0;
extern int fn_02383634(int a1, int a2);
extern int fn_023843C0(int *a1);

int *fn_02384200(unsigned a1, int a2) {
    unsigned char *v6;
    unsigned v4 = a1;
    int i = 0;
    int cb_z = i;
    int sub_z = i;
    int s34_z = i;
    int s4_z = i;
    int v7;
    int result;
    int *ret;
    for (i = 0; i < 16 && v4 != 0; ++i, v4 >>= 1) {
        result = (int)(v4 & 1);
        if (result != 0) {
            v6 = (unsigned char *)unk_38075C4 + 84 * i;
            result = unk_38075A4 & (1 << i);
            if (result == 0) {
                v7 = ((int *)v6)[18];
                if (v7 != 0)
                    ((void (*)(unsigned char *, int, int))v7)(v6, cb_z, ((int *)v6)[19]);
                fn_02383634(i, sub_z);
                v6[34] = (unsigned char)s34_z;
                fn_023843C0((int *)v6);
                v6[4] = (unsigned char)s4_z;
                {
                    unsigned char *b3 = v6 + 3;
                    result = (int)(*b3 & ~1);
                    *b3 = (unsigned char)result;
                }
            }
        }
    }
    if ((a2 & 1) != 0) {
        ret = &unk_38075A0;
        *ret |= (int)a1;
    } else {
        ret = &unk_38075A4;
        *ret |= (int)a1;
    }
    return ret;
}
