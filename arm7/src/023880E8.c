/* storage 0x023880E8 runtime 0x037FFF80 size 0x1C wram ARM.
 * Call fn_02387DB0(a1), store the result into (*a1)[2], return it. */

extern int fn_02387DB0(int **);

int fn_023880E8(int **a1) {
    int r = fn_02387DB0(a1);
    (*a1)[2] = r;
    return r;
}
