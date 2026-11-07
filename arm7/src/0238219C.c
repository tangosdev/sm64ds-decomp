/* storage 0x0238219C runtime 0x037FA034 size 0x8C + pool 0x90 wram ARM.
 * Unlink a node from the doubly-linked queue, clear its timer words, rearm.
 * One pool data ref (dword_3806FE4), three calls. */

extern int fn_02382AC4(void);
extern int fn_02382AD8(int);
extern int fn_023824BC(int);
extern int dword_3806FE4[2];

int fn_0238219C(int *a1) {
    int v3 = fn_02382AC4();
    if (*a1 == 0)
        return fn_02382AD8(v3);
    {
        int v5 = a1[6];
        if (v5 == 0)
            dword_3806FE4[1] = a1[5];
        else
            *(int *)(v5 + 20) = a1[5];
        {
            int v6 = a1[5];
            if (v6 != 0) {
                *(int *)(v6 + 24) = v5;
            } else {
                dword_3806FE4[0] = v5;
                if (v5 != 0)
                    fn_023824BC(v5);
            }
        }
        a1[0] = 0;
        a1[7] = 0;
        a1[8] = 0;
        return fn_02382AD8(v3);
    }
}
