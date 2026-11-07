#pragma opt_propagation off

extern unsigned short word_38098E4[];
extern int fn_023882B0(int a, int b, int c, int d, int e, unsigned f);
extern int fn_023883FC(int a, int b);

void fn_0238C70C(unsigned int a1, int a2, int a3, int a4) {
    unsigned short *table;
    unsigned int idx;
    unsigned short hw;
    unsigned int op;
    unsigned int ptr;
    int fourth;
    int fifth;

    if (a1 & 0x2000000) {
        int i = 0;
        do {
            word_38098E4[i] = 0;
            i = i + 1;
        } while (i < 16);
    }
    idx = a1 & 0xF0000u;
    idx = idx >> 16;
    {
        unsigned off = idx << 1;
        table = word_38098E4;
        *(unsigned short *)((char *)table + off) = (unsigned short)a1;
    }
    if ((a1 & 0x1000000) == 0)
        return;
    hw = table[0];
    op = ((hw & 0xFF00u) << 8) >> 16;
    fourth = a4;
    fifth = a3;
    switch (op) {
    case 0x22:
        ptr = ((unsigned)(hw & 0xFF) << 24)
            | ((unsigned)table[1] << 8)
            | ((unsigned)(table[2] & 0xFF00) >> 8);
        if (ptr < 0x2000000 || ptr >= 0x2400000) {
            fn_023883FC((int)op, 2);
            return;
        }
        break;
    case 0x23:
    case 0x24: {
        unsigned hi = table[4];
        unsigned lo = table[5];
        ptr = lo | (hi << 16);
        if (ptr < 0x2000000 || ptr >= 0x2400000) {
            fn_023883FC((int)op, 2);
            return;
        }
        {
            unsigned lo8 = hw & 0xFFu;
            unsigned t1 = table[1];
            fourth = (int)(t1 | (lo8 << 16));
            unsigned t2 = table[2];
            unsigned t3 = table[3];
            fifth = (int)(t3 | (t2 << 16));
        }
        break;
    }
    case 0x25:
    case 0x26: {
        unsigned hi = table[3];
        unsigned lo = table[4];
        ptr = lo | (hi << 16);
        if (ptr < 0x2000000 || ptr >= 0x2400000) {
            fn_023883FC((int)op, 2);
            return;
        }
        {
            unsigned lo8 = hw & 0xFFu;
            unsigned t1 = table[1];
            fourth = (int)(t1 | (lo8 << 16));
            fifth = (int)table[2];
        }
        break;
    }
    case 0x27:
    case 0x28: {
        unsigned lo8 = hw & 0xFFu;
        fourth = (int)lo8;
        fourth = (int)((unsigned)table[1] | ((unsigned)fourth << 16));
        break;
    }
    default:
        break;
    }
    if (fn_023882B0(1, (int)op, 3, fourth, fifth, ptr) == 0)
        fn_023883FC((int)op, 4);
}
