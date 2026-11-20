/* storage 0x02387184 runtime 0x037FF01C size 0x30 wram. Pool +0x30: 03808778. */

extern int dword_3808778;
extern int fn_023871C4(int);

int fn_02387184(int result) {
    if (dword_3808778 == 0) {
        dword_3808778 = 1;
        result = fn_023871C4(result);
    }
    return result;
}
