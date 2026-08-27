struct Entry {
    char p0[2];
    unsigned char b2;
    char p3[1];
    short f4;
    short f6;
    char p8[2];
    short fa;
    char rest[0x68 - 0xc];
};

struct Self {
    int f0;
    int f4;
    unsigned short f8;
    unsigned short fa;
    unsigned char fc;
    unsigned char fd;
    char p0e[2];
    void (*f10)(struct Self *);
    short f14;
    short f16;
    short f18;
    char pad[0x1d4 - 0x1a];
    struct Entry table[1];
};

void func_02064470(struct Self *self, int r1, int r2)
{
    struct Entry *e = &self->table[r2];
    unsigned short mask = 1 << r2;

    self->fa = mask;
    if (self->fc != e->b2) return;
    self->fd = e->b2;
    if ((self->f8 & mask) == 0) goto after;
    {
        unsigned short *p8 = (unsigned short *)(((int)self + 8));
        *p8 = *p8 & ~mask;
    }

    switch (r1) {
    case 0: case 1: case 2: case 4:
    case 5: case 6: case 7: case 8:
    case 9: case 10: case 11:
        break;
    case 3:
        self->f14 = e->fa;
        self->f16 = e->f6;
        self->f18 = e->f4;
        break;
    }

    self->f4 = r1;
    if (self->f10 != 0) {
        self->f10(self);
    }
after:
    if (self->f8 == 0) self->f0 = 0;
}
