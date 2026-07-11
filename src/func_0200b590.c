typedef struct { int x, y, z; } Vector3;

extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern void func_0200897c(void *self, void *arg);
extern int func_020092c4(int arg0, Vector3 *out, Vector3 *target);

extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, const Vector3 *pos, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void _ZN13RaycastGroundD1Ev(void *self);

extern Vector3 data_02086e84;

void func_0200b590(char *c, short *a1, int arg2)
{
    Vector3 v0;
    Vector3 rotated;
    short angle1;
    short angle2;
    int diff;
    short newAngle;
    Vector3 pos;
    Vector3 base;
    char rg[0x50];
    int dx, dz;
    int hitY;
    int dist;
    int rem;
    int selfZ;
    int selfX;

    {
        int tz = a1[3] << 12;
        int ty = a1[2] << 12;
        int tx = a1[1] << 12;
        v0.x = tx;
        v0.y = ty;
        v0.z = tz;
    }

    angle1 = Vec3_HorzAngle((Vector3 *)(c + 0x98), &v0);
    angle2 = Vec3_HorzAngle((Vector3 *)(c + 0x8c), &v0);
    diff = (short)(angle2 - angle1);
    if (diff < -0x4000)
        diff = -0x4000;
    else if (diff > 0x4000)
        diff = 0x4000;
    newAngle = (short)(angle1 + diff);

    base.x = data_02086e84.x;
    base.y = data_02086e84.y;
    base.z = data_02086e84.z;
    Vec3_RotateYAndTranslate((int *)&rotated, (int *)&v0, newAngle, (int *)&base);
    rotated.y = arg2;

    selfZ = *(int *)(c + 0x88);
    dz = rotated.z - selfZ;
    selfX = *(int *)(c + 0x80);
    dx = rotated.x - selfX;
    pos.y = arg2 + 0x100000;

    pos.x = selfX + (int)(((long long)dx * 0xb33 + 0x800) >> 12);
    pos.z = selfZ + (int)(((long long)dz * 0xb33 + 0x800) >> 12);

    _ZN13RaycastGroundC1Ev(rg);
    func_0200897c(c, rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(rg)) {
        hitY = *(int *)(rg + 0x44);
        rotated.y = hitY;
        if (hitY < arg2)
            rotated.y = arg2;
    }
    rotated.y = rotated.y + 0x7d000;
    dist = Vec3_HorzDist((Vector3 *)(c + 0x8c), (Vector3 *)(c + 0x98));
    rem = 0x100000 - dist;
    if (rem > 0)
        rotated.y += rem;
    if (*(int *)(c + 0x154) & 4) {
        *(int *)(c + 0x8c) = rotated.x;
        *(int *)(c + 0x90) = rotated.y;
        *(int *)(c + 0x94) = rotated.z;
    } else {
        func_020092c4((int)c, (Vector3 *)(c + 0x8c), &rotated);
    }

    *(short *)(c + 0x17c) = Vec3_HorzAngle((Vector3 *)(c + 0x80), (Vector3 *)(c + 0x8c));
    _ZN13RaycastGroundD1Ev(rg);
}
