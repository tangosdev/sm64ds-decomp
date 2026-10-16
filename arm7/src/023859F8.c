/* storage 0x023859F8 runtime 0x037FD890 linked 0xB8 wram ARM. */

extern int *dat_38075C0;
extern int fn_02384C1C(int voice);
extern void fn_0238592C(int voice);

int fn_023859F8(unsigned char *a1) {
    int count = 0;
    unsigned short *cursor = (unsigned short *)(a1 + 0x1c);
    int span;
    unsigned short *end;

    while (*(unsigned short *)(a1 + 0x1c) >= 0xF0) {
        *cursor -= 240;
        count++;
    }
    if (dat_38075C0 != 0)
        *(int *)((char *)dat_38075C0 + 0x40 + 36 * a1[2]) += count;
    while (count > 0) {
        if (fn_02384C1C((int)a1)) {
            fn_0238592C((int)a1);
            break;
        }
        count--;
    }
    span = *(unsigned short *)(a1 + 0x18) * *(unsigned short *)(a1 + 0x1a);
    end = (unsigned short *)(a1 + 0x1c);
    span = *end + (span >> 8);
    *end = span;
    return span;
}
