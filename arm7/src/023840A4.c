/* storage 0x023840A4 runtime 0x037FBF3C size 0x70 wram ARM. No pool, no calls.
 * Counter/accumulator: if halfword[3] < halfword[2], increment it and return.
 * Else fold byte[1]<<6 into halfword[4]: reduce ((h[4]+(b<<6))>>8) modulo
 * 0x80, add the shift back, keep the low byte, OR the reduced value into the
 * high byte; return the combined halfword. The three halfword updates are
 * separate read-modify-write statements (each reloads), and only the final
 * store's value is returned via a plain unsigned temp (returning the short
 * lvalue itself would emit a narrowing reload). The h[4] snapshot is volatile:
 * this live audio state is also touched in IRQ context, and the qualifier is
 * what pins the LDRH ahead of the LDRB in the issue order. */

int fn_023840A4(void *a1) {
    unsigned short *h = (unsigned short *)a1;
    unsigned char *b = (unsigned char *)a1;
    if (h[3] < h[2]) {
        unsigned short *v1 = &h[3];
        int r = (int)*v1 + 1;
        *v1 = (unsigned short)r;
        return r;
    }
    {
        unsigned h8 = *(volatile unsigned short *)&h[4];
        unsigned b1 = b[1];
        unsigned r3 = b1 << 6;
        unsigned r1 = h8 + (b1 << 6);
        unsigned r2 = r1 >> 8;
        unsigned short *hp;
        while (r2 >= 0x80)
            r2 -= 0x80;
        hp = &h[4];
        *hp = (unsigned short)(*hp + r3);
        *hp = (unsigned short)(*hp & 0xFF);
        r1 = (unsigned)*hp | (r2 << 8);
        *hp = (unsigned short)r1;
        return (int)r1;
    }
}
