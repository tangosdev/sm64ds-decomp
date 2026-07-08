// NONMATCHING: different op / idiom (div=39). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vec3 { int x, y, z; };
struct Mtx43 { int m[12]; };

extern char *CAMERA;
extern struct Mtx43 MATRIX_SCRATCH_PAPER;

extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(struct Vec3 *v, struct Mtx43 *m, struct Vec3 *out);
extern void Vec3_Add(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern int func_ov084_0212cda0(int arg0, struct Vec3 *out, struct Vec3 *target);
extern void _ZN6Camera9SetLookAtERK7Vector3(void *cam, struct Vec3 *v);
extern void _ZN6Camera6SetPosERK7Vector3(void *cam, struct Vec3 *v);

int func_ov084_0212cae0(char *self)
{
    char *cam;
    char *actor;
    struct Vec3 vF;
    struct Vec3 vG;
    struct Vec3 vA;
    struct Vec3 vB;
    struct Vec3 vC;
    struct Vec3 vD;
    struct Vec3 vE;
    int ok1;
    int ok2;

    cam = CAMERA;
    if (*(unsigned int *)(self + 0x198) != 0)
    {
        actor = _ZN5Actor10FindWithIDEj(*(unsigned int *)(self + 0x198));
        if (actor != 0)
            goto body;
    }
    goto end;

body:
    ok1 = 0;
    vA.x = ok1;
    vA.y = 0x80000;
    vA.z = -0xa000;
    vB.x = ok1;
    vB.y = 0x200000;
    vB.z = 0xc8000;
    vC.x = ok1;
    vC.y = ok1;
    vC.z = ok1;

    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(short *)(actor + 0x8e));
    MulVec3Mat4x3(&vA, &MATRIX_SCRATCH_PAPER, &vC);
    Vec3_Add(&vD, (struct Vec3 *)(actor + 0x5c), &vC);
    vC.x = ok1;
    vA.x = vD.x;
    vC.y = ok1;
    vA.y = vD.y;
    vC.z = ok1;
    vA.z = vD.z;

    MulVec3Mat4x3(&vB, &MATRIX_SCRATCH_PAPER, &vC);
    Vec3_Add(&vE, (struct Vec3 *)(actor + 0x5c), &vC);
    vB.y = vE.y;
    vB.x = vE.x;
    vB.z = vE.z;

    {
        struct Vec3 *pF = (struct Vec3 *)(cam + 0x80);
        vF.x = pF->x;
        {
            struct Vec3 *pG = (struct Vec3 *)(cam + 0x8c);
            vF.y = pF->y;
            vF.z = pF->z;
            vG.x = pG->x;
            vG.y = pG->y;
            vG.z = pG->z;
        }
    }

    ok1 = func_ov084_0212cda0((int)self, &vF, &vA);
    ok2 = func_ov084_0212cda0((int)self, &vG, &vB);

    if ((*(short *)(self + 0x8c) & 0xff) == 2)
    {
        ok1 = 1;
        ok2 = 1;
        _ZN6Camera9SetLookAtERK7Vector3(cam, &vA);
        _ZN6Camera6SetPosERK7Vector3(cam, &vB);
    }
    else
    {
        _ZN6Camera9SetLookAtERK7Vector3(cam, &vF);
        _ZN6Camera6SetPosERK7Vector3(cam, &vG);
    }

end:
    return (ok1 != 0 && ok2 != 0) ? 1 : 0;
}
