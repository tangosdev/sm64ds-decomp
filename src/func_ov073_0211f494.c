
typedef int Fix12;
typedef struct { int x, y, z; } Vec3;
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(Fix12 y, Fix12 x);
extern Fix12 Vec3_HorzLen(Vec3 *v);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12 x, Fix12 y, Fix12 z);
extern short data_02082214[];
void func_ov073_0211f494(char *a, char *b)
{
    Vec3 t, p, q, d;
    int ay, ax, vx, tmp, cos_ax, sin_ax, cos_ay, sin_ay, rx, ry, rz;
    int y0;
    int *bp = (int *)(((int)b + 0x5c) & 0xFFFFFFFFFFFFFFFF);

    p.x = *(int *)(a + 0x5c);
    y0 = *(int *)(a + 0x60);
    p.y = y0;
    {
        Vec3 *pq = &q;
        int zval = *(int *)(a + 0x64);
        Vec3 *pp = &p;
        pp->z = zval;
        pq->x = bp[0];
        pq->y = bp[1];
        pq->z = bp[2];
        {
            int qy = pq->y + 0x46000;
            int h = *(int *)(a + 0x118);
            vx = *(int *)(a + 0x114);
            pp->y = y0 + h;
            pq->y = qy;
            Vec3_Sub(&d, pq, pp);
        }
    }
    t.x = d.x; t.y = d.y; t.z = d.z;
    ay = _ZN4cstd5atan2E5Fix12IiES1_(t.x, t.z);
    ax = _ZN4cstd5atan2E5Fix12IiES1_(t.y, Vec3_HorzLen(&t));
    ax = (int)((unsigned short)ax >> 4);
    ay = (int)((unsigned short)ay >> 4);
    sin_ax = data_02082214[ax * 2 + 1];
    cos_ax = data_02082214[ax * 2];
    tmp = (int)(((long long)vx * sin_ax + 0x800) >> 12);
    ry = (int)(((long long)vx * cos_ax + 0x800) >> 12);
    cos_ay = data_02082214[ay * 2];
    sin_ay = data_02082214[ay * 2 + 1];
    rx = (int)(((long long)tmp * cos_ay + 0x800) >> 12);
    rz = (int)(((long long)tmp * sin_ay + 0x800) >> 12);
    p.x += rx; p.y += ry; p.z += rz;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x84, p.x, p.y, p.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x85, p.x, p.y, p.z);
}
