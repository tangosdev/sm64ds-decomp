//cpp
struct Mtx { int w[12]; };
extern "C" {
void Matrix4x3_FromQuaternion(const void* q, struct Mtx* mF);
void Matrix4x3_FromTranslation(struct Mtx* m, int x, int y, int z);
void MulMat4x3Mat4x3(struct Mtx* a, struct Mtx* b, struct Mtx* out);
void _ZN18TextureTransformer6UpdateER15ModelComponents(void* t, void* mc);
void _ZN9ModelBase12ApplyOpacityEj(void* mb, unsigned int op, int z);
}
extern struct Mtx data_020a0e68;

struct VtO { int dummy[0x14/4]; void (*f)(void*, void*); };

extern "C" void func_ov006_020ce988(char* c){
    struct Mtx tmp;
    Matrix4x3_FromQuaternion(c+0x74, &tmp);
    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c+8), *(int*)(c+0xc), *(int*)(c+0x10));
    MulMat4x3Mat4x3(&tmp, &data_020a0e68, &data_020a0e68);
    *(struct Mtx*)(*(char**)(c+0x190) + 0x1c) = data_020a0e68;
    _ZN18TextureTransformer6UpdateER15ModelComponents(c+0x194, *(char**)(c+0x190) + 8);
    _ZN9ModelBase12ApplyOpacityEj(*(void**)(c+0x190), *(unsigned char*)(c+0x9c), 0);
    {
        void* o = *(void**)(c+0x190);
        VtO* vt = *(VtO**)o;
        vt->f(o, c+0x68);
    }
}
