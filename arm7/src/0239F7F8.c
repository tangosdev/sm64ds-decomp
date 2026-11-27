/* storage 0x0239F7F8 runtime 0x027D1770 size 0x98 main. */

extern int fn_02394B80(int *node);
extern void fn_0239AAA8(unsigned short *arg);
extern void fn_0239D638(char *list, int *node);

void fn_0239F7F8(unsigned int which, unsigned int flag) {
    unsigned int scaled;
    int *node;
    int next;
    unsigned short *entry;

    scaled = which * 0xC;
    node = *(int **)(*(char * volatile *)0x0380FFF4 + scaled + 0x158);
    if (node != (int *)-1) {
        do {
            next = fn_02394B80(node);
            entry = (unsigned short *)((char *)node + 0x10);
            if (which != 2)
                fn_0239AAA8(entry);
            entry[4] = 2;
            if (flag != 0)
                fn_0239D638(*(char * volatile *)0x0380FFF4 + 0x158 + scaled, node);
            node = (int *)next;
        } while (node != (int *)-1);
    }
}
