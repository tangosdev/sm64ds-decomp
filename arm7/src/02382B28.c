/* storage 0x02382B28 runtime 0x037FA9C0 size 0xC + 4 pool wram ARM.
 * Tail call through the Thumb entry at 0x037FFFCD (pool word). */

extern int thumb_37FFFCD;

void fn_02382B28(unsigned int a1) {
    ((void (*)(unsigned int))&thumb_37FFFCD)(a1 >> 2);
}
