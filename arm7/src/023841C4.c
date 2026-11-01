/* storage 0x023841C4 runtime 0x037FC05C size 0x3C (listed 0x34 + pool) wram ARM.
 * Mask-store to one of two words: a2 odd clears into 38075A0, a2 even into
 * 38075A4 (both `*pool &= ~a1`). The compiler lays the even block first and
 * predicates it EQ, so the odd test reads `!= 0`. Two pools, no calls. */

extern int unk_38075A4;
extern int unk_38075A0;

int fn_023841C4(int a1, int a2) {
    if ((a2 & 1) != 0)
        return (unk_38075A0 &= ~a1);
    return (unk_38075A4 &= ~a1);
}
