// NONMATCHING: halfword schedule (i/mat between ldrsh/strh) + scale/late regperm (div=52, size-exact 0x2f8). Logic verified vs ROM.
typedef short s16;
typedef struct { int x, y, z; } Vector3;
typedef struct { int m[12]; } Matrix4x3;
#define LA(p) ((int*)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))
extern void Vec3_Asr(Vector3* d, Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void* m, s16 ang);
extern void MulVec3Mat4x3(const void* v, const void* m, void* out);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void* m, s16 x, s16 y, s16 z);
extern void _Z14ApproachLinearRsss(s16* dst, s16 target, s16 step);
extern void MulMat4x3Mat4x3(void* m1, void* m0, void* mF);
extern void* _ZN5Actor10FindWithIDEj(unsigned id);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, s16 x, s16 y, s16 z);
extern Matrix4x3 data_020a0e68;

void func_ov016_02111c40(char* c)
{

    Vector3 in;
    Vector3 out;
    Vector3 asr;
    volatile int slot;
    int i;
    char* bone;
    int base;
    int* pos;
    Matrix4x3* mat_src;
    Matrix4x3* scratch;
    void* found;
    int off424;
    char* p400;
    unsigned id;
    int tx, ty, tz;
    int* px;
    int* py;
    int* pz;
    int v;

    Vec3_Asr(&asr, (Vector3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    in.z = 0;
    in.z = -0x190000;
    in.x = 0;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    Matrix4x3_FromTranslation(
        &data_020a0e68,
        (*(int*)(c + 0x5c) + out.x) >> 3,
        (*(int*)(c + 0x60)) >> 3,
        (*(int*)(c + 0x64) + out.z) >> 3);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(
        &data_020a0e68,
        *(s16*)(c + 0x8c),
        *(s16*)(c + 0x8e),
        *(s16*)(c + 0x90));
    *(Matrix4x3*)(c + 0x36c) = data_020a0e68;


    p400 = c + 0x400;
    off424 = 0x424;
    _Z14ApproachLinearRsss((s16*)(c + off424), *(s16*)(p400 + 0x26), 0x40);
    i = 0;
    {
        s16 t = *(s16*)(c + 0x424);
        scratch = &data_020a0e68;
        mat_src = (Matrix4x3*)(c + 0x36c);
        *(s16*)(c + 0x422) = t;
    }
    {
        int ox = 0x448;
        bone = c;
        base = i;
        pos = (int*)(c + ox);
    }


    do {
        Matrix4x3* walk;
        Matrix4x3* dst;
        Matrix4x3* dst_save;
        int ox, oy;

        slot = i;
        v = slot;
        *(int*)(bone + 0x448) = v;
        *(int*)(bone + 0x44c) = v;
        walk = mat_src;
        *(int*)(bone + 0x450) = v;
        dst = &data_020a0e68;
        dst_save = dst;
        *dst = *walk;
        MulMat4x3Mat4x3(
            (char*)*(void**)(c + 0x364) + base,
            dst_save,
            dst_save);
        {
            Matrix4x3 *sc = &data_020a0e68;
            unsigned t;
            ox = 0x448;
            tx = *(int*)((char*)sc + 0x24);
            pos[0] = tx;
            px = LA(bone + ox);
            ty = *(int*)((char*)sc + 0x28);
            oy = 0x44c;
            pos[1] = ty;
            py = LA(bone + oy);
            tz = *(int*)((char*)sc + 0x2c);
            pz = (int*)(bone + 0x450);
            pos[2] = tz;
            t = (unsigned)*px;
            i++;
            *px = (int)(t << 3);
            t = (unsigned)*py;
            bone += 0xc;
            *py = (int)(t << 3);
            t = (unsigned)*pz;
            base += 0x30;
            *pz = (int)(t << 3);
            pos += 3;
        }
    } while (i < 7);

    id = *(unsigned*)(c + 0x49c);
    if (id == 0) return;
    found = _ZN5Actor10FindWithIDEj(id);
    if (found == 0) return;
    *(int*)(c + 0x43c) = 0;
    *(int*)(c + 0x440) = 0;
    *(int*)(c + 0x444) = 0;
    MulMat4x3Mat4x3((char*)*(void**)(c + 0x364) + 0xc0, c + 0x36c, c + 0x3c0);
    Matrix4x3_FromTranslation(&data_020a0e68, 0x28000, 0, 0);

    Matrix4x3_ApplyInPlaceToRotationXYZExt(
        &data_020a0e68, 0x4000, -0x8000, *(s16*)(c + 0x416));

    MulMat4x3Mat4x3(&data_020a0e68, c + 0x3c0, c + 0x3c0);


    {
        Matrix4x3* src = (Matrix4x3*)(c + 0x3c0);
        void* live = src;
        int off43c = 0x43c;
        int off444 = 0x444;
        data_020a0e68 = *src;
        scratch = &data_020a0e68;
        px = LA(c + off43c);
        py = LA(c + 0x440);
        pz = LA(c + off444);
        *px = *(int*)((char*)scratch + 0x24);
        *py = *(int*)((char*)scratch + 0x28);
        *pz = *(int*)((char*)scratch + 0x2c);
        *px = *px << 3;
        *py = *py << 3;
        *pz = *pz << 3;
        *(void**)((char*)found + 0xc8) = live;
    }

}
