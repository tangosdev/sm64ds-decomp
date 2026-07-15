struct Vec3 { int x, y, z; };
struct Mtx43 { int m[12]; };
extern char *data_0209f318;
extern struct Mtx43 data_020a0e68;
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(struct Vec3 *v, struct Mtx43 *m, struct Vec3 *out);
extern void Vec3_Add(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern int func_ov084_0212cda0(int arg0, struct Vec3 *out, struct Vec3 *target);
extern void _ZN6Camera9SetLookAtERK7Vector3(void *cam, struct Vec3 *v);
extern void _ZN6Camera6SetPosERK7Vector3(void *cam, struct Vec3 *v);

int func_ov084_0212cae0(char *self)
{
    int ok2, ok1;
    char *cam;
    char *actor;
    struct Vec3 vF, vG, vA, vB, vC, vD, vE;
    unsigned id = *(unsigned int *)(self + 0x198);
    cam = data_0209f318;
    if (id != 0) {
        actor = _ZN5Actor10FindWithIDEj(id);
        if (actor != 0) goto body;
    }
    goto end;
body:
    ok1 = 0;
    vA.x = ok1; vA.y = 0x80000; vA.z = -0xa000;
    vB.x = ok1; vB.y = 0x200000; vB.z = 0xc8000;
    vC.x = ok1; vC.y = ok1; vC.z = ok1;
    Matrix4x3_FromRotationY(&data_020a0e68, *(short *)(actor + 0x8e));
    MulVec3Mat4x3(&vA, &data_020a0e68, &vC);
    Vec3_Add(&vD, (struct Vec3 *)(actor + 0x5c), &vC);
    {
        int dx = vD.x; int dy = vD.y; int z = ok1;
        *(volatile int *)&vC.x = z; *(volatile int *)&vA.x = dx;
        *(volatile int *)&vC.y = z; *(volatile int *)&vA.y = dy;
        int dz = *(volatile int *)&vD.z;
        *(volatile int *)&vC.z = z; *(volatile int *)&vA.z = dz;
    }
    MulVec3Mat4x3(&vB, &data_020a0e68, &vC);
    Vec3_Add(&vE, (struct Vec3 *)(actor + 0x5c), &vC);
    {
        int ey = vE.y; int ex = vE.x;
        vB.y = ey;
        int ez = vE.z;
        vB.x = ez ? ex : ex;
        vB.z = ez;
    }
    {
        int *pF = (int *)(unsigned)((unsigned long long)(unsigned)(cam + 0x80));
        vF.x = pF[0];
        {
            int *pG = (int *)(unsigned)((unsigned long long)(unsigned)(cam + 0x8c));
            vF.y = pF[1]; vF.z = pF[2];
            vG.x = pG[0]; vG.y = pG[1]; vG.z = pG[2];
        }
    }
    ok1 = func_ov084_0212cda0((int)self, &vF, &vA);
    ok2 = func_ov084_0212cda0((int)self, &vG, &vB);
    if ((*(short *)(self + 0x8c) & 0xff) == 2) {
        ok1 = 1; ok2 = 1;
        _ZN6Camera9SetLookAtERK7Vector3(cam, &vA);
        _ZN6Camera6SetPosERK7Vector3(cam, &vB);
    } else {
        _ZN6Camera9SetLookAtERK7Vector3(cam, &vF);
        _ZN6Camera6SetPosERK7Vector3(cam, &vG);
    }
end:
    return (ok1 != 0 && ok2 != 0) ? 1 : 0;
}
