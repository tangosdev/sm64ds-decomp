//cpp
/* func_ov002_020f8c94 at 0x020f8c94 */
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;

struct Vector3 { int x, y, z; };
struct Vec3P { int x, y, z; Vec3P() {} };
enum Bool { FALSE, TRUE };

struct Obj {
    virtual int m00(); virtual int m01(); virtual int m02(); virtual int m03();
    virtual int m04(); virtual int m05(); virtual int m06(); virtual int m07();
    virtual int m08(); virtual int m09(); virtual int m10(); virtual int m11();
    virtual int m12(); virtual int m13(); virtual int m14(); virtual int m15();
    virtual int m16(); virtual int m17(); virtual int m18(); virtual int m19();
    virtual int m20(); virtual int m21(); virtual int m22(); virtual int m23();
    virtual int m24(); virtual int m25(); virtual int m26(); virtual int m27();
    virtual int m28(); virtual int m29();
};

extern "C" {
void func_ov002_020f8b24(void* self);
int func_ov002_020ad660(void* cc, void* pp, void* r5p, int flags);
void _ZN5Actor19DisappearPoofDustAtERK7Vector3(void* self, const struct Vector3* v);
void* _ZN5Actor13ClosestPlayerEv(void* self);
s16 Vec3_HorzAngle(const void* a, const void* b);
void* _ZN5Actor10FindWithIDEj(u32 id);
void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const void* pos);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, s32 x, s32 y, s32 z);
void _ZN12CylinderClsn5ClearEv(void* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
void _ZN6Player4BurnEv(void* self);
void func_02012694(u32 id, const void* v);
void _ZN9ActorBase18MarkForDestructionEv(void* self);
void func_020383fc(void* p);
int _Z14ApproachLinearRiii(void* p, s32 target, s32 step);
int _Z14ApproachLinearRsss(void* p, s16 target, s16 step);
int _ZNK12WithMeshClsn8IsOnWallEv(void* self);
int _ZNK12WithMeshClsn12TouchesWaterEv(void* self);
void func_ov002_020f897c(void* self);
void func_ov002_020f88ec(void* self);
extern s16 data_02082214[];
}

