extern void func_020551f0(void *s, int v);
extern int func_ov075_0211a194(char *self, void *src);

void func_ov075_02119cc0(char *self) {
    *(unsigned char *)(((long long)(int)(self + 0x284)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    if (*(unsigned char *)(self + 0x284) & 1) return;
    *(unsigned char *)(((long long)(int)(self + 0x283)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    func_020551f0((void *)0x4001050, *(unsigned char *)(self + 0x283));
    if (*(unsigned char *)(self + 0x283) < 0x10) return;
    *(unsigned char *)(self + 0x285) = 1;
    func_ov075_0211a194(self, *(void **)(self + 0x6c));
}
