struct Vector3 { int x, y, z; };

struct Data02086f2c { int unk0, unk4, unk8; };
extern struct Data02086f2c data_02086f2c;

extern unsigned int func_020093f4(void *p, int x);
extern void Vec3_RotateYAndTranslate(struct Vector3 *out, void *a, short ang, int *t);
extern void _ZN11RaycastLineC1Ev(void *self);
extern void func_0200897c(void *self, void *arg);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, const struct Vector3 *a, const struct Vector3 *b, void *actor);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void _ZN11RaycastLine10GetClsnPosEv(struct Vector3 *out, void *self);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, const struct Vector3 *p, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void SubVec3(struct Vector3 *a, void *b, struct Vector3 *c);
extern unsigned int func_020093d4(void *p, int a);
extern int Vec3_HorzLen(void *v);
extern void Vec3_MulScalar(void *out, void *in, int scale);
extern void AddVec3(struct Vector3 *a, void *b, struct Vector3 *c);
extern void Vec3_Sub(struct Vector3 *out, struct Vector3 *a, void *b);
extern void Math_Function_0203b14c(int *a, int b, int c, int d, int e);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void _ZN13RaycastGroundD1Ev(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _Z14ApproachLinearRiii(int *r, int b, int c);

int func_0200bec4(char *self, int *arg1, int arg2, char *arg3, int arg4)
{
    int r5 = arg4;
    struct Vector3 sp8;
    struct Vector3 sp14;
    struct Vector3 sp20;
    struct Vector3 sp2C;
    struct Vector3 sp38;
    struct Vector3 sp44;
    struct Vector3 sp50;
    struct Vector3 sp5C;
    struct Vector3 sp68;
    char sp74[0x78];
    char spEC[0x54];
    int r4;
    int t;
    int sl;

    sp44.z = func_020093f4(self, *(int*)(arg3 + 0x14));
    data_02086f2c.unk8 = sp44.z;
    sp44.x = data_02086f2c.unk0;
    sp44.y = data_02086f2c.unk4;
    Vec3_RotateYAndTranslate(&sp8, arg1, *(short*)(*(char**)(self + 0x110) + 0x8e), (int*)&sp44);
    _ZN11RaycastLineC1Ev(sp74);
    sp14.x = sp8.x;
    sp14.y = sp8.y + 0x32000;
    sp14.z = sp8.z;
    {
        int tz = arg1[2];
        int ty = arg1[1] + 0x32000;
        int tx = arg1[0];
        sp20.x = tx;
        sp20.y = ty;
        sp20.z = tz;
    }
    func_0200897c(self, sp74);
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(sp74, &sp20, &sp14, 0);
    if (_ZN11RaycastLine10DetectClsnEv(sp74) != 0) {
        _ZN11RaycastLine10GetClsnPosEv(&sp50, sp74);
        sp8.x = sp50.x;
        sp8.y = sp50.y;
        sp8.z = sp50.z;
    }
    sp2C.x = sp8.x;
    sp2C.y = sp8.y + 0x100000;
    sp2C.z = sp8.z;
    func_0200897c(self, sp74);
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(sp74, &sp8, &sp2C, 0);
    if (_ZN11RaycastLine10DetectClsnEv(sp74) != 0) {
        _ZN11RaycastLine10GetClsnPosEv(&sp5C, sp74);
        sp2C.x = sp5C.x;
        sp2C.y = sp5C.y;
        sp2C.z = sp5C.z;
    }
    _ZN13RaycastGroundC1Ev(spEC);
    func_0200897c(self, spEC);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(spEC, &sp2C, 0);
    r4 = sp8.y;
    if (_ZN13RaycastGround10DetectClsnEv(spEC) != 0) {
        int d;
        r4 = *(int*)(spEC + 0x44);
        d = r4 - sp8.y;
        if (d > 0) {
            if (d > 0x30000)
                d = 0x30000;
            sp8.y += d;
        }
    }
    SubVec3(&sp8, arg1, &sp8);
    t = *(int*)(arg3 + 0x10);
    if (*(int*)(self + 0x154) & 0x20000) {
        t = (int)(((long long)t * 0x3000 + 0x800) >> 12);
    }
    sp8.y += func_020093d4(self, t);
    sl = Vec3_HorzLen(&sp8);
    Vec3_MulScalar(&sp68, self + 0x120, *(int*)(self + 0x130));
    AddVec3(&sp8, &sp68, &sp8);
    if (*(int*)(self + 0x154) & 4) {
        *(int*)(self + 0xa4) = sp8.x;
        *(int*)(self + 0xa8) = sp8.y;
        *(int*)(self + 0xac) = sp8.z;
    } else {
        int hl = Vec3_HorzLen(self + 0xa4) - sl;
        if (*(unsigned short*)(arg3 + 0x26) & 0x40) {
            r5 = 0x10000;
        } else if (hl > 0) {
            r5 += (int)(((long long)hl * 0x400 + 0x800) >> 12);
        }
        Vec3_Sub(&sp38, &sp8, self + 0xa4);
        sl = Vec3_HorzLen(&sp38);
        {
            int f = sl;
            int fd;
            if (sl != 0) {
                Math_Function_0203b14c(&f, 0, 0x100, (int)(((long long)r5 * 0x600 + 0x800) >> 12), 0x1000);
                fd = _ZN4cstd4fdivEii(f, sl);
                *(int*)(self + 0xa4) = sp8.x - (int)(((long long)sp38.x * fd + 0x800) >> 12);
                *(int*)(self + 0xac) = sp8.z - (int)(((long long)sp38.z * fd + 0x800) >> 12);
            }
        }
        {
            int a8 = *(int*)(self + 0xa8);
            int diff = sp8.y - a8;
            *(int*)(self + 0xa8) = sp8.y - (int)(((long long)diff * 0xf80 + 0x800) >> 12);
        }
        if (*(int*)(self + 0x154) & 0x400) {
            _ZN13RaycastGroundD1Ev(spEC);
            _ZN11RaycastLineD1Ev(sp74);
            return r4;
        }
    }
    *(int*)(self + 0x80) = arg1[0] + *(int*)(self + 0xa4);
    *(int*)(self + 0x88) = arg1[2] + *(int*)(self + 0xac);
    {
        int sl2 = arg1[1] + *(int*)(self + 0xa8);
        int r5b = sl2 - *(int*)(self + 0x84);
        if (r5b < (int)func_020093d4(self, *(int*)(arg3 + 0x18))) {
            _Z14ApproachLinearRiii((int*)(self + 0x84), sl2, 0x64000);
        } else {
            int r6 = arg2 - arg1[1];
            if (r6 >= 0) {
                r6 = 0;
            } else {
                int lim = func_020093d4(self, -0xf0a00);
                if (r6 < lim)
                    r6 = lim;
            }
            {
                int r2 = r6 + r5b;
                if (r2 > 0) {
                    *(int*)(self + 0x84) = sl2 - (r5b - (int)(((long long)r2 * 0x600 + 0x800) >> 12));
                }
            }
        }
    }
    _ZN13RaycastGroundD1Ev(spEC);
    _ZN11RaycastLineD1Ev(sp74);
    return r4;
}