#pragma opt_common_subs off
extern void *fn_0238E688(void);
extern void fn_023817E0(int a1);
extern int fn_0238176C(int a1);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

typedef struct {
    unsigned short h[6];
    int d[3];
} Rec;

typedef struct {
    unsigned short head, tail;
} Pair;

typedef struct {
    unsigned short mode;
    char pad0[0x84];
    unsigned short f43;
    char pad1[0x2F4 - 0x88];
    Rec recs[64];
    unsigned short free_head, free_tail;
    Pair in[4], out[4];
    int lock[5];
    int flag;
} Work;

int fn_02393128(int a1, int a2) {
    Work *v2;
    Rec *v3;
    int v8;
    Pair *v10;
    int i;
    Rec *v13;
    int v11;
    int v4, v5, v9, m;
    int v20, v21;
    unsigned short v22;
    unsigned short *v15;

    v2 = **(Work ***)((char *)fn_0238E688() + 0x1610);
    v3 = v2->recs;
    v20 = 0;
    v4 = v2->mode;
    if (v4 == 9)
        v5 = 1;
    else if (v4 == 10)
        v5 = v20;
    else
        return v20;
    fn_023817E0((int)&v2->lock);
    if (v2->flag == 0) {
        fn_0238176C((int)&v2->lock);
        return 0;
    }
    {
        int v7 = (v5 == 1) ? v2->f43 : 1;
        v8 = 0;
        v22 = (unsigned short)v7;
    }
    v9 = 0xFFFF;
    do {
        v10 = &v2->in[v8];
        v11 = v9;
        v21 = v9;
        for (i = v10->head; i != v9; i = v10->head) {
            v13 = &v3[i];
            if ((v13->h[2] & v22) != 0 &&
                (a1 != 0 || (a2 & v13->h[2]) != 0) &&
                ((v13->h[1] & 8) != 0 || v13->h[5] != 0)) {
                v20 = 1;
                if (v13->h[5] != 0)
                    --v13->h[5];
                if (a1 == 0)
                    v13->h[2] &= a2;
                if (v13->h[0] == v9)
                    v10->tail = v9;
                v10->head = v13->h[0];
                v13->h[0] = v9;
                if (v11 == v9)
                    v21 = i;
                else
                    v3[v11].h[0] = i;
                v11 = i;
            } else {
                v15 = fn_0238DE4C();
                v15[0] = 0x81;
                m = v13->h[2];
                if ((m & v22) == 0) {
                    v15[1] = 0;
                    v13->h[2] = 0;
                } else if (a1 != 0 || (a2 & m) != 0) {
                    v15[1] = 0xF;
                } else {
                    v15[1] = 0;
                }
                v15[4] = 0x14;
                v15[5] = v13->h[1];
                v15[6] = v13->h[2];
                v15[7] = v13->h[3];
                ((int *)v15)[4] = v13->d[0];
                ((int *)v15)[5] = v13->d[1];
                ((int *)v15)[6] = v13->d[2];
                v15[14] = v13->h[4];
                fn_0238DEF4((int)v15);
                if (v13->h[0] == v9)
                    v10->tail = v9;
                v10->head = v13->h[0];
                v13->h[0] = v9;
                m = v2->free_tail;
                if (m == v9)
                    v2->free_head = i;
                else
                    v3[m].h[0] = i;
                v2->free_tail = i;
            }
        }
        if (v11 != v9) {
            v3[v11].h[0] = v2->out[v8].head;
            if (v2->out[v8].tail == v9)
                v2->out[v8].tail = v11;
            v2->out[v8].head = v21;
        }
        ++v8;
    } while (v8 < 4);
    v2->flag = 0;
    fn_0238176C((int)&v2->lock);
    return v20;
}
