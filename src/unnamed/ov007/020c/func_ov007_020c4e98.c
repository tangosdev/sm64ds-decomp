void func_ov007_020c4fcc(int r0, int r1);
void func_ov007_020c19cc(int r0, int r1, int r2, int r3);

void func_ov007_020c4e98(int *thiz, int a1, int a2, int a3, int idx,
                         unsigned short a5, int a6)
{
    int i;
    int *base;
    base = (int*)thiz[0x28/4];
    func_ov007_020c4fcc(a2, a3);
    for (i = 0; i < thiz[8/4]; i++) {
        if (i == idx) continue;
        func_ov007_020c19cc(
            *(unsigned short*)(((int*)thiz[0x38/4])[i] + 8),
            base[i], a1, 0x1000);
    }
    func_ov007_020c19cc(
        *(unsigned short*)(thiz[0x20/4] + 8),
        thiz[0x24/4], a1, 0x1000);
    func_ov007_020c4fcc(a5, a6);
    func_ov007_020c19cc(
        *(unsigned short*)(((int*)thiz[0x38/4])[idx] + 8),
        base[idx], a1, 0x1000);
}
