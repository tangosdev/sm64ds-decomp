struct Mgr {
    char pad0[8];
    int flags;                 /* 0x8 */
    char pad1[0x466c - 0xc];
    int count;                 /* 0x466c */
    char pad2[0x4720 - 0x4670];
    int *arr[1];               /* 0x4720 */
};

struct Obj {
    char pad0[4];
    struct Mgr *mgr;           /* 0x4 */
    int x;                     /* 0x8 */
    int y;                     /* 0xc */
    char pad1[0x28 - 0x10];
    int field28;               /* 0x28 */
    char pad2[0x30 - 0x2c];
    unsigned char b30;         /* 0x30 */
    char pad3[0x3a - 0x31];
    unsigned char b3a;         /* 0x3a */
    char pad4[0x121 - 0x3b];
    unsigned char b121;        /* 0x121 */
};

extern int func_ov006_021115c4(unsigned char *p);
extern void func_ov006_0211470c(int *a, int *b);
extern long long func_0203d5bc(int *p);
extern void func_ov004_020b0104(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern void *data_ov006_021382e0[];
extern void *data_ov006_021382f8[];

void func_ov006_02113c14(struct Obj *c)
{
    long long sq;
    int v;
    int i;
    int s[6];

    if (c->b30 == 0) return;
    if (c->b121 == 0) return;
    if (c->b3a == 1) return;

    if ((c->mgr->flags & 0xff) != 0) {
        int x = c->x;
        if (x >= 0x40000 && x < 0xa0000) {
            int y = c->y;
            if (y >= -0xb5000 && y < -0x85000) return;
        }
        v = c->field28 + 0x15000;
        sq = (int)(((long long)v * v + 0x800) >> 12);
        for (i = 0; i < c->mgr->count; i++) {
            if (func_ov006_021115c4((unsigned char *)((i >= c->mgr->count) ? 0 : c->mgr->arr[i])) == 0) {
                s[0] = c->x;
                s[1] = c->y;
                s[1] += 0x5000;
                func_ov006_0211470c(&s[4], (i >= c->mgr->count) ? 0 : c->mgr->arr[i]);
                s[0] = s[0] - s[4];
                s[1] = s[1] - s[5];
                s[1] = s[1] * 2;
                if (func_0203d5bc(&s[0]) < sq) return;
            }
        }
    }

    if ((c->mgr->flags & 0xff) == 0)
        func_ov004_020b0104(data_ov006_021382e0[1], (c->x + 0x800) >> 12, ((c->y + 0x800) >> 12) + 5, -1, 2, 0);
    else
        func_ov004_020b0104(data_ov006_021382f8[1], (c->x + 0x800) >> 12, ((c->y + 0x800) >> 12) + 5, -1, 2, 0);
}
