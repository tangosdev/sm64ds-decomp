/* storage 0x02386260 runtime 0x037FE0F8 linked 0x60 wram ARM.
 * Release track index and clear its bit in the word at dword_38075C0+4.
 * Pool: unk_3807B04, dword_38075C0. */

extern unsigned char unk_3807B04[];
extern int fn_0238592C(char *entry);
extern int *dword_38075C0;

void fn_02386260(int index) {
    unsigned char *entry = unk_3807B04 + 36 * index;
    if (*entry != 0) {
        fn_0238592C((char *)entry);
        if (dword_38075C0 != 0)
            dword_38075C0[1] &= ~(1 << index);
    }
}
