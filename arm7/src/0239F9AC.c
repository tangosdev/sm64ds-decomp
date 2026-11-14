/* storage 0x0239F9AC runtime 0x027D1924 size 0xEC main. */

extern int fn_02394B80(int *node);
extern void fn_0239AAA8(unsigned short *arg);
extern void fn_0239D638(char *list, int *node);

void fn_0239F9AC(unsigned int index) {
    int *node;
    char **pp;
    unsigned int flag;
    unsigned int i;
    unsigned int wide;
    unsigned int scaled;
    int next;
    char *entry;
    int one;
    char *base;

    pp = (char **)0x0380FFF4;
    base = *pp;
    flag = 0;
    if (*(unsigned short *)(*(int *)(base + 0x2E0) + index * 0x1C + 0x16) == 0)
        return;
    i = flag;
    one = 1;
    for (; i < 3; i = i + 2) {
        scaled = i * 0xC;
        node = *(int **)(*pp + scaled + 0x158);
        if (node == (int *)-1)
            continue;
        wide = i * 0x14;
        do {
            next = fn_02394B80(node);
            entry = (char *)node + 0x10;
            if (*(unsigned short *)(entry + 2) == index
                && entry != *(char **)(wide + (unsigned int)*pp + 0x3F0)) {
                *(unsigned short *)(entry + 8) = 2;
                fn_0239AAA8((unsigned short *)entry);
                fn_0239D638(*pp + 0x158 + scaled, node);
                if (flag == 0)
                    flag = one;
            }
            node = (int *)next;
        } while (node != (int *)-1);
    }
}
