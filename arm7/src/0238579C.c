/* storage 0x0238579C runtime 0x037FD634 size 0x44 wram ARM. No pool. */

extern int fn_023859CC(int a1, int a2);
extern int fn_023857E0(int result, int a2);

int fn_0238579C(int a1) {
    int i;
    int result;

    for (i = 0; i < 16; i++) {
        result = fn_023859CC(a1, i);
        if (result != 0)
            result = fn_023857E0(result, a1);
    }
    return result;
}
