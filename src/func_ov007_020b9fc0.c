extern int _ZN4cstd3divEii(int, int);
extern void *data_ov007_02104b9c;
extern void *data_ov007_02104ba0;

int func_ov007_020b9fc0(void) {
    char *base = (char *)data_ov007_02104b9c;
    char *p1 = *(char **)(base + 4);
    char *p2 = *(char **)(base + 8);
    int *addr4 = (int *)(p2 + 0x50);
    int *addr1 = (int *)(((long long)(int)(p1 + 0x50)) & 0xffffffffffffffffLL);

    if (*addr1 < 0x1f000) {
        char *c = (char *)data_ov007_02104ba0;
        if (*(short *)(*(char **)(c + 8)) == 5) {
            *addr1 = 0x1f000;
        }
    } else {
        *addr4 -= 0x1000;
        if (*addr4 <= 0) {
            *addr4 = 0;
        }
    }

    *(int *)((char *)data_ov007_02104ba0 + 0x40) = _ZN4cstd3divEii(*addr4, 0x1f);

    return (*addr4 == 0) ? 1 : 0;
}
