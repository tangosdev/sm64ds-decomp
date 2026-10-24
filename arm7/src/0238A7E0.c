/* storage 0x0238A7E0 runtime 0x03802678 size 0x118 wram ARM.
 * Channel-select rewrite on a packed control word: field idx (bits 8-13)
 * picks the case, bit 14 is the enable flag. Cases 0-7/16-17 rewrite idx
 * to idx+18 when the flag is set, cases 8-9 to idx+24, cases 18-25/32-35
 * set the flag, default clears the flag and idx. The asymmetric shift
 * extracts codegen as literal LSL/LSR pairs; everything stays in
 * registers (no frame). No calls, no pool (jumptable only). */

int *fn_0238A7E0(int *result) {
    unsigned int v1 = (unsigned int)*result;
    unsigned int v2 = (v1 << 18) >> 26;
    switch (v2) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 16:
    case 17:
        if (((v1 << 17) >> 31) == 0)
            return result;
        *result = (int)(v1 & ~0x3F00u) | (int)(((v2 + 18) & 0x3F) << 8);
        return result;
    case 8:
    case 9:
        if (((v1 << 17) >> 31) == 0)
            return result;
        *result = (int)(v1 & ~0x3F00u) | (int)(((v2 + 24) & 0x3F) << 8);
        return result;
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 32:
    case 33:
    case 34:
    case 35:
        *result = (int)(v1 | 0x4000u);
        return result;
    default:
        *result &= ~0x4000;
        *result &= ~0x3F00;
        break;
    }
    return result;
}
