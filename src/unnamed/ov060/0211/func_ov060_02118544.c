extern int Vec3_HorzLen(void *v);
extern int Vec3_Dist(const void *a, const void *b);
int func_ov060_02118544(char *c, void *v) {
    int h, r2;
    if (*(int*)(c + 0x170) != 0) return 0;
    h = Vec3_HorzLen(v);
    r2 = *(int*)(c + 0x180);
    if (h >= r2 - 0x12c000 && h <= r2 + 0x12c000) {
        if (Vec3_Dist(c + 0x174, v) < *(int*)(c + 0x184)) return 1;
    }
    return 0;
}
