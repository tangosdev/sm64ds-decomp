/* storage 0x0238600C runtime 0x037FDEA4 linked 0x5C wram ARM.
 * Release every live track whose word at +0x20 lies in [lo, hi].
 * Pool: unk_3807B04. */

extern unsigned char unk_3807B04[];
extern int fn_0238592C(char *entry);

void fn_0238600C(unsigned int lo, unsigned int hi) {
    int i;
    for (i = 0; i < 16; i++) {
        unsigned char *entry = unk_3807B04 + 36 * i;
        if (*entry != 0) {
            unsigned int v = *(unsigned int *)(entry + 0x20);
            if (lo <= v && v <= hi)
                fn_0238592C((char *)entry);
        }
    }
}
