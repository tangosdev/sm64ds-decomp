typedef union {
    unsigned short w;
    struct {
        unsigned short b0:1,b1:1,b2:1,b3:1,b4:1,b5:1,b6:1,b7:1,
                       b8:1,b9:1,b10:1,b11:1,b12:1,b13:1,b14:1,b15:1;
    } b;
} Flags;

extern int fn_0238D594(int,int);
extern int fn_0239AD50(unsigned short*);
extern void fn_0239A8F8(unsigned short,int);
extern void fn_023969D8(unsigned short*,unsigned short*);
extern int fn_0239F064(unsigned short*,int);
extern int fn_0239E91C(unsigned short*,int);
extern void fn_0239F778(int);
extern void fn_02396EFC(int);
extern void fn_02394DF8(int,int);
extern void fn_0239A96C(unsigned short,unsigned short);
extern void fn_0239AE84(int,int);
extern void fn_0239477C(int,int);

void fn_023A3CAC(void) {
    #pragma opt_propagation off
    unsigned short *v3;
    unsigned short *cfg;
    int *v2p;
    int *v2;
    int v0;
    int v4;
    int v8;

    v2 = (int *)(*(unsigned char **)0x0380FFF4 + 0x4F4);
    v3 = *(unsigned short **)(*(unsigned char **)0x0380FFF4 + 0x188);
    if (v3 == (unsigned short *)-1) return;
    if (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x310) != 0x40) {
        fn_02394DF8((int)(*(unsigned char **)0x0380FFF4 + 0x188), (int)v3);
        return;
    }
    cfg = v3 + 8;
    if (((Flags *)&cfg[10])->b.b8) {
        if (((Flags *)&cfg[18])->w & 1) ++v2[11]; else ++v2[10];
    } else if (((Flags *)&cfg[12])->w & 1) {
        ++v2[11];
    } else {
        ++v2[10];
    }
    v2p = v2 + 9;
    *v2p += fn_0238D594(cfg[4] & 0xF0, 16) - 1;
    v4 = 1;
    switch (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x314)) {
    case 1:
        if (!(((Flags *)&cfg[10])->w & 1)) {
            v0 = fn_0239AD50(cfg + 15);
            if (!(v0 != 0xFF && *(unsigned short *)(*(int *)(*(unsigned char **)0x0380FFF4 + 0x2E0) + 28 * v0) == 0x40)) {
                if (*(unsigned short *)(*(int *)(*(unsigned char **)0x0380FFF4 + 0x2E0) + 28 * v0) == 0x30)
                    v8 = fn_0239F064(cfg + 15, 7);
                else
                    v8 = fn_0239E91C(cfg + 15, 7);
                if (v8 != 0) fn_0239F778(v8);
            } else {
                fn_0239A8F8((unsigned short)v0, ((Flags *)&cfg[10])->b.b12);
                if (cfg[21] == *(unsigned short *)(*(int *)(*(unsigned char **)0x0380FFF4 + 0x2E0) + 28 * v0 + 20)) {
                    ++v2[15];
                } else {
                    fn_023969D8(cfg + 12, cfg + 18);
                    v4 = 0;
                }
            }
        }
        break;
    case 2:
    case 3:
        if (!(((Flags *)&cfg[10])->w & 1)) {
            if (((Flags *)(*(unsigned char **)0x0380FFF4 + 0x316))->b.b0 && !(((Flags *)&cfg[10])->w & 0x2000)) {
                if (((Flags *)&cfg[12])->w & 1)
                    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x38A) &= ~1;
                else
                    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x38A) = 0;
                if (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x38A) == 0 &&
                    *(unsigned short *)(*(int *)(*(unsigned char **)0x0380FFF4 + 0x2E0) +
                        28 * *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x384) + 22) == 0)
                    fn_02396EFC(1);
            }
            v0 = *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x384);
            if (cfg[21] == *(unsigned short *)(*(int *)(*(unsigned char **)0x0380FFF4 + 0x2E0) + 28 * v0 + 20)) {
                ++v2[15];
            } else {
                fn_023969D8(cfg + 15, cfg + 18);
                v4 = 0;
            }
        }
        break;
    }
    if (v4 == 0) {
        int v10;
        cfg[1] = (unsigned short)v0;
        fn_0239A96C(v0, (unsigned short)(cfg[9] & 0xff));
        *(unsigned short *)(*(int *)(*(unsigned char **)0x0380FFF4 + 0x2E0) + 28 * v0 + 20) = cfg[21];
        v10 = *(int *)(*(unsigned char **)0x0380FFF4 + 0x2E0) + 28 * v0;
        *(unsigned short *)(v10 + 24) = *(unsigned short *)(v10 + 26);
        cfg[3] = cfg[8] - 24;
        v3[6] = 384;
        v3[7] = (unsigned)(cfg[3] + 45) >> 1;
        fn_0239AE84((int)(*(unsigned char **)0x0380FFF4 + 0x188), (int)v3);
    } else {
        fn_02394DF8((int)(*(unsigned char **)0x0380FFF4 + 0x188), (int)v3);
    }
    if (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x190) != 0)
        fn_0239477C(2, 6);
}
