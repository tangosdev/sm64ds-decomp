typedef struct BitmapFind {
    int field0;
    int max;
    int result;
    int current;
    int pad10;
    unsigned int *bits;
} BitmapFind;

int func_0206470c(BitmapFind *self) {
    int idx = self->current + 1;
    int start;
    int bit;
    unsigned int *word;
    if (idx >= self->max) idx = 0;
    word = self->bits + (idx >> 5);
    start = idx;
    bit = idx & 0x1f;
    /* found/fail gotos kept. The ROM leaves the loop with a forward branch over the found
       tail to the fail tail, laid out in label order (0x98). Measured: returning from
       inside the loop, with a for, a break or a do-while, 0x90; breaking to the found
       tail with `return -1` in the loop, 0x94 (the fail return lands straight after
       the loop); while (*word & (1 << bit)) with the fail return inside, 0xa0. */
    for (;;) {
        if (!(*word & (1 << bit))) goto found;
        idx++;
        if (idx < self->max) {
            bit++;
            if (bit > 0x1f) {
                bit = 0;
                word++;
            }
        } else {
            idx = 0;
            bit = 0;
            word = self->bits;
        }
        if (idx == start) goto fail;
    }
found:
    self->result = idx;
    return idx;
fail:
    return -1;
}
