typedef struct { int x, y, z; } Vec3;
typedef int (**VT)(void*);

struct Self {
    char pad0[0x9c];
    Vec3 pos;                /* 0x9c */
    Vec3 vel;                /* 0xa8 */
    char padb4[0xd8 - 0xb4];
    int d8;                  /* 0xd8 */
};

extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void Vec3_Add(Vec3* out, Vec3* a, Vec3* b);
extern int LenVec3(Vec3* v);
extern int DotVec3(Vec3* a, Vec3* b);
extern int NormalizeVec3IfNonZero(Vec3* v);
extern void func_ov006_020c4f68(struct Self* self);
extern char* data_ov006_021403f4[];

#define O  (data_ov006_021403f4[i])
#define OM (((char* volatile*)data_ov006_021403f4)[i])

void func_ov006_020c49d8(struct Self* self)
{
    int i;
    int lim = (int)(((long long)self->d8 * 0x12000 + 0x800) >> 12);
    int half = lim >> 1;

    for (i = 0; i < 3; i++) {
        Vec3 a;
        Vec3 b;
        Vec3 d;
        Vec3 c;

        if ((*(VT*)O)[2](O) == 0) continue;

        Vec3_Sub(&a, (Vec3*)(*(VT*)O)[0](O), &self->pos);
        a.y -= half;
        if (a.y < 0) {
            if (a.y > -0x18000) a.y = 0;
            else a.y += 0x18000;
        } else {
            if (a.x > 0x2000) a.x -= 0x2000;
            else if (a.x < -0x2000) a.x += 0x2000;
            else a.x = 0;
        }

        Vec3_Sub(&b, (Vec3*)(*(VT*)O)[1](O), &self->vel);
        Vec3_Add(&c, &a, &b);
        if (LenVec3(&c) > lim) continue;

        if (NormalizeVec3IfNonZero(&a) == 0) {
            *(short*)(OM + 0x10) = 1;
            *(int*)(O + 4) = self->pos.x;
            *(int*)(O + 8) = self->pos.y;
            *(int*)(O + 0xc) = self->pos.z;
            func_ov006_020c4f68(self);
        }

        d.x = 0;
        d.y = 0x1000;
        d.z = 0;
        if (DotVec3(&d, &a) > 0) {
            *(short*)(OM + 0x10) = 1;
            *(int*)(O + 4) = self->pos.x;
            *(int*)(O + 8) = self->pos.y;
            *(int*)(O + 0xc) = self->pos.z;
            func_ov006_020c4f68(self);
            return;
        } else {
            *(short*)(OM + 0x10) = 2;
            *(int*)(O + 4) = self->pos.x;
            *(int*)(O + 8) = self->pos.y;
            *(int*)(O + 0xc) = self->pos.z;
        }
    }
}
