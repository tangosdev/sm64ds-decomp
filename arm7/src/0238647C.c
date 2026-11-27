extern unsigned char unk_3807B04[];
extern unsigned char unk_3807D44[];
void fn_0238647C(void) {
    int i = 0;
    unsigned char *e;
    int j;
    do {
        e = &unk_3807B04[i * 36];
        unk_3807B04[i * 36] = 0;
        e[2] = i;
        i += 1;
    } while (i < 16);
    j = 0;
    do {
        unk_3807D44[j << 6] &= ~1;
        j += 1;
    } while (j < 32);
}
