#pragma exceptions off

/* storage 0x02384C1C runtime 0x037FCAB4 size 0x7C wram ARM.
 * Calls fn_023859CC, fn_02384C98 (this pack), fn_02385968. No pool. */

extern int fn_023859CC(int, int);
extern int fn_02384C98(int, int);
extern void fn_02385968(int, int);

int fn_02384C1C(int a1) {
    int v2 = 0;
    int i;
    for (i = 0; i < 16; ++i) {
        int v4 = fn_023859CC(a1, i);
        if (v4 != 0 && *(int *)(v4 + 40) != 0) {
            if (fn_02384C98(v4, a1) != 0)
                fn_02385968(a1, i);
            else
                v2 = 1;
        }
    }
    return v2 == 0;
}
