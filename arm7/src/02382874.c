/* storage 0x02382874 runtime 0x037FA70C size 0x48 (listed 0x44 + pool) wram ARM.
 * Head-insert into unk_3806FF0 list; empty-list path tail-calls fn_02382A0C.
 * One address pool, one call. */

extern int unk_3806FF0[];
extern int fn_02382A0C(int a1);

int fn_02382874(int *a1) {
    int *v2;
    a1[5] = 0;
    v2 = (int *)unk_3806FF0[1];
    unk_3806FF0[1] = (int)a1;
    a1[4] = (int)v2;
    if (v2 != 0) {
        v2[5] = (int)a1;
    } else {
        unk_3806FF0[1] = (int)a1;
        unk_3806FF0[0] = (int)a1;
        return fn_02382A0C((int)a1);
    }
    return (int)a1;
}
