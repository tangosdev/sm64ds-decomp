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
    for (;;) {
        if (!(*word & (1 << bit))) goto found;
        idx++;
        if (idx >= self->max) goto wrap;
        bit++;
        if (bit > 0x1f) {
            bit = 0;
            word++;
        }
        goto check;
    wrap:
        idx = 0;
        bit = 0;
        word = self->bits;
    check:
        if (idx == start) goto fail;
    }
found:
    self->result = idx;
    return idx;
fail:
    return -1;
}
