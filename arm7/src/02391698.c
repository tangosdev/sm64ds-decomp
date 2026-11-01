/* MP parent. States 8 and 10 finish the setup at +0x186 and drop to state 2.
 * States 7 and 9 clear each aid bit set in both +0x17E and the caller's mask.
 * a2 == 1 also posts the result packet. */

extern void *fn_0238E688(void);
extern unsigned short *fn_0238E57C(unsigned short *dst, unsigned short *src);
extern unsigned short *fn_02390214(unsigned short *buf, unsigned short *src, int n);
extern int fn_0239165C(int a, int b);
extern unsigned short *fn_023904C0(unsigned short *buf, int n);
extern unsigned short *fn_0238FB88(unsigned short *buf);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *pkt);

int fn_02391698(int *a1, int a2) {
    unsigned short tmpA[3];
    unsigned short tmpB[3];
    unsigned short buf[0x100];
    unsigned short **pp;
    unsigned short *obj;
    unsigned short mask16;
    int state;
    int eight;
    int zero;
    int c3;
    int c9;
    int c24;
    int one;
    int i;
    int idx;

    pp = *(unsigned short ***)((char *)fn_0238E688() + 0x1610);
    mask16 = (unsigned short)a1[1];
    obj = *pp;
    state = obj[0];
    if (state != 9 && state != 7) {
        if (state == 10 || state == 8) {
            if (state == 10) {
                *(int *)((char *)obj + 0x10) = 0;
                *(int *)((char *)*pp + 0x18) = 0;
                *(int *)((char *)*pp + 0x24) = 0;
            }
        } else {
            return 0;
        }
    }

    obj = *pp;
    state = obj[0];
    if (state == 10 || state == 8) {
        unsigned short *slot;

        fn_0238E57C(tmpA, (unsigned short *)((char *)obj + 0x186));
        slot = fn_02390214(buf, tmpA, 3);
        if (slot[2] != 0) {
            fn_0239165C(5, slot[2]);
            return 0;
        }
        slot = fn_023904C0(buf, 1);
        if (slot[2] != 0) {
            fn_0239165C(0, slot[2]);
            return 0;
        }
        (*pp)[0] = 3;
        slot = fn_0238FB88(buf);
        if (slot[2] != 0) {
            fn_0239165C(0x302, slot[2]);
            return 0;
        }
        (*pp)[0] = 2;
        *(int *)((char *)*pp + 0x10) = 0;
        *(int *)((char *)*pp + 0x18) = 0;
        *(int *)((char *)*pp + 0x14) = 0;
        if (a2 == 1) {
            unsigned short *pkt = fn_0238DE4C();
            pkt[0] = 12;
            pkt[1] = 0;
            pkt[4] = 9;
            pkt[6] = 0x24;
            pkt[5] = *(unsigned short *)((char *)*pp + 0x184);
            *(unsigned short *)((char *)*pp + 0x18E) = 0;
            *(unsigned short *)((char *)*pp + 0x190) = 0;
            fn_0238DEF4(pkt);
        }
    } else {
        eight = 8;
        zero = 0;
        c3 = 3;
        c9 = 9;
        c24 = 0x24;
        one = 1;
        for (i = 1; i < 16; i++) {
            obj = *pp;
            int bit = one << i;
            if ((bit & (*(unsigned short *)((char *)obj + 0x17E) & mask16)) != 0) {
                unsigned short *slot;
                unsigned short *pkt;
                unsigned short *hp;

                idx = (unsigned short)i;
                fn_0238E57C(tmpB, (unsigned short *)((char *)obj + 0x124 + (i - 1) * 6));
                slot = fn_02390214(buf, tmpB, c3);
                if (slot[2] != 0) {
                    fn_0239165C(5, slot[2]);
                    return 0;
                }
                hp = (unsigned short *)((char *)*pp + 0x17E);
                *hp &= ~bit;
                *(unsigned short *)((char *)*pp + 0x86) &= ~bit;
                *(unsigned short *)((char *)*pp + i * 6 + 0x11E) = (unsigned short)zero;
                *(unsigned short *)((char *)*pp + i * 6 + 0x120) = (unsigned short)zero;
                *(unsigned short *)((char *)*pp + i * 6 + 0x122) = (unsigned short)zero;
                if (a2 == 1) {
                    pkt = fn_0238DE4C();
                    pkt[0] = (unsigned short)eight;
                    pkt[1] = (unsigned short)zero;
                    pkt[4] = (unsigned short)c9;
                    pkt[9] = (unsigned short)c24;
                    pkt[8] = idx;
                    fn_0238E57C(pkt + 5, tmpB);
                    fn_0238DEF4(pkt);
                }
            }
        }
    }
    return 1;
}
