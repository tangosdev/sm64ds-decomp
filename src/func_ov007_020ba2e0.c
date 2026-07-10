// Pokes the object referenced by the global pointer data_ov007_02104ba0:
// calls func_ov007_020c5dec on its first field, sets +0x30=1, +0x40=0x1000,
// (+0x44)->0x18=0x800, mirrors that into +0x3c, increments +0x24, and writes
// 5 into the halfword at (+0x08)+2.
typedef struct Sub {
    int pad[6];
    int x18;
} Sub;

typedef struct Hdr {
    short f0;
    short f2;
} Hdr;

typedef struct G {
    void *f0;
    int f4;
    Hdr *f8;
    int f0c;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    int x24;
    int f28;
    int f2c;
    int x30;
    int f34;
    int f38;
    int x3c;
    int x40;
    Sub *x44;
} G;

extern G *data_ov007_02104ba0;
extern void func_ov007_020c5dec(void *c);

void func_ov007_020ba2e0(void)
{
    func_ov007_020c5dec(data_ov007_02104ba0->f0);
    data_ov007_02104ba0->x30 = 1;
    data_ov007_02104ba0->x40 = 0x1000;
    data_ov007_02104ba0->x44->x18 = 0x800;
    data_ov007_02104ba0->x3c = data_ov007_02104ba0->x44->x18;
    (*(int *)((long long)(int)((char *)data_ov007_02104ba0 + 0x24) & 0xFFFFFFFFFFFFFFFFLL)) += 1;
    data_ov007_02104ba0->f8->f2 = 5;
}
