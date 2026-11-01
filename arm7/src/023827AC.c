/* storage 0x023827AC runtime 0x037FA644 size 0x64 wram ARM.
 * Fill node (flag=1), link via fn_023828BC, return fn_02382AD8(v).
 * No pool, three calls (fail handler is external fn_02382B38). */

extern void fn_02382B38(void);
extern int fn_02382AC4(void);
extern void fn_023828BC(int *a1);
extern int fn_02382AD8(int a1);

int fn_023827AC(int *a1, int a2, int a3, int a4, int a5) {
    int v;
    if (a1 == 0 || *a1 != 0)
        fn_02382B38();
    v = fn_02382AC4();
    a1[6] = 1;
    ((short *)a1)[6] = (short)a2;
    ((short *)a1)[7] = (short)a3;
    a1[0] = a4;
    a1[1] = a5;
    fn_023828BC(a1);
    return fn_02382AD8(v);
}
