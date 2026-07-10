// NONMATCHING: large jump-table state machine (size 0x444 vs 0x46c); structural first pass from disasm
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct V3 { int x, y, z; };

extern char *data_0209f318;
extern int data_0209b454;
extern int data_0209b490;
extern char data_020a0e68;
extern void _ZN6Camera9SetFlag_3Ev(char *cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(char *cam, struct V3 *v);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 a, int b);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 a, void *v);
extern int Vec3_HorzDist(const void *a, const void *b);
extern short Vec3_HorzAngle(const void *a, const void *b);
extern void _ZN7Message7EndTalkEv(void);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *a, void *b, int c);
extern void Vec3_ApproachHorz(void *a, void *b, int c);
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern void func_ov089_02131dcc(char *c, char *p);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, int ang);
extern void MulMat4x3Mat4x3(void *d, void *a, void *b);
extern void SubVec3(void *d, void *a, void *b);
extern void Vec3_LslInPlace(void *v, int sh);
extern void AddVec3(void *d, void *a, void *b);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int x, int y, int z, const void *v, void *cb);

void func_ov089_021311c0(char *c)
{
    char *cam = data_0209f318;
    u8 st = *(u8 *)(c + 0x442);
    struct V3 v;
    struct V3 zero;
    int dist;

    if (st > 3)
        goto common_tail;

    switch (st) {
    case 0: {
        int *b0 = (int *)(c + 0xb0);
        int *s1 = (int *)(cam + 0x80);
        int *s2 = (int *)(cam + 0x8c);
        int *dst434 = (int *)(c + 0x434);
        *b0 |= 0x4000000;
        data_0209b454 |= 0x4000000;
        dst434[0] = s1[0];
        dst434[1] = s1[1];
        dst434[2] = s1[2];
        *(int *)(c + 0x44c) = *(int *)(c + 0x434);
        *(int *)(c + 0x450) = *(int *)(c + 0x438);
        *(int *)(c + 0x454) = *(int *)(c + 0x43c);
        *(int *)(c + 0x458) = s2[0];
        *(int *)(c + 0x45c) = s2[1];
        *(int *)(c + 0x460) = s2[2];
        /* ROM copies from cam+0x80 into 0x434 then from c+0x434 into 0x44c — actually from s1 into 434 and from c+0x434 into 44c */
        _ZN6Camera9SetFlag_3Ev(cam);
        *(u8 *)(c + 0x442) = *(u8 *)(c + 0x442) + 1;
        {
            int vol = data_0209b490;
            /* signed / 15 magic 0x88888889 asr 3 */
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0, vol / 15);
        }
        break;
    }
    case 1: {
        u16 *timer = (u16 *)(c + 0x100);
        *timer = *timer + 1;
        if (*timer < 0x46)
            break;
        *(u8 *)(c + 0x442) = *(u8 *)(c + 0x442) + 1;
        *timer = 0;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x73, c + 0x74);
        zero.x = zero.y = zero.z = 0;
        *(int *)(c + 0xa8) = 0x50000;
        dist = Vec3_HorzDist(c + 0x5c, &zero);
        *(int *)(c + 0x98) = dist / 80; /* 0x66666667 magic ~ /80? asr 5 from smull */
        *(short *)(c + 0x94) = Vec3_HorzAngle(c + 0x5c, &zero);
        *(u8 *)(c + 0x443) = 0;
        *(int *)(c + 0x9c) = -0x2000;
        _ZN7Message7EndTalkEv();
        break;
    }
    case 2: {
        v.x = *(int *)(c + 0x5c);
        v.y = *(int *)(c + 0x60);
        v.z = *(int *)(c + 0x64);
        v.y += 0x64000;
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(c + 0x428, &v, 0x28000);
        _ZN6Camera9SetLookAtERK7Vector3(cam, (struct V3 *)(c + 0x44c));
        zero.x = zero.y = zero.z = 0;
        Vec3_ApproachHorz(c + 0x5c, &zero, *(int *)(c + 0x98));
        if (*(u8 *)(c + 0x443) == 0) {
            if (*(int *)(c + 0xa8) < 0) {
                if (*(int *)(c + 0x60) < *(int *)(c + 0x420)) {
                    *(int *)(c + 0x60) = *(int *)(c + 0x420);
                    *(int *)(c + 0xa8) = 0x41000;
                    *(int *)(c + 0x98) = 0;
                    *(u8 *)(c + 0x443) = *(u8 *)(c + 0x443) + 1;
                    _ZN5Sound9PlayBank3EjRK7Vector3(0x74, c + 0x74);
                }
            }
        } else {
            if (*(int *)(c + 0xa8) < 0) {
                int lim = *(int *)(c + 0x420) + 0xc8000;
                if (*(int *)(c + 0x60) < lim) {
                    *(int *)(c + 0x60) = lim;
                    *(int *)(c + 0xa8) = 0;
                    *(int *)(c + 0x9c) = 0;
                    *(u8 *)(c + 0x442) = *(u8 *)(c + 0x442) + 1;
                    *(u16 *)(c + 0x100) = 0;
                    *(int *)(c + 0x238) &= ~1;
                    {
                        int *cf = (int *)(cam + 0x154);
                        *cf &= ~8;
                    }
                    {
                        int *b0 = (int *)(c + 0xb0);
                        *b0 &= ~0x4000000;
                    }
                    data_0209b454 &= ~0x4000000;
                }
            }
        }
        break;
    }
    case 3: {
        u32 id = *(u32 *)(c + 0x244);
        char *found;
        if (id == 0)
            break;
        found = _ZN5Actor10FindWithIDEj(id);
        if (found == 0)
            break;
        if ((*(int *)(c + 0x240) & 0x400000) == 0)
            break;
        func_ov089_02131dcc(c, found);
        return;
    }
    }

common_tail:
    Matrix4x3_FromTranslation(&data_020a0e68, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short *)(c + 0x94));
    MulMat4x3Mat4x3(*(void **)(c + 0x128), &data_020a0e68, &data_020a0e68);
    {
        char *m = &data_020a0e68;
        v.x = *(int *)(m + 0x24);
        v.y = *(int *)(m + 0x28);
        v.z = *(int *)(m + 0x2c);
    }
    SubVec3(&v, c + 0x5c, &v);
    Vec3_LslInPlace(&v, 3);
    AddVec3(&v, c + 0x5c, &v);
    {
        int y = v.y;
        y = *(int *)(c + 0x80) * 0xd + y;
        v.y = y;
    }
    *(void **)(c + 0x464) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32 *)(c + 0x464), 0xb4, v.x, v.y, v.z, 0, 0);
    *(void **)(c + 0x468) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32 *)(c + 0x468), 0xb5, v.x, v.y, v.z, 0, 0);
    *(void **)(c + 0x46c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32 *)(c + 0x46c), 0xb6, v.x, v.y, v.z, 0, 0);
}
