/* storage 0x02383450 runtime 0x037FB2E8 size 0x64 wram ARM.
 * Pools: =dat_38070B4 (x2), =0x7FFF. */

extern int dat_38070B4;

int fn_02383450(int result, int a2) {
    if (a2 < 24)
        return (result * (dat_38070B4 * (a2 + 40) + ((0x7FFF - dat_38070B4) << 6))) >> 21;
    if (a2 > 104)
        return (result * (-dat_38070B4 * (a2 - 40) + ((dat_38070B4 + 0x7FFF) << 6))) >> 21;
    return result;
}
