
typedef long long s64;
typedef struct {
    char _pad[0x734];
    int unk734;
    int unk738;
} Arg0;
typedef struct {
    int unk0, unk4, unk8, unkC;
} Arg1;
static inline int FX(int a, int b) {
    return (int)(((s64)(a) * (b) + 0x800) >> 12);
}

void func_ov002_020c3160(Arg0 *self, Arg1 *out)
{
    int t = self->unk734;
    int u = 0x1000 - t;
    int u2 = FX(u, u);
    int u3 = FX(u2, u);
    int t2 = FX(t, t);
    int t3 = FX(t2, t);

    switch (self->unk738) {
    case 0: break;
    case 1: {
        int a = FX(t, 0x3000);
        int b = FX(t3, 0x1c00);
        int c = FX(t2, 0x4800);
        out->unk0 = u3;
        out->unk4 = a + (b - c);
        out->unk8 = FX(t2, 0x1800) - FX(t3, 0xeaa);
        out->unkC = FX(t3, 0x2aa);
        break;
    }
    case 2: {
        int k1400 = 0x1400;
        s64 p955 = (s64)t3 * 0x955;
        s64 p1400 = (s64)t2 * k1400;
        out->unk0 = FX(u3, 0x400);
        {
            int b = FX(t, 0x400);
            int c = (int)((p955 + 0x800) >> 12);
            int d = (int)((p1400 + 0x800) >> 12);
            out->unk4 = b + (c - d) + 0x955;
        }
        out->unk8 = FX(t, 0x800) + (FX(t2, 0x800) - FX(t3, 0x800)) + 0x2aa;
        out->unkC = FX(t3, 0x2aa);
        break;
    }
    case 3: {
        out->unk0 = FX(u3, 0x2aa);
        out->unk4 = FX(t3, 0x800) - t2 + 0xaaa;
        out->unk8 = FX(t, 0x800) + (FX(t2, 0x800) - FX(t3, 0x800)) + 0x2aa;
        out->unkC = FX(t3, 0x2aa);
        break;
    }
    case 4: {
        out->unk0 = FX(u3, 0x2aa);
        out->unk4 = FX(u, 0x800) + (FX(u2, 0x800) - FX(u3, 0x800)) + 0x2aa;
        out->unk8 = FX(u, 0x400) + (FX(u3, 0x955) - FX(u2, 0x1400)) + 0x955;
        out->unkC = FX(t3, 0x400);
        break;
    }
    case 5: {
        out->unk0 = FX(u3, 0x2aa);
        out->unk4 = FX(u2, 0x1800) - FX(u3, 0xeaa);
        out->unk8 = FX(u, 0x3000) + (FX(u3, 0x1c00) - FX(u2, 0x4800));
        out->unkC = t3;
        break;
    }
    }
}
