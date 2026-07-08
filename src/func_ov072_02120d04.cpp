//cpp
struct Vector3 { int x, y, z; };
extern "C" {
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void Vec3_Asr(Vector3* d, Vector3* s, int sh);
extern int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void* m, void* v, Vector3* a, int b, Vector3* e);
extern int GLOBAL_CLIPPER[];
extern int VIEW_MATRIX_ASR_3[];
}

extern "C" void func_ov072_02120d04(char* c)
{
    Vector3 out;
    Vector3 tmp;
    if (*(int*)(c + 0x368) == 3) return;
    if ((unsigned)*(int*)(c + 0x60) > (unsigned)(-0x1800000)) {
        if (Vec3_Dist((Vector3*)(c + 0x350), (Vector3*)(c + 0x5c)) < 0x190000) return;
        if (DecIfAbove0_Short((unsigned short*)(c + 0x36c)) != 0) return;
    }
    Vec3_Asr(&tmp, (Vector3*)(c + 0x350), 3);
    if (_ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_((void*)GLOBAL_CLIPPER, (void*)VIEW_MATRIX_ASR_3, &tmp, 0x1f400, &out) <= 0xc350000) return;
    *(int*)(c + 0x5c) = *(int*)(c + 0x350);
    *(int*)(c + 0x60) = *(int*)(c + 0x354);
    *(int*)(c + 0x64) = *(int*)(c + 0x358);
}
