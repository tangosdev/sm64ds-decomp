//cpp
// @symbol _ZN12dScMg3DEsp_c13InitResourcesEv
#include "decl_common.h"
#include "dMg3DEspModel_c.h"
#include "dScMg3DEsp_c.h"
#include "TextureTransformer.h"
/* dScMg3DEsp_c::InitResources, vtable slot 0 (tools/rtti_vtables.py --own
 * dScMg3DEsp_c). Sets up the camera and both display engines, loads the two
 * models, the texture animation and the dMg3DEspModel_c block, then calls
 * OnYoshiTryEat(-1). */
extern "C" {
void Camera_UpdateMatrices(void* self);
int func_020179b4(void* r0, void* r1, int r2);
/* Scalar speed: calling SetFile through TextureTransformer.h with a
   Fix12<int> by value adds 12 or more bytes. */
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* self, void* bta, int a, int b, unsigned int d);
int LoadFile(int handle);
unsigned _ZN3G2S13GetBG2CharPtrEv(void);
void DecompressLZ16(int src, void* dst);
void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void func_ov006_020e8aac(char* c);

extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
}

/* Both models' starting matrix. Declaring it non-volatile adds 32 bytes to
   InitResources, and copying it without the stack temporary removes 32. */
extern volatile Matrix4x3 data_ov006_0213c88c;

s32 dScMg3DEsp_c::InitResources()
{
    Matrix4x3 tmp;
    int f;
    int r5v;
    int r4v;

    data_0209d45c = 0x11;
    data_0209d454 = 0x10;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    InitialiseVramGlobals();

    /* main BG0CNT: priority 1 */
    *(volatile unsigned short*)0x4000008 = (*(volatile unsigned short*)0x4000008 & ~3) | 1;
    func_ov004_020b04d0(0x10);

    mCameraEyeX = 0;
    mCameraEyeY = 0xd0000;
    mCameraEyeZ = 0x40000;
    mCameraTargetX = 0xffed3000;
    mCameraTargetY = 0xe0000;
    mCameraTargetZ = 0x40000;
    mCameraAngle = 0xc00;
    Camera_UpdateMatrices(pad_4660); /* the camera block starts at 0x4660 */

    if (func_020179b4(&data_ov006_02141e9c, mModel1, 1) == 0) return 0;

    /* word 1 of a SharedFilePtr is its loaded file; SharedFilePtr.h declares no fields */
    TextureTransformer::Prepare(**(BMD_File**)((void**)&data_ov006_02141e9c + 1), *(BTA_File*)&data_ov006_0213c844);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(mTextureTransformer, &data_ov006_0213c844, 0, 0x1000, 0);

    if (((dMg3DEspModel_c*)pad_4fd8)->InitResources() == 0) return 0;

    if (func_020179b4(&data_ov006_02141e74, mModel2, 1) == 0) return 0;

    tmp = (Matrix4x3&)data_ov006_0213c88c;
    ((Model*)mModel1)->mat4x3 = (Matrix4x3&)data_ov006_0213c88c;
    ((Model*)mModel2)->mat4x3 = tmp;

    data_0209d454 |= 4;
    /* sub BG2CNT, then the sub BG2 scroll offsets */
    *(volatile unsigned short*)0x400100c &= ~3;
    *(volatile unsigned short*)0x400100c &= ~0x40;
    *(volatile unsigned int*)0x4001018 = 0;
    *(volatile unsigned short*)0x400100c = (*(volatile unsigned short*)0x400100c & 0x43) | 0x210;

    f = LoadFile(0x12);
    DecompressLZ16(f, (void*)(_ZN3G2S13GetBG2CharPtrEv() + 0x4000));
    Deallocate((void*)f);

    f = LoadFile(0x13);
    _ZN3GXS10LoadBGPlttEPKvjj((const void*)f, 0x1e0, 0x20);
    Deallocate((void*)f);

    f = LoadFile(0x14);
    func_02056374((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    r5v = LoadFile(8);
    r4v = LoadFile(9);
    DecompressLZ16(r5v, (void*)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void*)r4v, 0, 0x100);
    Deallocate((void*)r5v);
    Deallocate((void*)r4v);

    func_ov004_020b04d0(0x20);

    OnYoshiTryEat(-1);

    *(unsigned short*)(pad_5540 + 0x8) = 0x40; /* 0x5548 */

    unk_0a8 = 3;
    unk_0ac = unk_0a8;
    unk_553c = 1;

    func_ov006_020e8aac((char*)this);

    unk_0a4 = 1;

    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);

    data_ov004_020bc880 = 0x80;
    data_ov004_020bc884 = ~0x3f;

    mHudScore = 0;

    return 1;
}
