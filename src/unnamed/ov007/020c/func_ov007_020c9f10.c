typedef struct { int x, y, z; } Vec3;
typedef struct {
    int pad0[3];
    Vec3 a;
    int pad1[2];
    Vec3 b;
    Vec3 c;
    Vec3 d;
    Vec3 e;
} T;
void func_ov007_020c9f10(T* self, Vec3* pos, int w, int h)
{
    Vec3* pe = &self->e;
    Vec3* pd = &self->d;
    Vec3* pc = (Vec3*)(((int)self + 0x2c));
    int* qd = (int*)(((int)self + 0x3c));
    int* qe = (int*)(((int)self + 0x48));
    *pe = *pos;
    *pd = *pe;
    *pc = *pd;
    {
        Vec3* pb = (Vec3*)(((int)self + 0x20));
        *pb = *pc;
        self->a = *pb;
        pb->x -= w / 2;
        pc->x += w / 2;
        *qd -= h / 2;
        *qe += h / 2;
    }
}
