struct S82 {
    char pad0[0x54];
    unsigned char *f54;
    unsigned char *f58;
};

int func_0204ede8(struct S82 *o, unsigned int i) {
    unsigned char *base = o->f54;
    int r = 0;
    if (i < *(unsigned short *)(base + 8)) {
        r = (int)o->f58 + *(int *)(base + i * 8 + 0xc);
    }
    return r;
}
