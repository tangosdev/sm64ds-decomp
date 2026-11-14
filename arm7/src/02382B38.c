/* storage 0x02382B38 runtime 0x037FA9D0 size 0x14 wram ARM. No pool, two calls. */

extern void fn_02382AC4(void);
extern void fn_02380594(void);

void fn_02382B38(void) {
    for (;;) {
        fn_02382AC4();
        fn_02380594();
    }
}
