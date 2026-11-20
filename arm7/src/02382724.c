/* storage 0x02382724 runtime 0x037FA5BC size 0x88 (listed 0x84 + pool) wram ARM.
 * Unlink node from unk_3806FF0 list; empty-slot path tail-calls fn_02382AD8(v).
 * One address pool, three calls. */

extern int unk_3806FF0[];
extern int fn_02382AC4(void);
extern int fn_02382AD8(int a1);
extern int fn_02382A0C(int a1);

int fn_02382724(int *a1) {
    int v3 = fn_02382AC4();
    if (a1[0] == 0)
        return fn_02382AD8(v3);
    {
        int *v5 = (int *)a1[5];
        if (v5 == 0)
            unk_3806FF0[1] = a1[4];
        else
            v5[4] = a1[4];
        {
            int *v6 = (int *)a1[4];
            if (v6 != 0)
                v6[5] = (int)v5;
            else {
                unk_3806FF0[0] = (int)v5;
                if (v5 != 0)
                    fn_02382A0C((int)v5);
            }
        }
    }
    a1[0] = 0;
    return fn_02382AD8(v3);
}
