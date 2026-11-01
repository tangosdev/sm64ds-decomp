/* storage 0x02385BC8 runtime 0x037FDA60 size 0x14 wram ARM. No pool.
 * The end bound is read back after the store. */

int fn_02385BC8(int result, int a2, int a3) {
    ((volatile int *)result)[9] = a2;
    *(int *)(result + 0x28) = ((volatile int *)result)[9] + a3;
    return result;
}
