typedef struct { int x, y, z; } Vector3;
typedef int Fix12i;
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void MulVec3Mat4x3(const Vector3* in, void* m, Vector3* out);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int, unsigned int, Fix12i, Fix12i, Fix12i, const void*, void*);
extern void* _ZN8Particle6System12FromUniqueIDEj(unsigned int id);
extern int data_020a0e68[];
void func_ov073_0211f144(char* c) {

    Vector3 pos, in, out;
    int y;
    ((int*)&pos)[0] = *(int*)(c + 0x5c);
    y = *(int*)(c + 0x60);
    ((int*)&pos)[1] = y;
    ((int*)&pos)[2] = *(int*)(c + 0x64);
    ((int*)&pos)[1] = y + (int)(((long long)*(int*)(c + 0x80) * 0x1e000 + 0x800) >> 12);

    Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x3d8));
    in.z = 0; in.z = 0xc8000; in.x = 0; in.y = 0;
    out.x = 0; out.y = 0; out.z = 0;
    Matrix4x3_FromRotationY(data_020a0e68, *(short*)(c + 0x94));
    MulVec3Mat4x3(&in, data_020a0e68, &out);
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;
    *(unsigned int*)(c + 0x4f8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(unsigned int*)(c + 0x4f8), 0x77, pos.x, pos.y, pos.z, 0, 0);
    *(unsigned int*)(c + 0x4fc) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(unsigned int*)(c + 0x4fc), 0x78, pos.x, pos.y, pos.z, 0, 0);
    if (*(unsigned int*)(c + 0x4f8) != 0) {
        void* sys = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int*)(c + 0x4f8));
        if (sys != 0) *(int*)((char*)sys + 0x44) = *(int*)(c + 0x80) * 0x14;
    }
    if (*(unsigned int*)(c + 0x4fc) == 0) return;
    {
        void* sys = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int*)(c + 0x4fc));
        if (sys != 0) *(int*)((char*)sys + 0x44) = *(int*)(c + 0x80) * 0x14;
    }
}
