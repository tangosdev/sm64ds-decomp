//cpp
struct Vec3 { int x, y, z; };
extern "C" void Vec3_MulScalar(Vec3 *res, Vec3 *v, int scalar);
extern Vec3 data_ov006_0213af98;
struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(Vec3 *p);
};
extern "C" void func_ov006_020c4c54(int this_) {
    Vec3 v;
    int *p = (int*)(((int)this_ + 0x30) & 0xFFFFFFFFFFFFFFFF);
    Vec3 *d = &data_ov006_0213af98;
    if (p[0] == d->x) {
        if (p[1] == d->y)
            return;
        if (((volatile int*)this_)[0xc] == 0)
            return;
    }
    Vec3_MulScalar(&v, (Vec3*)(this_ + 0xcc), *(int*)(this_ + 0xd8));
    Sub *s = (Sub*)(this_ + 0x38);
    s->m5(&v);
}
