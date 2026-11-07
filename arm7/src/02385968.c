/* storage 0x02385968 runtime 0x037FD800 size 0x54 wram ARM.
 * Pool word is dat_3807D44. */

extern int fn_023859CC(int a1, int a2);
extern int fn_023859BC(int a1, int a2);
extern unsigned char dat_3807D44[];

int fn_02385968(int a1, int a2) {
    int result;
    unsigned char slot;

    result = fn_023859CC(a1, a2);
    if (result != 0) {
        fn_023859BC(result, a1);
        slot = *(unsigned char *)(a1 + 8 + a2);
        dat_3807D44[slot << 6] &= ~1;
        *(unsigned char *)(a1 + 8 + a2) = 255;
        return 255;
    }
    return result;
}
