extern void _Z14ApproachLinearRiii(int *dst, int target, int step);
extern void Vec2_Sub(int *o, int *a, int *b);
struct M2 { int x, y; };
extern struct M2 data_ov004_020bc7f8;
void func_ov004_020b4ff0(char *c) {
    int v[2];
    _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10), *(int*)(c + 0x18));
    _Z14ApproachLinearRiii((int*)(c + 0xc), *(int*)(c + 0x14), *(int*)(c + 0x1c));
    Vec2_Sub(v, (int*)(c + 0x10), (int*)(c + 8));
    if (v[0] != 0) return;
    if (v[1] == 0) *(struct M2*)c = data_ov004_020bc7f8;
}
