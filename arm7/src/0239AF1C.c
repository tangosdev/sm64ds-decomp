/* storage 0x0239AF1C runtime 0x027CCE94 size 0x2EC main. */

extern void fn_0239AE84(int a, int b);
extern void fn_0239477C(unsigned int a, unsigned int b);

struct DEnt {
    unsigned short lo;
    unsigned short hi;
    int (*fn)(int node, unsigned short *slot);
};

#pragma opt_common_subs off
void fn_0239AF1C(void) {
    char *base;
    int node;
    unsigned short *slot;
    unsigned int lim;
    struct DEnt *tbl;
    unsigned short type;
    unsigned short idx;
    unsigned int err;
    unsigned int mask;

    err = 0;
    mask = err;
    base = *(char **)0x0380FFF4;
    if (*(unsigned short *)(base + 0x3E0) != 0)
        return;
    node = *(int *)(base + 0x1C4);
    *(int *)(base + 0x3DC) = node;
    base = *(char **)0x0380FFF4;
    node = *(int *)(base + 0x3DC);
    if (node == -1)
        return;
    slot = (unsigned short *)(node + *(unsigned short *)(node + 0xE) * 2 + 0x10);
    if (*(unsigned short *)(base + 0x302) != 0) {
        slot[1] = 1;
        slot[2] = 6;
        goto clear;
    }
    type = *(unsigned short *)(node + 0xC);
    if (type != slot[0]) {
        slot[2] = 0xD;
        goto clear;
    }
    switch (type & 0xFF00) {
    case 0x000:
        mask = 1;
        tbl = (struct DEnt *)0x027D68BC;
        idx = type & 0xFF;
        lim = 0xB;
        if (*(unsigned short *)(base + 0x3E0) & 1) {
            err = 2;
        } else if (*(unsigned short *)(base + 0x310) < 0x20) {
            err = mask;
        }
        break;
    case 0x100:
        mask = 2;
        tbl = (struct DEnt *)0x027D6814;
        idx = type & 0xFF;
        lim = 5;
        if (*(unsigned short *)(base + 0x310) != 0x40)
            err = 1;
        break;
    case 0x200:
        idx = type & 0xFF;
        if (idx < 0x40) {
            if (*(unsigned short *)(base + 0x310) < 0x10)
                err = 1;
            mask = 4;
            tbl = (struct DEnt *)0x027D69C4;
            lim = 0x16;
        } else if (idx < 0x80) {
            if (*(unsigned short *)(base + 0x310) != 0x40)
                err = 1;
            mask = 8;
            tbl = (struct DEnt *)0x027D683C;
            idx -= 0x40;
            lim = 6;
        } else if (idx < 0xC0) {
            if (*(unsigned short *)(base + 0x310) < 0x10)
                err = 1;
            mask = 0x10;
            tbl = (struct DEnt *)0x027D6914;
            idx -= 0x80;
            lim = 0x16;
        } else {
            idx = 1;
            lim = err;
        }
        break;
    case 0x300:
        mask = 0x40;
        tbl = (struct DEnt *)0x027D686C;
        idx = type & 0xFF;
        lim = 0xA;
        break;
    default:
        idx = 1;
        lim = 0;
        break;
    }
    if (idx > lim) {
        err = 3;
    } else if (*(unsigned short *)(node + 0xE) < tbl[idx].lo || slot[1] < tbl[idx].hi) {
        err = 4;
    }
    if (err != 0) {
        slot[1] = 1;
        slot[2] = err;
        goto clear;
    }
    *(unsigned short *)(base + 0x3E0) |= mask;
    slot[2] = tbl[idx].fn(node, slot);
    if (slot[2] == 0x80)
        return;
    if (slot[2] != 0x81)
        goto clear;
    *(unsigned short *)(*(char **)0x0380FFF4 + 0x3E0) &= ~mask;
    goto tail;
clear:
    *(unsigned short *)(*(char **)0x0380FFF4 + 0x3E0) &= ~mask;
    fn_0239AE84((int)(*(char **)0x0380FFF4 + 0x1C4),
                *(int *)(*(char **)0x0380FFF4 + 0x3DC));
tail:
    if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x1CC) != 0)
        fn_0239477C(2, 0xB);
}
