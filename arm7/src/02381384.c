/* storage 0x02381384 runtime 0x037F921C size 0x64 wram. Pool +0x64: 03806E60. */

extern int word_3806E60[];

int *fn_02381384(int *node) {
    int *cur;
    int *prev;

    cur = (int *)word_3806E60[3];
    prev = 0;
    while (cur != 0 && *(unsigned int *)((char *)cur + 0x54)
            < *(volatile unsigned int *)((char *)node + 0x54)) {
        prev = cur;
        cur = *(int **)((char *)cur + 0x4C);
    }
    if (prev == 0) {
        *(int *)((char *)node + 0x4C) = word_3806E60[3];
        word_3806E60[3] = (int)node;
    } else {
        *(int *)((char *)node + 0x4C) = *(int *)((char *)prev + 0x4C);
        *(int *)((char *)prev + 0x4C) = (int)node;
    }
    return node;
}
