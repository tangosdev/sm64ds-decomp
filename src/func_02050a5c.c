struct Hdr84 {
    char pad[8];
    int count;
};

extern struct { char pad[0x7c]; struct Hdr84 *f7c; } *data_020a5bb8;

int func_02050a5c(unsigned int i) {
    struct Hdr84 *h = data_020a5bb8->f7c;
    if (i >= (unsigned int)h->count) return 0;
    return *(int *)((char *)h + i * 16 + 0x10);
}
