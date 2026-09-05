/*
 * ov006 minigame: per-slot tick for one of the tracked entries (idx).
 * Bumps the global tick byte at +0x4fe9.  If the slot's hold counter
 * (+0x4e78[idx]) is running, count it down and clamp at zero; otherwise
 * advance the slot's frame counter (+0x4e30[idx]) and, once it reaches
 * the limit (4 while the +0x4f8a flag is set, else 8), reset it, step the
 * slot's phase (+0x4de8[idx]) and wrap the phase past 5 with the +0x4efa
 * flag cleared.
 *
 * Shape notes: the pool-constant + register-offset form is what the
 * RMW sites (`*p = *p +- 1`) produce, the split base+0xNN00/#off form is
 * what the plain reads produce, and opt_common_subs off keeps the
 * else-branch recomputing base+idx*2 (its own extended basic block) the
 * way the ROM does.  The frame counter must be bumped before the flag
 * byte is read, and the limit compare is unsigned.
 */
#pragma opt_common_subs off
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

void func_ov006_02106aa8(char *base, int idx)
{
    u8 *cnt = (u8 *)(base + 0x4fe9);

    *cnt = *cnt + 1;

    if (*(u16 *)(base + idx * 2 + 0x4e78) != 0) {
        u16 *q = (u16 *)(base + 0x4e78 + idx * 2);
        *q = *q - 1;
        if (*(s16 *)(base + idx * 2 + 0x4e78) < 0) {
            *(u16 *)(base + idx * 2 + 0x4e78) = 0;
        }
    } else {
        u16 *r = (u16 *)(base + 0x4e30 + idx * 2);
        u8 flag;
        unsigned int lim;
        *r = *r + 1;
        flag = *(u8 *)(base + idx + 0x4000 + 0xf8a);
        lim = (flag != 0) ? 4 : 8;
        if (*(u16 *)(base + idx * 2 + 0x4e30) < lim) {
            return;
        }
        *(u16 *)(base + idx * 2 + 0x4e30) = 0;
        {
            u16 *s = (u16 *)(base + 0x4de8 + idx * 2);
            *s = *s + 1;
        }
        if (*(u16 *)(base + idx * 2 + 0x4d00 + 0xe8) >= 5) {
            *(u16 *)(base + idx * 2 + 0x4d00 + 0xe8) = 0;
            *(u8 *)(base + idx + 0x4000 + 0xefa) = 0;
        }
    }
}
