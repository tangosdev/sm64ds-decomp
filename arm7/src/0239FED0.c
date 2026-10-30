/* storage 0x0239FED0  runtime 0x027D1E48  linked 0x4C8  main ARM.
 * Wireless message dispatcher: bumps the +0x4F4 stat counters, then
 * switches on (msg+0x14 & 0xFC). 0x10/0x30 pair send, 0xB0 pair config,
 * 0xA0/0xC0 rescan the slot table (threshold 0x30/0x20, marker 0x71/0x41).
 * Tail: teardown calls + the +0x3F8 clear + optional fn_023A07BC(1). */

typedef struct {
    char _000[0x6C];
    unsigned short f_16C;
} Dat100;
typedef struct {
    char _000[0xE6];
    unsigned short f_2E6;
} Dat200;
typedef struct {
    char _000[0x10];
    unsigned short f_310;
    char _012[0x2];
    unsigned short f_314;
    char _016[0xA6];
    unsigned short f_3BC;
    char _0BE[0x3A];
    unsigned short f_3F8;
} Dat300;
typedef struct {
    char _000[0xEA];
    unsigned short f_4EA;
} Dat400;

extern char *dat_0380FFF4;
extern void fn_0239AAA8(void *);
extern int fn_0239A98C(unsigned short, int);
extern void fn_0239B76C(void *, int);
extern void fn_0239B5F4(void *, int, void *);
extern void fn_0239B504(void *, int, void *);
extern int fn_0239A7A4(unsigned short);
extern void fn_023969F4(int);
extern void fn_02396A98(void);
extern void fn_0239B7E8(void);
extern void fn_02394DF8(int, void *);
extern int fn_023A07BC(int);

void fn_0239FED0(unsigned short *a1, int a2) {
    int *stats = (int *)(dat_0380FFF4 + 0x4F4);
    int peer = a1[1];
    unsigned int code;
    unsigned int i;
    unsigned int j;
    char **gp;
    char *d;

    if (!(a1[4] & 2)) {
        ++stats[0];
        if (a1[0xC] & 1)
            ++stats[5];
        else
            ++stats[4];
        if (((*(unsigned short *)(dat_0380FFF4 + 0x4EA) >> peer) & 1) != 0 &&
            !(a1[0xA] & 0x2000) &&
            *(unsigned short *)(*(int *)(dat_0380FFF4 + 0x2E0) + peer * 0x1C) ==
                0x40)
            *(unsigned short *)(dat_0380FFF4 + 0x4EC) &= ~(1 << peer);
    } else {
        ++stats[1];
    }
    if ((unsigned)(a1[0xA] << 0x11) >> 0x1F)
        ++stats[6];
    stats[2] += (unsigned)(a1[6] << 8) >> 16;
    code = a1[0xA] & 0xFC;
    switch (code) {
    case 0xB0:
        if (!(a1[4] & 2)) {
            int v = a1[0x16];
            if (v == 0 && a1[0x17] == 2 && a1[0x18] == 0) {
                fn_0239A98C(peer, 0x30);
                fn_0239B76C(a1 + 0xC, a1[0x16]);
            } else if (v == 1 && a1[0x17] == 4 && a1[0x18] == 0) {
                fn_0239A98C(peer, 0x30);
                fn_0239B76C(a1 + 0xC, a1[0x16]);
            }
        }
        break;
    case 0x10:
    case 0x30:
        if (!(a1[4] & 2)) {
            if (a1[0x17] == 0) {
                fn_0239A98C(peer, 0x40);
                if (code == 0x10)
                    fn_0239B5F4(a1 + 0xC, a1[0x18],
                                (char *)(a1 + 0xA) + a1[9]);
                else
                    fn_0239B504(a1 + 0xC, a1[0x18],
                                (char *)(a1 + 0xA) + a1[9]);
            }
        } else {
            fn_0239A7A4(peer);
        }
        break;
    case 0xA0:
        gp = &dat_0380FFF4;
        d = *gp;
        if (((Dat300 *)(d + 0x300))->f_314 == 1) {
            if (peer != 0) {
                if (*(unsigned short *)(*(int *)(d + 0x2E0) +
                                        peer * 0x1C) > 0x30)
                    fn_0239A98C(peer, 0x30);
            } else if (a1[0xC] & 1) {
                for (i = 1; i < ((Dat200 *)(*gp + 0x200))->f_2E6; i++)
                    if (*(unsigned short *)(*(int *)(*gp + 0x2E0) +
                                            i * 0x1C) > 0x30)
                        fn_0239A98C(i, 0x30);
            }
        } else {
            if (((Dat300 *)(d + 0x300))->f_310 > 0x30) {
                fn_023969F4(0x30);
                fn_02396A98();
            }
        }
        gp = &dat_0380FFF4;
        d = *gp;
        if (((Dat300 *)(d + 0x300))->f_3BC == 0x71) {
            *(unsigned short *)(*(int *)(d + 0x3D8) + 4) = 0;
            ((Dat300 *)(*gp + 0x300))->f_3BC = 0;
            fn_0239B7E8();
        }
        break;
    case 0xC0:
        gp = &dat_0380FFF4;
        d = *gp;
        if (((Dat300 *)(d + 0x300))->f_314 == 1) {
            if (peer != 0) {
                if (*(unsigned short *)(*(int *)(d + 0x2E0) +
                                        peer * 0x1C) > 0x20)
                    fn_0239A98C(peer, 0x20);
            } else if (a1[0xC] & 1) {
                for (j = 1; j < ((Dat200 *)(*gp + 0x200))->f_2E6; j++)
                    if (*(unsigned short *)(*(int *)(*gp + 0x2E0) +
                                            j * 0x1C) > 0x20)
                        fn_0239A98C(j, 0x20);
            }
        } else {
            if (((Dat300 *)(d + 0x300))->f_310 > 0x20) {
                fn_023969F4(0x20);
                fn_02396A98();
            }
        }
        gp = &dat_0380FFF4;
        d = *gp;
        if (((Dat300 *)(d + 0x300))->f_3BC == 0x41) {
            *(unsigned short *)(*(int *)(d + 0x3D8) + 4) = 0;
            ((Dat300 *)(*gp + 0x300))->f_3BC = 0;
            fn_0239B7E8();
        }
        break;
    case 0:
    case 0x20:
    default:
        break;
    }
    fn_0239AAA8(a1);
    fn_02394DF8((int)(dat_0380FFF4 + 0x164), a1 - 8);
    ((Dat300 *)(dat_0380FFF4 + 0x300))->f_3F8 = 0;
    if (a2 != 0 && ((Dat100 *)(dat_0380FFF4 + 0x100))->f_16C != 0)
        fn_023A07BC(1);
}
