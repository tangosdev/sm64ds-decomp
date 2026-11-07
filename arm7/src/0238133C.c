/* storage 0x0238133C runtime 0x037F91D4 size 0x44 wram. Pool +0x44: 03806E60.
 * The base pointer has to die after the first load so the equal path reloads it. */

extern int word_3806E60[];

int *fn_0238133C(int *node) {
    int *base;
    int *cur;
    int *prev;
    int *next;

    base = word_3806E60;
    cur = (int *)base[3];
    prev = 0;
    while (cur != 0 && cur != node) {
        prev = cur;
        cur = *(int **)((char *)cur + 0x4C);
    }
    if (prev == 0) {
        next = *(int **)((char *)node + 0x4C);
        word_3806E60[3] = (int)next;
        return word_3806E60;
    }
    next = *(int **)((char *)node + 0x4C);
    *(int **)((char *)prev + 0x4C) = next;
    return next;
}
