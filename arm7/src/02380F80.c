extern int word_3806E60[];

int *fn_02380F80(void) {
    int *node;

    for (node = (int *)word_3806E60[3]; node != 0 && node[0x12] != 1; node = (int *)node[0x13])
        ;
    return node;
}
