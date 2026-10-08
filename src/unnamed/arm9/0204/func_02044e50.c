struct Sub {
    unsigned char *bytes;
    unsigned short *halfs;
    char pad8[4];
    char *blk;
};
struct Self {
    struct Inner *p0;
    char pad[8];
    char *m;
};
struct Inner {
    char pad[0x34];
    struct Sub *sub;
};

extern void func_02047218(void *a, void *b, void *out, int c);

void func_02044e50(struct Self *self, int count, int base, int *out)
{
    int i;
    struct Inner *in = self->p0;
    out[0] = 0; out[1] = 0; out[2] = 0; out[3] = 0;
    out[4] = 0; out[5] = 0; out[6] = 0; out[7] = 0;
    out[8] = 0; out[9] = 0; out[10] = 0; out[11] = 0;
    for (i = 0; i < count; i++) {
        struct Sub *sub = in->sub;
        int idx = base + i;
        unsigned short lh = sub->halfs[idx];
        unsigned char b = sub->bytes[idx];
        func_02047218(self->m + lh * 0x30, sub->blk + lh * 0x30, out, b << 4);
    }
}
