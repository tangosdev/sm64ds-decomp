//cpp
// NONMATCHING: base materialization / addressing (div=38). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
template<class T> struct Fix12 { T v; };
struct Vector3 { int x, y, z; };
struct Vec3 { int x, y, z; };
struct Matrix4x3 { int m[12]; };
struct Actor;
struct CylinderClsn;
struct ShadowModel;

extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(void* out, void* m, void* in);
extern void Vec3_Add(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov002_020ee5d0(unsigned char *self, int arg);

struct RaycastLine {
    RaycastLine();
    ~RaycastLine();
    void SetObjAndLine(Vector3 const & a, Vector3 const & b, Actor * c);
    int DetectClsn();
    char buf[0x78];
};

extern Matrix4x3 MATRIX_SCRATCH_PAPER;

struct Platform {
    char _0[0x5c];
    int pos[3];        /* 0x5c */
    char _68[0x8c-0x68];
    short rot[3];      /* 0x8c, 0x8e, 0x90 */
    char _92[0x94-0x92];
    short f_94;        /* 0x94 */
    char _96[0x31c-0x96];
    unsigned char f_31c; /* 0x31c */
    unsigned char f_31d; /* 0x31d */
    void UpdatePos(CylinderClsn* c);
    int UpdateKillByMegaChar(short a, short b, short c, Fix12<int> d);
};

int Platform::UpdateKillByMegaChar(short a, short b, short c, Fix12<int> d)
{
    int vin[3];
    int vmid[3];
    int vout[3];

    if (this->f_31c == 0) return 0;

    vin[0] = 0;
    vin[1] = 0;
    vin[2] = 0xc8000;
    vmid[0] = 0;
    vmid[1] = 0;
    vmid[2] = 0;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, this->f_94);
    MulVec3Mat4x3(vin, &MATRIX_SCRATCH_PAPER, vmid);
    Vec3_Add((Vec3*)vout, (Vec3*)this->pos, (Vec3*)vmid);
    vmid[0] = vout[0];
    vmid[1] = vout[1];
    vmid[2] = vout[2];
    RaycastLine ray;
    ray.SetObjAndLine(*(Vector3*)this->pos, *(Vector3*)vmid, (Actor*)this);
    if (ray.DetectClsn()) {
        this->f_94 = this->f_94 + 0x8000;
    }
    this->rot[0] = this->rot[0] + a;
    this->rot[1] = this->rot[1] + b;
    this->rot[2] = this->rot[2] + c;
    this->UpdatePos(0);
    if (DecIfAbove0_Byte(&this->f_31d) == 0) {
        (*(void(**)(Platform*))(*(int**)this + 0x7c/4))(this);
        func_ov002_020ee5d0((unsigned char*)this, d.v);
    }
    return 1;
}
