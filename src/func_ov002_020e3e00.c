//cpp
//

extern "C" {
    extern signed char data_0209f2f8;
    extern signed char data_02092120;
    extern int* func_02111b6c;
    extern int func_02111b64;
    extern int data_020a0e68[12];

    void func_0203c178(int *dst, int a, int b, int c);
    void MulMat3x3Mat3x3(int *dst, const int *a, const int *b);
    void func_02016acc(void *obj, unsigned int mask);
    void func_02016b24(void *obj, unsigned int mask);
}

struct Vector3 { int x, y, z; };
struct Model { };

extern "C" void _ZN5Model6RenderEPK7Vector3(Model *self, const Vector3 *v);
extern "C" void _ZN9ModelBase12ApplyOpacityEj(Model *self, unsigned int a, int b);

typedef struct { int m[12]; } Mat;

extern "C" void func_ov002_020e3e00(Model *obj, Vector3 *vec, unsigned int opacity) {
    int ip = (data_0209f2f8 == 0x2f) ? 1 : 0;

    if (!ip) {
        if (data_0209f2f8 != 5) return;
        if (data_02092120 != 3) return;
    }

    if (ip) {
        int ok;
        if (func_02111b6c == 0 || (func_02111b64 & 0x20000) != 0)
            ok = 1;
        else
            ok = 0;
        if (!ok) return;
    }

    Mat tmp;
    Mat *src = (Mat*)((char*)obj + 0x1c);
    tmp = *src;

    int r0;
    if (ip)
        r0 = 0;
    else
        r0 = 0x1086000;
    int t = r0 >> 3;
    t <<= 1;
    int diff = t - ((int*)src)[9];
    ((int*)src)[9] = diff;

    func_0203c178(data_020a0e68, -0x1000, 0x1000, 0x1000);
    MulMat3x3Mat3x3((int*)src, data_020a0e68, (int*)src);

    func_02016acc(obj, 0x80);
    func_02016b24(obj, 0x40);
    _ZN5Model6RenderEPK7Vector3(obj, vec);
    _ZN9ModelBase12ApplyOpacityEj(obj, opacity, 0);
    func_02016b24(obj, 0x80);
    func_02016acc(obj, 0x40);

    *src = tmp;
}
