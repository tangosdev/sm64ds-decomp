extern int func_02053274(int a, int b);

struct S { char pad[8]; unsigned short w8; unsigned short wa; };

int func_ov007_020c2e90(struct S *s, int b, int c, int d, int e)
{
    int r;
    if (s->w8 == 0) goto ret1;
    if (d < 0) goto ret1;
    if (e < 0) goto ret1;
    if (s->w8 >= s->wa) goto ret0a;
    r = func_02053274(b, c);
    if ((d << 12) > r) goto ret0b;
    if (r <= (e << 12)) return 1;
    goto ret0b;
ret0b:
    return 0;
ret0a:
    return 0;
ret1:
    return 1;
}
