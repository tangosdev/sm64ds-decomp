extern void Vec3_Sub(void *out, void *a, void *b);
extern void Vec3_MulScalar(void *out, void *v, int s);
extern void AddVec3(void *a, void *b, void *c);

void func_ov014_021122dc(char *c)
{
    char *p;
    int i;
    int diff[3];
    int scaled[3];

    Vec3_Sub(diff, c + 0x5c, c + 0x524);
    p = c + 0x524;
    i = 0;
    do {
        Vec3_MulScalar(scaled, diff, (7 - i) / 7);
        AddVec3(p, scaled, p);
        i++;
        p += 0xc;
    } while (i < 7);
}
