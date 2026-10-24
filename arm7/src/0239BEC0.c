/* storage 0x0239BEC0 runtime 0x027CDE38 main ARM. */

extern void fn_023969F4(int a);
extern unsigned int fn_02395334(unsigned short *p);
extern int fn_023A43D8(int a);
extern void fn_023970C8(unsigned int note, int flag);
extern void fn_02395DA8(void);
extern unsigned short *fn_0239EB98(unsigned short *p);
extern void fn_0239F778(unsigned short *node);
extern void fn_0239554C(unsigned int key, unsigned int cb);
extern void fn_02395CB8(void);
extern void fn_0239B7E8(void);
extern void fn_0238477C(int a, int b);

void fn_0239BEC0(void) {
    unsigned short *state;
    int fail;
    unsigned int note;

    state = (unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x3BC);
    fail = 0;
    switch (*state) {
    case 0x10:
        fn_023969F4(0x20);
        *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x300 + 0x14) = 2;
        *(unsigned short *)(*(unsigned char **)((char *)state + 0x1C) + 8) = (unsigned short)fail;
        *(unsigned short *)(*(unsigned char **)((char *)state + 0x1C) + 6) = (unsigned short)fail;
        state[3] = (unsigned short)fail;
        state[4] = (unsigned short)fail;
        {
            unsigned char *rec;

            rec = *(unsigned char **)((char *)state + 0x18);
            if (*(unsigned short *)(rec + 0x38) == 0) {
                state[6] = (unsigned short)((*(unsigned short *)(rec + 0x4A) + 3) / 4);
                if (state[6] < 10u)
                    state[6] = 10;
            } else {
                state[6] = *(unsigned short *)(rec + 0x4A);
            }
        }
        *(unsigned short *)(*(unsigned char **)((char *)state + 0x1C) + 4) = 0;
        /* fall through */
    case 0x11:
        note = fn_02395334((unsigned short *)(*(unsigned char **)((char *)state + 0x18) + 0x3A + state[3]));
        if (note == 0) {
            *state = 0x15;
            fail = 1;
            break;
        }
        {
            unsigned short *step;

            step = state + 3;
            *step = (unsigned short)(*step + 1);
        }
        state[5] = 0;
        if (fn_023A43D8(0) != 0) {
            *(unsigned short *)(*(unsigned char **)((char *)state + 0x1C) + 4) = 14;
            *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x300 + 0xBC) = 0x15;
            fail = 1;
            break;
        }
        if (*state == 0x10) {
            fn_023970C8(note, 0);
            fn_02395DA8();
        } else {
            fn_023970C8(note, 1);
        }
        *state = 0x12;
        /* fall through */
    case 0x12:
    case 0x13:
        *state = 0x13;
        {
            unsigned char *rec;

            rec = *(unsigned char **)((char *)state + 0x18);
            if (*(unsigned short *)(rec + 0x38) == 0) {
                unsigned short *created;

                created = fn_0239EB98((unsigned short *)(rec + 0x10));
                if (created == 0) {
                    *(unsigned short *)(*(unsigned char **)((char *)state + 0x1C) + 4) = 8;
                    *state = 0x15;
                    fail = 1;
                    break;
                }
                fn_0239F778(created);
            }
        }
        fn_0239554C(state[6], 0x027CDDD4);
        break;
    case 0x15:
        *state = (unsigned short)fail;
        fn_02395CB8();
        {
            unsigned char *base;

            base = *(unsigned char **)0x0380FFF4;
            *(unsigned short *)(base + 0x300 + 0x14) = *(unsigned short *)(base + 0x200 + 0xF2);
        }
        fn_0239B7E8();
        break;
    case 0x14:
    default:
        break;
    }
    if (fail != 0)
        fn_0238477C(2, 0);
}
