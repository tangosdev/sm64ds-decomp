#pragma opt_propagation off

/* Rising edge of sample bits 4 and 5. The unsigned-short bitfield is
 * what keeps the halfword in r1 and the extracted bit in r3; a plain
 * unsigned temporary swaps those two. opt_propagation off keeps
 * `one = zero | 1` as orr. The two self-selects pin base in r4 and
 * code in r5. */

extern int fn_02381050(unsigned short *a1);
extern int fn_0238AB54(int a1);
extern int fn_0238AAFC(int a1);
extern int fn_0238AAD0(int a1);
extern void fn_0238A4E0(int code, int flags);

extern unsigned short unk_380973C;
extern char unk_3809520[];

struct SampleBits {
    unsigned short low: 4;
    unsigned short bit4: 1;
    unsigned short bit5: 1;
    unsigned short rest: 10;
};

void fn_02389F30(void) {
    union {
        unsigned short hw[6];
        struct {
            struct SampleBits cur;
            struct SampleBits prev;
            unsigned short sample[4];
        } bits;
    } frame;
    unsigned int flags;
    unsigned short *word;
    unsigned short *cur;
    int zero;
    int one;
    int code;
    char *base;
    unsigned short *sample;

    frame.hw[1] &= ~0x10;
    frame.hw[1] &= ~0x20;
    cur = &frame.hw[0];
    sample = &frame.hw[2];
    zero = 0;
    code = 0x30;
    word = &unk_380973C;
    base = unk_3809520;
    one = zero | 1;
    base = base ? base : base;
    code = code ? code : code;

    while (1) {
        do {
            fn_02381050(word);
        } while (*(int *)(base + 0x19C) != 0);
        fn_0238AB54((int)cur);
        if ((frame.bits.cur.bit4 - frame.bits.prev.bit4 == 1) ||
            (frame.bits.cur.bit5 - frame.bits.prev.bit5 == 1)) {
            fn_0238AAFC((int)sample);
            flags = zero;
            if (((unsigned int)frame.hw[0] << 27) >> 31) {
                flags = one;
                frame.hw[2] &= ~0x0F;
            }
            if (((unsigned int)frame.hw[0] << 26) >> 31) {
                flags |= 2;
                frame.hw[2] &= ~0x40;
            }
            fn_0238AAD0((int)sample);
            fn_0238A4E0(code, flags);
        }
        frame.hw[1] = frame.hw[0];
    }
}
