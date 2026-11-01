/* storage 0x02381E94 runtime 0x037F9D2C size 0x18 wram ARM.
 * Set bit a1 in the halfword at 0x03806FCC. One data pool word. */

extern unsigned short word_3806FCC;

int fn_02381E94(int a1) {
    int r = word_3806FCC | (1 << a1);
    word_3806FCC = (unsigned short)r;
    return r;
}
