/* storage 0x0239401C runtime 0x027C5F94 size 0x38 (0x28 + 0x10 pool) main.
 * Three MMIO halfword stores (0x0480xxxx are numeric pool constants,
 * not relocations) and returns the last MMIO address. */

void *fn_0239401C(void) {
    *(volatile unsigned short *)0x04808124 = 200;
    *(volatile unsigned short *)0x04808128 = 2000;
    *(volatile unsigned short *)0x04808150 = 0x202;
    return (void *)0x04808150;
}
