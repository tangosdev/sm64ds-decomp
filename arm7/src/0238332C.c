/* storage 0x0238332C runtime 0x037FB1C4 size 0x34 wram ARM.
 * Sound-control byte build at 0x4000501. One numeric pool word. */

void *fn_0238332C(int a1, int a2, int a3, int a4) {
    int v = (((*(unsigned char *)0x4000501 & 0x80) != 0) << 7) | (a4 << 5) | (a3 << 4) | (a2 << 2) | a1;
    *(unsigned char *)0x4000501 = (unsigned char)v;
    return (void *)0x4000501;
}
