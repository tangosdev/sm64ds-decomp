/* storage 0x023828BC runtime 0x037FA754 size 0xE0 (listed 0xD8 + pool) wram ARM.
 * Sorted insert by key fn_02382A80(key(a1)-k0). Returns void (callers ignore r0;
 * the values r0 happens to hold at the exits are incidental). The loop is
 * `for (i=head; i!=0;)` with the advance at the bottom: `if (less==0) goto adv`
 * hops over the splice (which itself must `goto end` past the head block), and
 * the bottom falls back into the top check with no branch. The less-than feeds
 * a named bool, which is what materializes the MOVLT/MOVGE pair. Two pools,
 * three calls. */

extern int unk_3806FF0[];
extern int fn_02382A80(int a1);
extern void fn_02382A0C(int a1);

void fn_023828BC(int *a1) {
    unsigned v2 = *(volatile unsigned short *)0x4000006;
    int *i;
    int v4;
    for (i = (int *)unk_3806FF0[0]; i != 0; ) {
        v4 = fn_02382A80(((short *)a1)[6] - (int)v2);
        {
            int less = v4 < fn_02382A80(((short *)i)[6] - (int)v2);
            if (less == 0)
                goto adv;
        }
        a1[4] = i[4];
        i[4] = (int)a1;
        a1[5] = (int)i;
        {
            int *nx = (int *)a1[4];
            if (nx != 0) {
                nx[5] = (int)a1;
                goto end;
            }
        }
        unk_3806FF0[0] = (int)a1;
        fn_02382A0C((int)a1);
        goto end;
    adv:
        i = (int *)i[5];
    }
    {
        int *v6;
        a1[5] = 0;
        v6 = (int *)unk_3806FF0[1];
        unk_3806FF0[1] = (int)a1;
        a1[4] = (int)v6;
        if (v6 != 0) {
            v6[5] = (int)a1;
            goto end;
        } else {
            unk_3806FF0[1] = (int)a1;
            unk_3806FF0[0] = (int)a1;
            fn_02382A0C((int)a1);
        }
    }
end: ;
}
