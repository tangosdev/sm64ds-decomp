/* storage 0x0239D3A4 runtime 0x027CF31C size 0x28 (code 0x24 + pool) main ARM.
 * Store 2 at a2+2, copy *(unk_380FFF4+784) to a2+6, return 0. Leaf, no stack.
 * One pool word (unk_380FFF4), no calls. */
extern unsigned int unk_380FFF4 /* =0x0380FFF4 */;

int fn_0239D3A4(int a1, int a2) {
    *(unsigned short *)(a2 + 2) = 2;
    *(unsigned short *)(a2 + 6) = *(unsigned short *)(unk_380FFF4 + 784);
    return 0;
}
