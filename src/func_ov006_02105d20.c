extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_02105d20(char *c) {
    int found;
    int i;
    int n;
    int tmp;
    if (*(unsigned char *)(c + 0x4fe9) == 0) return;
    if (*(unsigned char *)(c + 0x4feb) == 0) return;
    tmp = *(volatile unsigned char *)(int *)(((int)c + 0x4feb) & 0xFFFFFFFFFFFFFFFF);
    --tmp;
    *(volatile unsigned char *)(int *)(((int)c + 0x4feb) & 0xFFFFFFFFFFFFFFFF) = tmp;
    if (*(unsigned char *)(c + 0x4feb) != 0) return;
    found = 0;
    i = 0;
    n = *(int *)(c + 0x4cb8);
    while (i < n) {
        if (*(unsigned char *)(c + i + 0x4f1e) != *(unsigned char *)(c + i + 0x4f42)) {
            found++;
            break;
        }
        i++;
    }
    if (found != 0) return;
    _ZN5Sound12PlayBank2_2DEj(0x1cd);
}
