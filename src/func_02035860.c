struct Vec3 { int x, y, z; };

void func_02035860(char *o, struct Vec3 *src)
{
    char *base = *(char **)(o + 0x14);
    struct Vec3 *d1 = (struct Vec3 *)(((long long)(int)(base + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    d1->x = src->x;
    d1->y = src->y;
    d1->z = src->z;
    struct Vec3 *d2 = (struct Vec3 *)(((long long)(int)(base + 0x68)) & 0xFFFFFFFFFFFFFFFFLL);
    d2->x = src->x;
    d2->y = src->y;
    d2->z = src->z;
}
