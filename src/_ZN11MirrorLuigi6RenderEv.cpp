//cpp
// @symbol _ZN11MirrorLuigi6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MirrorLuigi.h"
struct Mtx { int m[12]; };
struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(int);
};

extern "C" {

extern unsigned char data_0209f250;
extern char* data_0209f394[];
extern char data_020a0e68;

char* func_ov002_020e496c(char* c);
void func_0203c178(void* m, int a, int b, int c);
void MulMat3x3Mat3x3(void* d, void* a, void* b);
void _ZN5Model6RenderEPK7Vector3(void* m, void* v);
void _ZN15TextureSequence6UpdateER15ModelComponents(void* ts, void* mc);

}

int MirrorLuigi::Render()
{
    char* player;
    char* r8res;
    char* comp;
    char* q;
    char* p_f0;
    unsigned int i;
    Mtx* dst;
    Mtx* src;

    if (data_ov055_02111b6c == 0) return 1;

    player = data_0209f394[data_0209f250];
    r8res = func_ov002_020e496c(player);

    /* Every offset the cartridge's own ~MirrorLuigi proves is inside one of the two
       model sub-objects (tools/dtor_members.py): 0x0dc/0x0e8 are the ModelAnim at
       0x0d4 (+0x8 data, +0x14 data.transforms), and 0x140/0x14c/0x154 are the Model
       at 0x138 (+0x8 data, +0x14 data.transforms, +0x1c mat4x3). */
    q = (char*)(int)((char*)&mModelAnim.data);
    comp = *(char**)q;
    dst = *(Mtx**)(q + 0xc);
    src = *(Mtx**)(r8res + 0x14);
    for (i = 0; i < *(unsigned int*)(comp + 4); i++) {
        *(Mtx*)(int)((long long)(int)dst) = *src;
        src++;
        dst++;
    }

    p_f0 = (char *)&mModelAnim.mat4x3;
    *(Mtx*)p_f0 = *(Mtx*)(r8res + 0x1c);
    *(int*)(p_f0 + 0x24) = -*(int*)(p_f0 + 0x24);
    func_0203c178(&data_020a0e68, -0x1000, 0x1000, 0x1000);
    MulMat3x3Mat3x3(p_f0, &data_020a0e68, p_f0);
    *(Mtx*)((char*)&mModel.mat4x3) = *(Mtx*)p_f0;

    if (data_ov055_02111b64 & 0x20000) return 1;

    _ZN5Model6RenderEPK7Vector3((void*)((char*)&mModelAnim), 0);
    *(Mtx*)(*(char**)((char*)&mModel.data.transforms)) =
        *(Mtx*)(*(char**)((char*)&mModelAnim.data.transforms) + 0x2d0);
    _ZN15TextureSequence6UpdateER15ModelComponents(
        &mTextureSequences[0], &mModelAnim.data);
    mTextureSequences[0].currFrame =
        (int)(*(unsigned char*)(player + 0x6fb)) << 12;
    _ZN15TextureSequence6UpdateER15ModelComponents(
        &mTextureSequences[1], &mModel.data);
    mTextureSequences[1].currFrame =
        (int)(*(unsigned char*)(player + 0x6fb)) << 12;
    ((Sub*)((char*)&mModel))->m5(0);
    return 1;
}
