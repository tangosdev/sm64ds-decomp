// NONMATCHING: case1 tail regperm/schedule only (div=7); control flow + body otherwise aligned
typedef unsigned char u8;
typedef unsigned int u32;

struct V3 { int x, y, z; };

extern void func_020383fc(char *p);
extern char *data_0209f318;
extern int data_0209b454;
extern void _ZN6Camera9SetFlag_3Ev(char *cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(char *cam, struct V3 *v);
extern void _ZN6Camera6SetPosERK7Vector3(char *cam, struct V3 *v);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int c, int d, int e, void *f, void *g);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(char *p);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(char *p);
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern void func_02012694(int a, char *v, int c);
extern void func_ov089_02131df4(char *thiz, char *player);

void func_ov089_02131b18(char *c)
{
    char *cam = data_0209f318;
    struct V3 v;
    u8 st;

    func_020383fc(c + 0x260);

    st = *(u8 *)(c + 0x442);
    if (st == 0)
        goto case0;
    if (st == 1)
        goto case1;
    if (st == 2)
        goto case2;
    return;

case0:
    {
        int *b0 = (int *)(unsigned)((unsigned long long)(unsigned)(c + 0xb0));
        int *s1 = (int *)(unsigned)((unsigned long long)(unsigned)(cam + 0x80));
        int *s2 = (int *)(unsigned)((unsigned long long)(unsigned)(cam + 0x8c));
        *b0 |= 0x4000000;
        data_0209b454 |= 0x4000000;
        *(int *)(c + 0x44c) = s1[0];
        *(int *)(c + 0x450) = s1[1];
        *(int *)(c + 0x454) = s1[2];
        *(int *)(c + 0x458) = s2[0];
        *(int *)(c + 0x45c) = s2[1];
        *(int *)(c + 0x460) = s2[2];
        _ZN6Camera9SetFlag_3Ev(cam);
        v.x = *(int *)(c + 0x5c);
        v.y = *(int *)(c + 0x60);
        v.z = *(int *)(c + 0x64);
        v.y += 0x64000;
        _ZN6Camera9SetLookAtERK7Vector3(cam, &v);
        {
            u8 *p = (u8 *)(unsigned)((unsigned long long)(unsigned)(c + 0x442));
            *p = *p + 1;
        }
        return;
    }

case1:
    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    v.y += 0x64000;
    _ZN6Camera9SetLookAtERK7Vector3(cam, &v);
    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    v.y += 0x1c000;
    *(void **)(c + 0x464) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32 *)(c + 0x464), 0x81, v.x, v.y, v.z, 0, 0);
    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x260)) {
        int t = (-*(int *)(c + 0xa8)) >> 1;
        *(int *)(c + 0xa8) = t;
        func_02012694(0x36, c + 0x74, t);
        return;
    }
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x260) == 0)
        return;
    _ZN6Camera9SetLookAtERK7Vector3(cam, (struct V3 *)(unsigned)((unsigned long long)(unsigned)(c + 0x44c)));
    _ZN6Camera6SetPosERK7Vector3(cam, (struct V3 *)(unsigned)((unsigned long long)(unsigned)(c + 0x458)));
    {
        int *cf = (int *)(unsigned)((unsigned long long)(unsigned)(cam + 0x154));
        int *b0 = (int *)(unsigned)((unsigned long long)(unsigned)(c + 0xb0));
        u8 *stp;
        int *gp;
        int gv;
        int *p238;
        int z;
        u8 s;
        *cf &= ~8;
        {
            int t = *b0;
            *b0 = t & ~0x4000000;
        }
        /* demand order: offset 0x442, &data_0209b454, stateptr, globval, p238, zero */
        stp = (u8 *)(unsigned)((unsigned long long)(unsigned)(c + 0x442));
        gp = &data_0209b454;
        gv = *gp;
        *gp = gv & ~0x4000000;
        {
            u8 s = *stp;
            int *p238 = (int *)(unsigned)((unsigned long long)(unsigned)(c + 0x238));
            int z = 0;
            int t = *p238;
            *stp = s + 1;
            *p238 = t & ~1;
            *(int *)(c + 0x464) = z;
        }
    }
    return;

case2:
    {
        u32 id = *(u32 *)(c + 0x244);
        char *found;
        if (id == 0)
            return;
        found = _ZN5Actor10FindWithIDEj(id);
        if (found == 0)
            return;
        if ((*(int *)(c + 0x240) & 0x400000) == 0)
            return;
        func_ov089_02131df4(c, found);
        {
            u8 *p = (u8 *)(unsigned)((unsigned long long)(unsigned)(c + 0x442));
            *p = *p + 1;
        }
        return;
    }
}
