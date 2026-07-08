//cpp
struct Matrix4x3 { int m[12]; };
struct Vector3;
namespace G3i {
    void PerspectiveW_(int a1, int a2, int a3, int a4, int a5, int a6, bool b, Matrix4x3 *m);
    void LookAt_(const Vector3 *eye, const Vector3 *at, const Vector3 *up, bool b, Matrix4x3 *m);
}
extern "C" void _Z13CopyToViewMatPK9Matrix4x3(const Matrix4x3 *m);
namespace Clipper { void Func_020156DC(void *a, int b, int c, int d, int e); }

extern "C" short data_02082614[];
extern "C" int data_ov075_0211c660;
extern "C" int GLOBAL_CLIPPER;

extern "C" void func_ov075_021152d4(char *self)
{
    Matrix4x3 view;
    G3i::PerspectiveW_(data_02082614[0xa], data_02082614[0xb], 0x1555, 0x1000,
                       0x1388000, 0x1000, true, (Matrix4x3*)0);
    G3i::LookAt_((Vector3*)(self + 0xf28), (Vector3*)&data_ov075_0211c660,
                 (Vector3*)(self + 0xf34), true, &view);
    _Z13CopyToViewMatPK9Matrix4x3(&view);
    Clipper::Func_020156DC(&GLOBAL_CLIPPER, 0x1555, 0x105b, 0x1000, 0x1388000);
}
