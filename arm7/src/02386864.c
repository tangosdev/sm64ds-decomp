extern unsigned char unk_3808544[];
extern void fn_0238219C(unsigned char *);

int fn_02386864(int a1) {
    unsigned char *entry = unk_3808544 + (a1 << 6);
    int result = *entry;
    if (result != 0) {
        fn_0238219C(entry + 20);
        entry[1] += 1;
        *entry = 0;
        return 0;
    }
    return result;
}
