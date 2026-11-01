/* storage 0x0238322C runtime 0x037FB0C4 size 0x1C wram ARM.
 * Set-bits halfword store to 0x4000134. One numeric pool word. */

int fn_0238322C(int a1, int a2) {
    int r = a2 | (~a1 & *(unsigned short *)0x4000134);
    *(unsigned short *)0x4000134 = (unsigned short)r;
    return r;
}
