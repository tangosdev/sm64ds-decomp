typedef int Fix12;
typedef struct Vec3 { Fix12 x, y, z; } Vec3;

typedef struct Obj Obj;
typedef struct ObjVt {
    Vec3 *(*f0)(Obj *);
    Vec3 *(*f1)(Obj *);
    int   (*f2)(Obj *);
} ObjVt;
struct Obj {
    ObjVt *vt;      /* 0x0 */
    Vec3 vec;       /* 0x4 */
    short flag;     /* 0x10 */
};

typedef struct Self {
    char _pad0[0x9c];
    Vec3 pos;       /* 0x9c */
    Vec3 vel;       /* 0xa8 */
    char _padb4[0xd8 - 0xb4];
    int speed;      /* 0xd8 */
} Self;

extern Obj *data_ov006_021403f4[3];
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);
extern Fix12 DotVec3(const Vec3 *a, const Vec3 *b);
extern int LenVec3(Vec3 *v);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern void func_ov006_020c4f68(Self *self);

void func_ov006_020c47d4(Self *self)
{
    int i;
    int thresh = (int)(((long long)self->speed * 0x12000 + 0x800) >> 12);

    for (i = 0; i < 3; i++) {
        Vec3 disp, rel, up, sum;
        Vec3 *p, *q;
        Obj *o;

        if (data_ov006_021403f4[i]->vt->f2(data_ov006_021403f4[i]) == 0)
            continue;

        p = data_ov006_021403f4[i]->vt->f0(data_ov006_021403f4[i]);
        Vec3_Sub(&disp, p, &self->pos);

        disp.y -= thresh >> 1;
        if (disp.y < 0) {
            if (disp.y > -0x18000)
                disp.y = 0;
            else
                disp.y += 0x18000;
        } else {
            if (disp.x > 0x2000)
                disp.x -= 0x2000;
            else if (disp.x < -0x2000)
                disp.x += 0x2000;
            else
                disp.x = 0;
        }

        q = data_ov006_021403f4[i]->vt->f1(data_ov006_021403f4[i]);
        Vec3_Sub(&rel, q, &self->vel);
        Vec3_Add(&sum, &disp, &rel);

        if (LenVec3(&sum) > thresh)
            continue;

        if (NormalizeVec3IfNonZero(&disp) == 0) {
            data_ov006_021403f4[i]->flag = 1;
            o = data_ov006_021403f4[i];
            o->vec.x = self->pos.x;
            o->vec.y = self->pos.y;
            o->vec.z = self->pos.z;
            func_ov006_020c4f68(self);
        }

        up.x = 0;
        up.y = 0x1000;
        up.z = 0;
        if (DotVec3(&up, &disp) <= 0)
            continue;

        if (rel.y >= 0)
            continue;

        data_ov006_021403f4[i]->flag = 1;
        o = data_ov006_021403f4[i];
        o->vec.x = self->pos.x;
        o->vec.y = self->pos.y;
        o->vec.z = self->pos.z;
        func_ov006_020c4f68(self);
        return;
    }
}
