//cpp
struct V3 { int x, y, z; };

extern "C" {
int DotVec3(const V3 *a, const V3 *b);
void Vec3_MulScalar(V3 *out, const V3 *in, int scale);
void SubVec3(V3 *a, V3 *b, V3 *c);
void AddVec3(V3 *a, V3 *b, V3 *c);
int _ZN4cstd4fdivEii(int a, int b);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisPtr, void *file, int i, int fix, unsigned int flags);
void func_ov006_020e6df0(int a0, int a1, int a2);
void func_ov006_020c97bc(char *c);
}

extern int data_ov006_02140574;
extern void *data_ov006_0213b22c[];
extern int data_ov006_021405b0;
extern int data_ov006_0213b1b4[2];

struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void m4();
};

extern "C" void func_ov006_020c9aa0(char *c)
{
    V3 tmp;
    V3 tmp2;
    V3 tmp3;
    int r5v;
    int r4v;
    int dot;
    int scale;
    int fdivr;
    int vx;

    r5v = *(int *)(c + 0x10);
    r4v = *(int *)(c + 0x14);
    if (r5v < 0) r5v = -r5v;
    {
        int *src = (int *)(((long long)(int)(c + 4)) & 0xFFFFFFFFFFFFFFFFLL);
        tmp.x = src[0];
        tmp.y = src[1];
        tmp.z = src[2];
    }

    dot = DotVec3((V3 *)(c + 0x3c), &tmp);
    scale = (int)(((long long)dot * 0x1200 + 0x800) >> 12);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    SubVec3((V3 *)(c + 0x3c), &tmp2, (V3 *)(c + 0x3c));

    fdivr = _ZN4cstd4fdivEii(r4v, r4v + r5v);
    scale = (int)(((long long)data_ov006_02140574 * fdivr + 0x800) >> 12);
    Vec3_MulScalar(&tmp3, &tmp, scale);
    AddVec3((V3 *)(c + 0x3c), &tmp3, (V3 *)(c + 0x3c));

    vx = *(int *)(c + 0x3c);
    if (vx < -0x2800) vx = -0x2800;
    else if (vx > 0x2800) vx = 0x2800;
    *(int *)(c + 0x3c) = vx;

    {
        int fx = (int)(((long long)data_ov006_02140574 * 0x600 + 0x800) >> 12);
        int cur = *(int *)(c + 0x40);
        if (cur >= fx) fx = cur;
        *(int *)(c + 0x40) = fx;
    }

    *(int *)(c + 0x48) = data_ov006_021405b0;
    ((VtObj *)c)->m4();

    *(int *)(c + 0x64) = 0;
    if (*(int *)(c + 0x60) != 0) {
        *(int *)(c + 0x60) = 0;
    } else {
        *(int *)(c + 0x60) = 1;
    }

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x78),
        *(void **)data_ov006_0213b22c[*(int *)(c + 0x60)], 0x40000000, 0x800, 0);

    *(int *)(c + 0xd0) = 0;
    func_ov006_020e6df0(0, *(int *)(c + 0x60), *(int *)(c + 0x24));

    {
        int a = data_ov006_0213b1b4[0];
        int b = data_ov006_0213b1b4[1];
        a = b ? a : a;
        *(int *)(c + 0x70) = a;
        *(int *)(c + 0x74) = b;
    }
    func_ov006_020c97bc(c);
}
