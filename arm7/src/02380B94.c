/* storage 0x02380B94 runtime 0x037F8A2C size 0x84 wram ARM.
 * Lock (fn_02382AF0/fn_02382AC4 by a4), look up (a1,a2) via
 * fn_02382EF0; on a miss fire the a3 callback then stamp the
 * halfword at a2+4; unlock and return the lookup result. */

extern int fn_02382AF0(void);
extern int fn_02382AC4(void);
extern int fn_02382EF0(int, char *);
extern void fn_02382B04(int);
extern void fn_02382AD8(int);

int fn_02380B94(int a1, char *a2, int a3, int a4) {
    int v8, v9;
    if (a4 != 0)
        v8 = fn_02382AF0();
    else
        v8 = fn_02382AC4();
    v9 = fn_02382EF0(a1, a2);
    if (v9 == 0) {
        if (a3 != 0)
            ((void (*)(void))a3)();
        *(unsigned short *)(a2 + 4) = a1;
    }
    if (a4 != 0)
        fn_02382B04(v8);
    else
        fn_02382AD8(v8);
    return v9;
}
