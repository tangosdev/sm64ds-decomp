//cpp
// NONMATCHING: different op / idiom (div=18). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vector3 { int x, y, z; };

struct Camera {
    char pad[0x80];
    Vector3 a;      /* 0x80 */
    Vector3 b;      /* 0x8c */
    void SetLookAt(const Vector3& v);
    void SetPos(const Vector3& v);
};

extern "C" int func_ov084_0212cda0(int arg0, Vector3* out, Vector3* target);

extern Camera* CAMERA;

struct Obj {
    char pad[0x8c];
    short f8c;       /* 0x8c */
    char pad2[0x1cc - 0x8e];
    Vector3 v1cc;    /* 0x1cc */
    Vector3 v1d8;    /* 0x1d8 */
};

extern "C" int func_ov084_0212ccb4(Obj* self)
{
    Camera* cam = CAMERA;
    Vector3 l1;
    Vector3 l2;
    Vector3* sa = &cam->a;
    Vector3* sb = &cam->b;
    l1.x = sa->x;
    l1.y = sa->y;
    l1.z = sa->z;
    l2.x = sb->x;
    l2.y = sb->y;
    l2.z = sb->z;
    int r1 = func_ov084_0212cda0((int)self, &l1, &self->v1cc);
    int r2 = func_ov084_0212cda0((int)self, &l2, &self->v1d8);
    if ((self->f8c & 0xff) == 2) {
        r1 = 1;
        r2 = 1;
        cam->SetLookAt(self->v1cc);
        cam->SetPos(self->v1d8);
    } else {
        cam->SetLookAt(l1);
        cam->SetPos(l2);
    }
    if (r1 != 0 && r2 != 0) return 1;
    return 0;
}
