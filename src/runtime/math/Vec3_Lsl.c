struct Vec3 { int x, y, z; };
void Vec3_Lsl(struct Vec3* d, struct Vec3* s, int sh){
    int a = s->x;
    int b = s->y;
    int c = s->z;
    d->x = a << sh;
    d->y = b << sh;
    d->z = c << sh;
}
