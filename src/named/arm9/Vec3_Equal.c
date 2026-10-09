struct V3 { int x, y, z; };

int Vec3_Equal(struct V3 *a, struct V3 *b)
{
    if (a->x == b->x && a->y == b->y && a->z == b->z) return 1;
    return 0;
}
