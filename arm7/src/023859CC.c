/* storage 0x023859CC runtime 0x037FD864 size 0x2C wram ARM.
 * Pool word is dat_3807D44. */

extern unsigned char dat_3807D44[];

unsigned char *fn_023859CC(int a1, int a2) {
    int slot;

    if (a2 > 15)
        return 0;
    slot = *(unsigned char *)(a1 + a2 + 8);
    if (slot == 255)
        return 0;
    return &dat_3807D44[slot << 6];
}
