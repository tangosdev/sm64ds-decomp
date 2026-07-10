// NONMATCHING: large 6-case state machine (size 0x4ac vs 0x4ec); structural first pass from disasm
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct V3 { int x, y, z; };

extern char *data_0209f318;
extern int data_0209b454;
extern int data_02111b68; /* cutscene flag */
extern void func_020383fc(char *p);
extern void _ZN6Camera9SetFlag_3Ev(char *cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(char *cam, struct V3 *v);
extern void _ZN6Camera6SetPosERK7Vector3(char *cam, struct V3 *v);
extern char *_ZN5Actor13ClosestPlayerEv(char *c);
extern void func_ov089_02130fb4(char *c, int *p, int n);
extern int Vec3_HorzDist(const void *a, const void *b);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(char *p);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(char *p);
extern void func_02012694(int a, char *v, int c);
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern void func_ov089_02131df4(char *c, char *p);
extern void _ZN9Animation7AdvanceEv(void *a);
extern int _ZN9Animation8FinishedEv(void *a);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int x, int y, int z, const void *v, void *cb);

void func_ov089_0213162c(char *c)
{
    char *cam = data_0209f318;
    struct V3 v;
    u8 st = *(u8 *)(c + 0x442);

    if (st < 3) {
        v.x = *(int *)(c + 0x5c);
        v.y = *(int *)(c + 0x60);
        v.z = *(int *)(c + 0x64);
        v.y += 0x1c000;
        *(void **)(c + 0x464) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32 *)(c + 0x464), 0x81, v.x, v.y, v.z, 0, 0);
    }
    if (st >= 2)
        func_020383fc(c + 0x260);

    st = *(u8 *)(c + 0x442);
    if (st > 5)
        return;

    switch (st) {
    case 0: {
        int *b0 = (int *)(c + 0xb0);
        int *s1 = (int *)(cam + 0x80);
        int *s2 = (int *)(cam + 0x8c);
        char *player;
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
        player = _ZN5Actor13ClosestPlayerEv(c);
        {
            int *ps = (int *)(player + 0x5c);
            v.x = ps[0];
            v.y = ps[1];
            v.z = ps[2];
            v.y += 0x78000;
        }
        _ZN6Camera6SetPosERK7Vector3(cam, &v);
        *(int *)(c + 0x428) = v.x;
        *(int *)(c + 0x430) = -v.z;
        *(int *)(c + 0x42c) = 0x64000;
        {
            int x = v.x;
            int lim = 0x1f4000;
            if (x >= lim)
                *(int *)(c + 0x428) = x - 0x12c000;
            else if (x <= -lim)
                ; /* keep */
            else
                *(int *)(c + 0x428) = x;
        }
        {
            int z = v.z;
            int az = z < 0 ? -z : z;
            if (az < 0xc8000) {
                *(int *)(c + 0x430) = -z;
                if (z >= 0)
                    *(int *)(c + 0x430) -= 0xc8000;
                else
                    *(int *)(c + 0x430) += 0xc8000;
            } else {
                *(int *)(c + 0x430) = -z;
            }
        }
        {
            int p[3];
            p[0] = *(int *)(c + 0x428);
            p[1] = *(int *)(c + 0x42c);
            p[2] = *(int *)(c + 0x430);
            func_ov089_02130fb4(c, p, 0x190000);
        }
        *(u8 *)(c + 0x442) = *(u8 *)(c + 0x442) + 1;
        return;
    }
    case 1: {
        int d;
        _ZN6Camera9SetLookAtERK7Vector3(cam, &v); /* ROM uses leftover stack lookat */
        d = Vec3_HorzDist(c + 0x5c, (void *)(c + 0x428));
        if (d >= *(int *)(c + 0x98))
            return;
        *(int *)(c + 0x5c) = *(int *)(c + 0x428);
        *(int *)(c + 0x60) = *(int *)(c + 0x42c);
        *(int *)(c + 0x64) = *(int *)(c + 0x430);
        *(int *)(c + 0x60) += 0xa000;
        *(int *)(c + 0xa8) = 0x23000;
        *(int *)(c + 0x9c) = -0x2000;
        *(int *)(c + 0x98) = 0;
        *(u8 *)(c + 0x442) = *(u8 *)(c + 0x442) + 1;
        return;
    }
    case 2:
        _ZN6Camera9SetLookAtERK7Vector3(cam, &v);
        if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x260)) {
            int t = (-*(int *)(c + 0xa8)) >> 1;
            *(int *)(c + 0xa8) = t;
            func_02012694(0x36, c + 0x74, t);
            return;
        }
        if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x260) == 0)
            return;
        *(u8 *)(c + 0x442) = *(u8 *)(c + 0x442) + 1;
        *(int *)(c + 0x238) &= ~1;
        *(void **)(c + 0x464) = 0;
        *(u16 *)(c + 0x100) = 0;
        data_02111b68 = 1;
        return;
    case 3: {
        u16 *tm = (u16 *)(c + 0x100);
        *tm = *tm + 1;
        if (*tm < 0x1e)
            return;
        _ZN6Camera9SetLookAtERK7Vector3(cam, (struct V3 *)(c + 0x44c));
        _ZN6Camera6SetPosERK7Vector3(cam, (struct V3 *)(c + 0x458));
        {
            int *cf = (int *)(cam + 0x154);
            *cf &= ~8;
        }
        {
            int *b0 = (int *)(c + 0xb0);
            *b0 &= ~0x4000000;
        }
        data_0209b454 &= ~0x4000000;
        *(u8 *)(c + 0x442) = *(u8 *)(c + 0x442) + 1;
        return;
    }
    case 4: {
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
        *(u8 *)(c + 0x442) = *(u8 *)(c + 0x442) + 1;
        return;
    }
    case 5: {
        char *pl = *(char **)(c + 0x110);
        int *s = (int *)(pl + 0x5c);
        *(int *)(c + 0x5c) = s[0];
        *(int *)(c + 0x60) = s[1];
        *(int *)(c + 0x64) = s[2];
        {
            int ang = *(short *)(*(char **)(c + 0x110) + 0x8e);
            *(short *)(c + 0x8e) = ang;
        }
        _ZN9Animation7AdvanceEv(c + 0x164);
        if (_ZN9Animation8FinishedEv(c + 0x164))
            *(u8 *)(c + 0x442) = *(u8 *)(c + 0x442) + 1;
        return;
    }
    }
}
