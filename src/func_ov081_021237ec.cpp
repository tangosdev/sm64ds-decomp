//cpp
// func_ov081_021237ec at 0x021237ec
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov081).
typedef struct { int x, y, z; } Vector3;
struct Mtx43 { int m[12]; };

extern "C" {
void _ZN5Actor10PoofDustAtERK7Vector3(void* c, const Vector3& v);
int _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* c, Vector3* v, unsigned int n, int f, short s);
void Matrix4x3_FromTranslation(struct Mtx43* m, int x, int y, int z);
void MulMat4x3Mat4x3(struct Mtx43* dst, struct Mtx43* a, struct Mtx43* b);
void SubVec3(Vector3* a, Vector3* b, Vector3* c);
void Vec3_LslInPlace(Vector3* v, int n);
void AddVec3(Vector3* a, Vector3* b, Vector3* c);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
void func_0201267c(int a, void* b);
void _ZN5Actor24KillAndTrackInDeathTableEv(void* c);
extern struct Mtx43 data_020a0e68;
}

extern "C" void func_ov081_021237ec(char* c)
{
    Vector3 t;
    Vector3 v;
    Vector3 b;

    t.x = *(int*)(c + 0x5c);
    t.y = *(int*)(c + 0x60);
    t.z = *(int*)(c + 0x64);
    t.y += *(int*)(c + 0x1a4) - 0x50000;
    v = t;
    _ZN5Actor10PoofDustAtERK7Vector3(c, v);

    b.x = *(int*)(c + 0x5c);
    b.y = *(int*)(c + 0x60);
    b.z = *(int*)(c + 0x64);
    _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &b, 3, 0xa000, 0);

    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    MulMat4x3Mat4x3((struct Mtx43*)(*(char**)(c + 0x124) + 0x30), &data_020a0e68, &data_020a0e68);

    t.x = data_020a0e68.m[9];
    t.y = data_020a0e68.m[10];
    t.z = data_020a0e68.m[11];
    SubVec3(&t, (Vector3*)(c + 0x5c), &t);
    Vec3_LslInPlace(&t, 3);
    AddVec3(&t, (Vector3*)(c + 0x5c), &t);

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x53, t.x, t.y, t.z);
    func_0201267c(0xd5, c + 0x74);
    _ZN5Actor24KillAndTrackInDeathTableEv(c);
}