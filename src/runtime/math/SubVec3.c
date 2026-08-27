struct Vec3 { int x, y, z; };
void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c)
{
    c->x = a->x - b->x;
    c->y = a->y - b->y;
    c->z = a->z - b->z;
}
