//cpp

extern "C" {
int func_020393b4(void *p);
int func_020393ac(void *p);
int func_0203939c(void *p);
int func_0203938c(void *p);
int func_02035354(void *a, void *b);
void func_02037fec(void *c, int p1, int p2, int p3, void *e);
void func_020379d0(void *c, int p1, int p2, int p3, void *e);
void func_0203799c(void *c, int p1, int p2, int p3, void *e);
void func_02037968(void *c, int p1, int p2, int p3, void *e);
void func_02037a38(void *o);
int Vec3_Dist(const void *a, const void *b);
}

extern void *data_020a0c80[];

class C {
public:
    virtual int v0();
    virtual int v1();
    virtual int v2();
    virtual int v3();
    virtual int v4();
    virtual int v5();
    virtual int v6();
    virtual int v7();
    virtual int v8(void *arg);
};

struct Vec3 { int x, y, z; };

struct Info {
    char pad0[0x5c];
    Vec3 pos;
    char pad1[0xb0 - 0x68];
    int pB0;
    int pB4;
    int pB8;
};

class SphereClsn {
public:
    int DetectClsn();
};

int SphereClsn::DetectClsn()
{
    int result = 0;
    int mask;
    char *e = (char *)data_020a0c80[0];
    if (e != 0) {
        if (func_02035354(this, (void *)func_020393b4(e)) == 0) {
            mask = ((C *)e)->v8(this);
            if (mask != 0) {
                func_02037fec((char *)this + 0x10, 0, func_020393ac(e), func_020393b4(e), e);
                if (mask & 1)
                    func_020379d0(this, 0, func_020393ac(e), func_020393b4(e), e);
                if (mask & 2)
                    func_0203799c(this, 0, func_020393ac(e), func_020393b4(e), e);
                if (mask & 4)
                    func_02037968(this, 0, func_020393ac(e), func_020393b4(e), e);
                result = 1;
            }
        }
    }

    e = (char *)this + 0x3c;
    int threshold = *(int *)((char *)this + 0x48) + 0x1000;
    for (int i = 1; i < 0x18; i++) {
        C *o = (C *)data_020a0c80[i];
        if (o == 0) continue;
        Info *info = (Info *)func_020393b4(o);
        if (func_02035354(this, info) != 0) continue;
        if (info != 0) {
            int active = (info->pB0 & 2) ? 1 : 0;
            if (active != 0) {
                Vec3 v;
                Vec3 *src = (Vec3 *)((unsigned long long)(unsigned int)((char *)info + 0x5c) & 0xFFFFFFFFFFFFFFFFULL);
                v.x = src->x;
                v.y = src->y;
                v.z = src->z;
                mask = func_0203939c(o);
                if (mask == -0x1000) {
                    int b4 = info->pB4;
                    int b8 = info->pB8;
                    v.y += b4;
                    mask = b8 << 3;
                } else {
                    v.y += func_0203938c(o);
                }
                if (Vec3_Dist(e, &v) > mask + threshold)
                    continue;
            }
        }
        {
            { int t = o->v8(this); mask = t; }
            if (mask != 0) {
                func_02037fec((char *)this + 0x10, i, func_020393ac(o), func_020393b4(o), o);
                if (mask & 1)
                    func_020379d0(this, i, func_020393ac(o), func_020393b4(o), o);
                if (mask & 2)
                    func_0203799c(this, i, func_020393ac(o), func_020393b4(o), o);
                if (mask & 4)
                    func_02037968(this, i, func_020393ac(o), func_020393b4(o), o);
                result = 1;
            }
        }
    }
    func_02037a38(this);
    return result;
}
