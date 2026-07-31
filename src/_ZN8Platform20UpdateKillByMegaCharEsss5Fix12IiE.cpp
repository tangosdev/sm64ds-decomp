//cpp
template<class T> struct Fix12 { T v; };
struct Vector3 { int x, y, z; };
struct Matrix4x3 { int m[12]; };
struct CylinderClsn;

struct Actor {
    void UpdatePos(CylinderClsn* c);
};

extern void Matrix4x3_FromRotationY(Matrix4x3* m, int angle);
extern void MulVec3Mat4x3(Vector3* v, Matrix4x3* m, Vector3* dst);
extern void Vec3_Add(Vector3* out, Vector3* a, Vector3* b);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov002_020ee5d0(unsigned char* self, int arg);

struct RaycastLine {
    RaycastLine();
    ~RaycastLine();
    void SetObjAndLine(Vector3 const& a, Vector3 const& b, Actor* c);
    int DetectClsn();
    char buf[0x78];
};

/* Only slot 31 (vtable offset 0x7c) is used; the preceding slots are placeholders. */
struct PlatformVT {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
};

extern Matrix4x3 data_020a0e68;

struct Platform {
    char _0[0x5c];
    Vector3 pos;         /* 0x5c */
    char _68[0x8c - 0x68];
    short rot[3];        /* 0x8c, 0x8e, 0x90 */
    char _92[0x94 - 0x92];
    short f_94;          /* 0x94 */
    char _96[0x31c - 0x96];
    unsigned char f_31c; /* 0x31c */
    unsigned char f_31d; /* 0x31d */

    int UpdateKillByMegaChar(short a, short b, short c, Fix12<int> d);
};

int Platform::UpdateKillByMegaChar(short a, short b, short c, Fix12<int> d)
{
    Vector3 vin;
    Vector3 vmid;
    Vector3 vout;

    if (this->f_31c == 0) return 0;

    vin.x = 0;
    vin.y = 0;
    vin.z = 0xc8000;
    vmid.x = 0;
    vmid.y = 0;
    vmid.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, this->f_94);
    MulVec3Mat4x3(&vin, &data_020a0e68, &vmid);
    Vec3_Add(&vout, &this->pos, &vmid);
    vmid = vout;

    RaycastLine ray;
    ray.SetObjAndLine(this->pos, vmid, (Actor*)this);
    if (ray.DetectClsn()) {
        *(short*)((int)(((long long)(int)((char*)this + 0x94)))) =
            (short)(*(short*)((int)(((long long)(int)((char*)this + 0x94)))) + 0x8000);
    }
    *(short*)((int)(((long long)(int)((char*)this + 0x8c)))) =
        (short)(*(short*)((int)(((long long)(int)((char*)this + 0x8c)))) + a);
    *(short*)((int)(((long long)(unsigned int)((char*)this + 0x8e)))) =
        (short)(*(short*)((int)(((long long)(unsigned int)((char*)this + 0x8e)))) + b);
    *(short*)((int)(((long long)(int)((char*)this + 0x90)))) =
        (short)(*(short*)((int)(((long long)(int)((char*)this + 0x90)))) + c);
    ((Actor*)this)->UpdatePos(0);
    if (DecIfAbove0_Byte(&this->f_31d) == 0) {
        ((PlatformVT*)this)->v31();
    }
    func_ov002_020ee5d0((unsigned char*)this, d.v);
    return 1;
}
