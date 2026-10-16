/* storage 0x0238BDCC runtime 0x03803C64 size 0x28 wram. No pool. */

extern void fn_02380A40(int);
extern int fn_02382AD8(int);

int fn_0238BDCC(int arg, int *state) {
    if (state[0] == 0)
        fn_02380A40(arg);
    return fn_02382AD8(state[1]);
}
