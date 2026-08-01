typedef long long s64;

struct P { int x; int y; };
struct B { struct P *p; int f4; int f8; int fc; int f10; int f14; };
struct J { struct B *a; struct B *b; int f8; int fc; int f10; };

#define FXM(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))
#define AT(p, off) (*(int *)(int)(((long long)(int)((char *)(p) + (off)))))

void func_ov007_020c7b2c(struct J *self, int m1, int m2)
{
    int dx;
    int dy;
    int nx;
    int ny;

    dx = self->a->p->x - self->b->p->x - self->fc;
    dy = self->a->p->y - self->b->p->y - self->f10;
    nx = -FXM(m1, dx);
    ny = -FXM(m1, dy);
    AT(self->a, 0x10) += nx - FXM(m2, self->a->f4);
    AT(self->a, 0x14) += ny - FXM(m2, self->a->f8);
    AT(self->b, 0x10) -= nx + FXM(m2, self->b->f4);
    AT(self->b, 0x14) -= ny + FXM(m2, self->b->f8);
}
