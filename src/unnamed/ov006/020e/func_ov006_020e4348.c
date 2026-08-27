void func_ov006_020e4348(char *base, int idx) {
    int off = idx * 0x24;
    int *a = (int *)(base + 0x48c0 + off);
    int *c = (int *)(base + 0x48c8 + off);
    int *b = (int *)(base + 0x48c4 + off);
    *a += *c;
    *b += *(int *)(base + off + 0x48cc);
    if (*c > 0) {
        *c -= 0x20;
        if ((int)(short)*c < 0) *c = 0;
    } else if (*c < 0) {
        *c += 0x20;
        if (*c > 0) *c = 0;
    } else {
        *(unsigned char *)(base + off + 0x48de) = 0;
    }
}
