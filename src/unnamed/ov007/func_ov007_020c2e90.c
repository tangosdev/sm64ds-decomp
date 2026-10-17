extern int func_02053274(int a, int b);

struct S { char pad[8]; unsigned short w8; unsigned short wa; };

int func_ov007_020c2e90(struct S *s, int b, int c, int d, int e)
{
    int r;
    if (s->w8 != 0 && d >= 0 && e >= 0) {
        if (s->w8 < s->wa) {
            r = func_02053274(b, c);
            return (d << 12) <= r && r <= (e << 12);
        }
        return 0;
    }
    return 1;
}
