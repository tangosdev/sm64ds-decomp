enum { false, true };

typedef struct { int x, y, z; } Vec3;
typedef struct { int m[12]; } Mtx43;

struct Entry {
    void *sfp;
    void *sfp2;
    signed char b8;
    signed char b9;
    short pad;
    int pad2;
};

extern int data_ov100_02148710;
extern Vec3 data_ov100_021487c0;
extern void func_020072c0(void);
extern void *data_ov100_021487b4;
extern void func_020731dc(void *a, void *b, void **node);
extern void Vec3_RotateYAndTranslate(void *d, void *s, short ang, void *t);
extern struct Entry data_ov100_02148204[];
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern unsigned char data_0209f2d8;
extern int data_ov002_0211094c;
extern void *func_02132894[];
extern void LoadKeyModels(int idx);
extern int data_ov089_02132c50;
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern int data_ov100_02148744;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, int fix, unsigned int j);
extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern Mtx43 data_020a0e68;
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
extern void *_Znwj(unsigned int sz);
extern void *_ZN5ModelC1Ev(void *self);
extern unsigned char data_0209f250;
extern int data_0209f394[];
extern void func_ov100_02145370(char *c);
extern int data_ov100_02148914;
extern int data_ov100_021488b4;
extern void func_ov100_021453d8(char *c, void *p, int a2);

int func_ov100_021455a0(char *c)
{
    unsigned int idx;
    struct Entry *e;
    void *f;
    void *an;
    void *m;
    Vec3 tmp;
    int r4;
    int v;
    int b;
    int x;
    int y;
    int z;

    *(unsigned int *)(c + 8) = *(unsigned int *)(c + 8) >> 0x10;

    if (!(data_ov100_02148710 & 1)) {
        data_ov100_021487c0.x = 0x4b000;
        data_ov100_021487c0.y = 0;
        data_ov100_021487c0.z = 0;
        func_020731dc(&data_ov100_021487c0, (void *)func_020072c0, &data_ov100_021487b4);
        data_ov100_02148710 |= 1;
    }

    Vec3_RotateYAndTranslate(c + 0x5c, c + 0x5c, *(short *)(c + 0x8e), &data_ov100_021487c0);

    idx = *(unsigned int *)(c + 8);
    e = &data_ov100_02148204[idx];
    f = _ZN5Model8LoadFileER13SharedFilePtr(e->sfp);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, 1);

    b = data_0209f2d8;
    b = b == 0;
    if (b != false) {
        if (e->b8 > 0) {
            *(void **)(c + 0x13c) = &data_ov002_0211094c;
        } else if (e->b9 >= 0) {
            unsigned int t = *(unsigned int *)(c + 8);
            if (t >= 9 && t <= 0xd) {
                *(signed char *)(c + 0x144) = (signed char)(t - 8);
                LoadKeyModels(*(signed char *)(c + 0x144) + 1);
                *(void **)(c + 0x13c) = func_02132894[*(signed char *)(c + 0x144) + 1];
                if (*(int *)(c + 8) == 0xc)
                    *(signed char *)(c + 0x144) = 0;
            } else {
                *(void **)(c + 0x13c) = &data_ov089_02132c50;
            }
        }
        if (*(void **)(c + 0x13c) != 0)
            _ZN5Model8LoadFileER13SharedFilePtr(*(void **)(c + 0x13c));
    }

    an = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov100_02148744);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, an, 0x40000000, 0x1000, 0);

    Vec3_Asr(&tmp, (Vec3 *)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, tmp.x, tmp.y, tmp.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short *)(c + 0x8e));
    *(Mtx43 *)(c + 0xf0) = data_020a0e68;

    if (e->sfp2 != 0) {
        m = _Znwj(0x50);
        if (m != 0)
            m = _ZN5ModelC1Ev(m);
        *(void **)(c + 0x138) = m;
        f = _ZN5Model8LoadFileER13SharedFilePtr(e->sfp2);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(*(void **)(c + 0x138), f, 1, -1);
        *(Mtx43 *)((char *)*(void **)(c + 0x138) + 0x1c) = data_020a0e68;
    }

    {
        int w;
        y = *(int *)(c + 0x60);
        z = *(int *)(c + 0x64);
        x = *(int *)(c + 0x5c);
        w = y + 0xb4000;
        *(int *)(c + 0xa4) = x;
        *(int *)(c + 0xa8) = w;
        {
            unsigned char bi = data_0209f250;
            *(int *)(c + 0xac) = z;
            r4 = data_0209f394[bi];
        }
    }
    func_ov100_02145370(c);

    v = *(int *)(c + 0x80);
    if (v < 0)
        v = -v;
    if (v > 0x4b000)
        goto big;
    v = *(int *)(c + 0x84);
    if (v < 0)
        v = -v;
    if (v > 0x32000)
        goto big;
    v = *(int *)(c + 0x88);
    if (v < 0)
        v = -v;
    if (v > 0x1f4000)
        goto big;

    func_ov100_021453d8(c, &data_ov100_02148914, r4);
    goto done;
big:
    func_ov100_021453d8(c, &data_ov100_021488b4, r4);
done:
    return 1;
}
