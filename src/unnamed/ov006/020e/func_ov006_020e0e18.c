void func_ov006_020e0e18(char *base, int idx) {
    int off = idx * 0x24;
    int *a = (int *)(base + 0x478c + off);
    int *c = (int *)(base + 0x4794 + off);
    int *b = (int *)(base + 0x4790 + off);
    *a += *c;
    *b += *(int *)(base + off + 0x4798);
    if (*c > 0) {
        *c -= 0x20;
        if ((int)(short)*c < 0) *c = 0;
    } else if (*c < 0) {
        *c += 0x20;
        if (*c > 0) *c = 0;
    } else {
        *(unsigned char *)(base + off + 0x47aa) = 0;
    }
}
