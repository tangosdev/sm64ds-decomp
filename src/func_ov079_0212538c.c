// Per-frame update: advance two sub-animations, seed a velocity from a table when a
// phase counter is zero, bail early below phase 8, then integrate an angle and derive a
// clamped speed (two variants by a mode flag). Sibling of func_ov079_02124530.
extern void func_ov079_g0(void *p);
extern void func_ov079_g1(void *p);
extern void func_ov079_g2(void *p);
extern void func_0201267c(int a, void *p);
extern int data_021275e4[];

void func_ov079_0212538c(char *c)
{
    func_ov079_g0(c + 0x418);
    func_ov079_g1(c + 0x418);
    func_ov079_g2(c + 0x31c);
    if (*(unsigned short *)(c + 0x100) == 0)
        *(int *)(c + 0xa8) = data_021275e4[*(unsigned char *)(c + 0x414)];
    if (*(unsigned short *)(c + 0x100) < 8) return;
    short *a = (short *)(c + 0x8c);
    if (*(unsigned char *)(c + 0x414)) {
        int *q = (int *)(((int)c + 0x3c8) & 0xFFFFFFFFFFFFFFFF);
        *q += 0x130;
    } else {
        int *q = (int *)(((int)c + 0x3c8) & 0xFFFFFFFFFFFFFFFF);
        *q += 0x100;
    }
    *a = *a + *(int *)(c + 0x3c8);
    if (*(short *)(c + 0x8c) > 0x4000) {
        *(int *)(c + 0x3c8) = 0;
        *(short *)(c + 0x8c) = 0x4000;
        *(int *)(c + 0x3b0) = 5;
        func_0201267c(0xc6, c + 0x74);
    }
    if (*(unsigned char *)(c + 0x414)) {
        *(int *)(c + 0x410) = (short)*(short *)(c + 0x8c) * (short)0x15;
        if (*(int *)(c + 0x410) >= 0x53000) *(int *)(c + 0x410) = 0x53000;
    } else {
        *(int *)(c + 0x410) = (short)*(short *)(c + 0x8c) * (short)0xa;
        if (*(int *)(c + 0x410) >= 0x25000) *(int *)(c + 0x410) = 0x25000;
    }
}