extern "C" int func_ov002_020f8c94(void* arg0) {
    char* c = (char*)arg0;
    struct Vector3 v1;
    volatile struct Vector3 pt;
    struct Vector3 v2, v3, w1, w2, w3;
    int f;
    int res;
    void* found;
    int lr;
    int id134;

    f = *(int*)(c + 0xb0);
    {
        enum Bool b = (enum Bool)((f & 0x20000) != 0);
        if (b != FALSE) {
            func_ov002_020f8b24(c);
            return 1;
        }
    }
    {
        enum Bool b = (enum Bool)((f & 0x40000) != 0);
        if (b != FALSE) {
            return 1;
        }
    }

    res = func_ov002_020ad660(c, c + 0x144, 0, 2);
    if (res != 0) {
        if (res == 2) {
            int x = *(int*)(c + 0x5c);
            v1.x = x;
            v1.y = *(int*)(c + 0x60);
            v1.z = *(int*)(c + 0x64);
            if (*(u8*)(c + 0x36e) != 0)
                v1.x = x * (u32)-1;
            v1.y += 0x50000;
            ((int*)&w1)[0] = ((int*)&v1)[0];
            ((int*)&w1)[1] = ((int*)&v1)[1];
            ((int*)&w1)[2] = ((int*)&v1)[2];
            _ZN5Actor19DisappearPoofDustAtERK7Vector3(c, &w1);
        } else {
            func_ov002_020f8b24(c);
        }
        return 1;
    }

    *(void**)(c + 0x358) = _ZN5Actor13ClosestPlayerEv(c);
    if (*(void**)(c + 0x358) != 0 && *(u8*)(c + 0x36d) != 3) {
        *(s16*)(c + 0x368) = Vec3_HorzAngle(c + 0x5c, (char*)(*(void**)(c + 0x358)) + 0x5c);
    } else {
        *(s16*)(c + 0x368) = *(s16*)(c + 0x8e);
    }

    id134 = *(int*)(c + 0x134);
    if (id134 != 0) {
        if ((*(int*)(c + 0x130) & 0x8000) == 0) {
            found = _ZN5Actor10FindWithIDEj((u32)id134);
            if (found != 0) {
                if (*(int*)(c + 0x130) & 0x10) {
                    *(u32*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x10000001;
                    *(s16*)(c + 0x94) = Vec3_HorzAngle((char*)found + 0x5c, c + 0x5c);
                    *(int*)(c + 0x98) = 0xa000;
                    *(int*)(c + 0xa8) = 0x28000;
                    *(s16*)(c + 0x102) = 0x1e;
                    *(s16*)(c + 0xec) = 0;
                    *(s16*)(c + 0xee) = 0;
                    *(s16*)(c + 0xf0) = 0;
                    *(int*)(c + 0x10c) = 8;
                    *(int*)(c + 0x9c) = -0x2000;
                    *(int*)(c + 0xa0) = -0x32000;
                    _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
                    pt.x = *(int*)(c + 0x5c);
                    pt.y = *(int*)(c + 0x60);
                    pt.z = *(int*)(c + 0x64);
                    {
                        int ret = ((Obj*)c)->m29();
                        int px, py, pz;
                        py = pt.y;
                        px = pt.x;
                        py = py + ret;
                        pz = pt.z;
                        pt.y = py;
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, px, py, pz);
                    }
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, pt.x, pt.y, pt.z);
                    func_ov002_020f8b24(c);
                    _ZN12CylinderClsn5ClearEv(c + 0x110);
                    return 1;
                }
                {
                    enum Bool isbf = (enum Bool)(*(u16*)((char*)found + 0xc) == 0xbf);
                    if (isbf != FALSE && *(u8*)((char*)found + 0x6f9) == 0 && *(u8*)((char*)found + 0x6fb) == 0) {
                        _ZN6Player4BurnEv(found);
                        {
                            int x = *(int*)(c + 0x5c);
                            v2.x = x;
                            v2.y = *(int*)(c + 0x60);
                            v2.z = *(int*)(c + 0x64);
                            if (*(u8*)(c + 0x36e) != 0)
                                v2.x = x * (u32)-1;
                            v2.y += 0x50000;
                            ((int*)&w2)[0] = ((int*)&v2)[0];
                            ((int*)&w2)[1] = ((int*)&v2)[1];
                            ((int*)&w2)[2] = ((int*)&v2)[2];
                            _ZN5Actor19DisappearPoofDustAtERK7Vector3(c, &w2);
                        }
                        if (*(u8*)(c + 0x36d) == 4)
                            func_02012694(0x157, c + 0x74);
                        _ZN9ActorBase18MarkForDestructionEv(c);
                    }
                }
            }
        } else {
            *(int*)(((int)c + 0x128) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
        }
    }

    func_020383fc(c + 0x144);
    if (_Z14ApproachLinearRiii(c + 0x98, *(int*)(c + 0x35c), 0x999) != 0) {
        _Z14ApproachLinearRsss(c + 0x94, *(s16*)(c + 0x368), 0x200);
    }

    lr = (*(int*)(c + 0x98) * data_02082214[(*(u16*)(c + 0x92) >> 4) * 2 + 1]) / 4096;
    *(int*)(c + 0xa4) = (lr * data_02082214[(*(u16*)(c + 0x94) >> 4) * 2]) / 4096;
    *(int*)(c + 0xa8) = (-*(int*)(c + 0x98) * data_02082214[(*(u16*)(c + 0x92) >> 4) * 2]) / 4096;
    *(int*)(c + 0xac) = (lr * data_02082214[(*(u16*)(c + 0x94) >> 4) * 2 + 1]) / 4096;
    *(int*)(((int)c + 0x5c) & 0xFFFFFFFFFFFFFFFFLL) += *(int*)(c + 0xa4);
    *(int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFLL) += *(int*)(c + 0xa8);
    *(int*)(((int)c + 0x64) & 0xFFFFFFFFFFFFFFFFLL) += *(int*)(c + 0xac);

    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    *(int*)(((int)c + 0x360) & 0xFFFFFFFFFFFFFFFFLL) += *(int*)(c + 0x98);

    if (*(int*)(c + 0x360) > *(int*)(c + 0x364)
        || _ZNK12WithMeshClsn8IsOnWallEv(c + 0x144) != 0
        || _ZNK12WithMeshClsn12TouchesWaterEv(c + 0x144) != 0) {
        u8 st = *(u8*)(c + 0x36d);
        if (st == 0) {
            func_ov002_020f897c(c);
        } else if (st == 4 && _ZNK12WithMeshClsn8IsOnWallEv(c + 0x144) == 0) {
            func_ov002_020f88ec(c);
        }
        {
            int x = *(int*)(c + 0x5c);
            v3.x = x;
            v3.y = *(int*)(c + 0x60);
            v3.z = *(int*)(c + 0x64);
            if (*(u8*)(c + 0x36e) != 0)
                v3.x = x * (u32)-1;
            v3.y += 0x50000;
            ((int*)&w3)[0] = ((int*)&v3)[0];
            ((int*)&w3)[1] = ((int*)&v3)[1];
            ((int*)&w3)[2] = ((int*)&v3)[2];
            _ZN5Actor19DisappearPoofDustAtERK7Vector3(c, &w3);
        }
        if (*(u8*)(c + 0x36d) == 4)
            func_02012694(0x157, c + 0x74);
        _ZN9ActorBase18MarkForDestructionEv(c);
    }

    *(u16*)(((int)c + 0x100) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    func_ov002_020f8b24(c);
    return 1;
}
