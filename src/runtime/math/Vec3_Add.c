struct Vec3 { int x, y, z; };
void Vec3_Add(struct Vec3* out, struct Vec3* a, struct Vec3* b) {
    int ax = a->x, bx = b->x;
    int az = a->z, bz = b->z;
    int ay = a->y, by = b->y;
    out->x = ax + bx;
    out->y = ay + by;
    out->z = az + bz;
}
