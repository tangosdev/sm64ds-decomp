/* storage 0x023813EC runtime 0x037F9284 size 0x30 + 1 pool word, wram ARM.
 * First-zero scan of 16 words at table+0x14; index or -1. */

struct Tbl {
    int pad[5];
    int slot[16];
};

extern struct Tbl word_3806E60;

int fn_023813EC(void) {
    int i;
    for (i = 0; i < 16; i++) {
        if (word_3806E60.slot[i] == 0)
            return i;
    }
    return -1;
}
