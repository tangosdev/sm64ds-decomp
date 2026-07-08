struct Hdr84 {
    char pad[8];
    int count;
};

extern struct { char pad[0x7c]; struct Hdr84 *f7c; } *_ZN5Sound8SDAT_RAM3PTRE;

int func_020509b0(unsigned int i) {
    struct Hdr84 *h = _ZN5Sound8SDAT_RAM3PTRE->f7c;
    if (i >= (unsigned int)h->count) return 0;
    return *(int *)((char *)h + i * 16 + 0x14);
}
