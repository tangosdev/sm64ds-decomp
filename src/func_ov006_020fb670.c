// NONMATCHING: div=8 — short-web-first coloring inversion (wall family: mask/val vs cnt):
// ROM colors long web cnt=r1 before short pool-mask web (r2); mwcc always colors the short
// mask web first (r1), pushing cnt to r2. AT-launder fixed the 0x5c34 tail RMW and i/p mov
// order + decl order fixed p=r3/i=ip; remaining 8 diffs are the pure r1<->r2 swap. Tried:
// decl-order permutations, def-position moves, statement split, #pragma opt_propagation off
// (regressed), cnt/q variable merge, C++ frontend — all inert. Same wall as func_02058568.
#define AT(p, off) ((void *)(int)(((long long)(int)((char *)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020fb670(char *obj)
{
    int i;
    unsigned int r;
    char *p;
    int cnt;
    if (*(unsigned char *)(obj + 0x5000 + 0xc34) != 0) return;
    *(unsigned char *)(obj + 0x5000 + 0xbc6) = 1;
    *(unsigned char *)(obj + 0x5000 + 0xbc8) = 0;
    *(int *)(obj + 0x5000 + 0xbc0) = 0;
    r = (unsigned int)RandomIntInternal(&data_0209d4b8);
    cnt = 0;
    r = (r >> 16) & 0x7fff;
    *(short *)(obj + 0x5b00 + 0xc4) = (short)(((r << 5) >> 15 << 3) + 0x200);
    i = 0;
    p = obj;
    for (; i < 4; i++) {
        if (*(unsigned char *)(p + 0x5000 + 0xbd4) != 0) cnt++;
        p += 0xc;
    }
    if (cnt == 1) {
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        *(short *)(obj + 0x5b00 + 0xc4) = (short)(((r << 5) >> 15 << 2) + 0x200);
    }
    cnt = (int)AT(obj, 0x5c34);
    *(unsigned char *)cnt = *(unsigned char *)cnt + 1;
}
