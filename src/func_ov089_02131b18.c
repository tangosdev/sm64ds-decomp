// NONMATCHING: different op / idiom (div=88). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned int u32;

struct V3 { int x, y, z; };

extern void func_020383fc(char *p);
extern char *CAMERA;
extern char *NEXT_ACTOR_UPDATE_FLAGS;
extern void Camera_SetFlag_3(char *cam);
extern void Camera_SetLookAt(char *cam, struct V3 *v);
extern void Camera_SetPos(char *cam, struct V3 *v);
extern char *Particle_System_New(u32 a, u32 b, int c, int d, int e, void *f, void *g);
extern int WithMeshClsn_JustHitGround(char *p);
extern int WithMeshClsn_IsOnGround(char *p);
extern char *Actor_FindWithID(u32 id);
extern void func_02012694(int a, char *v, int c);
extern void func_ov089_02131df4(char *thiz);

void func_ov089_02131b18(char *c)
{
    char *cam = *(char **)&CAMERA;
    struct V3 v;

    func_020383fc(c + 0x260);

    switch (*(u8 *)(c + 0x442)) {
    case 0:
    {
        int *s1 = (int *)(cam + 0x80);
        int *s2 = (int *)(cam + 0x8c);
        int *b0 = (int *)(c + 0xb0);
        *b0 |= 0x4000000;
        *(int *)&NEXT_ACTOR_UPDATE_FLAGS |= 0x4000000;
        *(int *)(c + 0x44c) = s1[0];
        *(int *)(c + 0x450) = s1[1];
        *(int *)(c + 0x454) = s1[2];
        *(int *)(c + 0x458) = s2[0];
        *(int *)(c + 0x45c) = s2[1];
        *(int *)(c + 0x460) = s2[2];
        Camera_SetFlag_3(cam);
        v.x = *(int *)(c + 0x5c);
        v.y = *(int *)(c + 0x60);
        v.z = *(int *)(c + 0x64);
        v.y += 0x64000;
        Camera_SetLookAt(cam, &v);
        *(u8 *)(c + 0x442) += 1;
        return;
    }
    case 1:
        v.x = *(int *)(c + 0x5c);
        v.y = *(int *)(c + 0x60);
        v.z = *(int *)(c + 0x64);
        v.y += 0x64000;
        Camera_SetLookAt(cam, &v);
        v.x = *(int *)(c + 0x5c);
        v.y = *(int *)(c + 0x60);
        v.z = *(int *)(c + 0x64);
        v.y += 0x1c000;
        *(char **)(c + 0x464) = Particle_System_New(
            *(u32 *)(c + 0x464), 0x81, v.x, v.y, *(int *)(c + 0x64), 0, 0);
        if (WithMeshClsn_JustHitGround(c + 0x260)) {
            int t = (-*(int *)(c + 0xa8)) >> 1;
            *(int *)(c + 0xa8) = t;
            func_02012694(0x36, c + 0x74, t);
            return;
        }
        if (WithMeshClsn_IsOnGround(c + 0x260) == 0)
            return;
        Camera_SetLookAt(cam, (struct V3 *)(c + 0x44c));
        Camera_SetPos(cam, (struct V3 *)(c + 0x458));
        *(int *)(cam + 0x154) &= ~8;
        *(int *)(c + 0xb0) &= ~0x4000000;
        *(int *)&NEXT_ACTOR_UPDATE_FLAGS &= ~0x4000000;
        *(u8 *)(c + 0x442) += 1;
        *(int *)(c + 0x238) &= ~1;
        *(char **)(c + 0x464) = 0;
        return;
    case 2:
    {
        u32 id = *(u32 *)(c + 0x244);
        if (id == 0)
            return;
        if (Actor_FindWithID(id) == 0)
            return;
        if ((*(int *)(c + 0x240) & 0x400000) == 0)
            return;
        func_ov089_02131df4(c);
        *(u8 *)(c + 0x442) += 1;
        return;
    }
    default:
        return;
    }
}
