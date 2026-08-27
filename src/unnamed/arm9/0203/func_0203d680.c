typedef struct { int x, y; } Vec2_Fix12;

void func_0203d680(Vec2_Fix12 *out, const Vec2_Fix12 *in, int scale)
{
    int rx = (int)(((long long)in->x * scale + 0x800) >> 12);
    int ry = (int)(((long long)in->y * scale + 0x800) >> 12);
    out->x = rx;
    out->y = ry;
}
