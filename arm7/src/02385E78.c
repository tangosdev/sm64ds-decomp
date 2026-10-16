/* storage 0x02385E78 runtime 0x037FDD10 size 0x28 wram ARM.
 * One byte at the cursor stored at a1+0x28, then advance the cursor. */

extern int fn_02385EA0(unsigned int addr);

int fn_02385E78(int a1) {
    int value = fn_02385EA0(*(unsigned int *)(a1 + 0x28));
    ++*(int *)(a1 + 0x28);
    return value;
}
