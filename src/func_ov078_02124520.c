typedef signed short s16;
typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct Matrix4x3 { int m[12]; } Matrix4x3;

extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void func_ov078_02125c24(void *c, int a);
extern void func_02012694(int a, void *v);
extern void MulMat4x3Mat4x3(void *a, void *b, void *dst);
extern void Vec3_Lsl(Vector3 *d, Vector3 *s, int sh);
extern void _ZN8dActor_c17HugeLandingDustAtER7Vector3b(void *actor, Vector3 *v, int b);
extern int _ZN6Player12GetTalkStateEv(void *player);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *out);
extern void _ZN6Camera9SetLookAtERK7Vector3(void *cam, Vector3 *v);
extern void _ZN6Camera6SetPosERK7Vector3(void *cam, Vector3 *v);
extern void _Z14ApproachLinearRsss(s16 *cur, s16 tgt, s16 step);
extern int _ZN9Animation8FinishedEv(void *anim);
extern void KingBobOmb_SetState(void *c, void *p);

extern void *data_0209f318;
extern Matrix4x3 data_020a0e68;
extern void *data_ov078_0212703c;

int func_ov078_02124520(char *c)
{
    Vector3 a;
    Vector3 look;
    Vector3 pos;
    Vector3 in;
    Vector3 out;
    Vector3 ppos;
    Vector3 tmp;
    Vector3 dust;
    void *cam;
    char *player;
    int *pp;
    unsigned int *flags;
    s16 ang;

    cam = data_0209f318;
    if (_ZNK9Animation12WillHitFrameEi(c + 0x31c, 0x46)) {
        func_ov078_02125c24(c, 0x7d0000);
        func_02012694(0x12c, c + 0x74);
        a.x = 0;
        a.y = 0;
        a.z = 0;
        data_020a0e68 = *(Matrix4x3 *)(c + 0x2e8);
        MulMat4x3Mat4x3((char *)*(void **)(c + 0x2e0) + 0x120, &data_020a0e68, &data_020a0e68);
        a.x = data_020a0e68.m[9];
        a.y = data_020a0e68.m[10];
        a.z = data_020a0e68.m[11];
        Vec3_Lsl(&tmp, &a, 3);
        a.x = tmp.x;
        dust.x = tmp.x;
        a.y = tmp.y;
        dust.y = tmp.y;
        a.z = tmp.z;
        dust.z = tmp.z;
        _ZN8dActor_c17HugeLandingDustAtER7Vector3b(c, &dust, 1);
    }
    player = *(char **)(c + 0x430);
    if (_ZN6Player12GetTalkStateEv(player) != -1) {
        pp = (int *)(((int)player + 0x5c));
        in.x = 0;
        in.y = 0;
        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        ppos.x = pp[0];
        ppos.y = pp[1];
        ppos.z = pp[2];
        ang = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &ppos);
        look.x = *(int *)(c + 0x5c);
        look.y = *(int *)(c + 0x60);
        look.z = *(int *)(c + 0x64);
        pos.x = *(int *)(c + 0x5c);
        pos.y = *(int *)(c + 0x60);
        pos.z = *(int *)(c + 0x64);
        look.y += 0x100000;
        in.z = 0x800000;
        Matrix4x3_FromRotationY(&data_020a0e68, ang);
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        pos.y += 0x100000;
        pos.x += out.x;
        pos.z += out.z;
        _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
        _ZN6Camera6SetPosERK7Vector3(cam, &pos);
        _Z14ApproachLinearRsss((s16 *)(c + 0x8e), ang, 0x800);
        *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
        return 1;
    }
    *(unsigned int *)(((int)cam + 0x154)) &= ~8;
    if (_ZN9Animation8FinishedEv(c + 0x31c)) {
        KingBobOmb_SetState(c, &data_ov078_0212703c);
    }
    return 1;
}
