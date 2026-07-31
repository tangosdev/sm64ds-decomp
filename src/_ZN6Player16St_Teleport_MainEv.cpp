//cpp
// @symbol _ZN6Player16St_Teleport_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct Obj { s16 x, y, z; u16 param; } Obj;

extern "C" {
extern Obj* GetTeleportDestObj(int i);
extern void func_02035860(char* o, void* src);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, char* v);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, char* v);
extern void _ZN13RaycastGroundC1Ev(char* rc);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(char* rc, Vector3* v, char* actor);
extern int _ZN13RaycastGround10DetectClsnEv(char* rc);
extern void _ZN13RaycastGroundD1Ev(char* rc);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* s);
extern u32 func_02012790(u32 a);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16* p, u16 a, s16 b);
extern void func_ov002_020bedd4(char* c);

extern char* data_0209f318;
extern int data_0209f32c;
extern u8 data_0209f284;
extern int data_ov002_0211067c[];
extern int data_ov002_0211013c[];
}

int Player::St_Teleport_Main()
{
    switch (mStateStep) {
    case 0:
        if (mOpacity > 1) {
            (*(u8*)(((int)((char*)this) + 0x6f5) & 0xFFFFFFFFFFFFFFFF))--;
            break;
        }
        (*(u8*)(((int)((char*)this) + 0x6e5) & 0xFFFFFFFFFFFFFFFF))++;
        if (mStateWork >= 0x10) {
            Obj* obj = GetTeleportDestObj((u8)(unk_6e8 - 1));
            int tx = obj->x << 12;
            int tz = obj->z << 12;
            int ty = obj->y << 12;
            mPosX = tx;
            mPosY = ty;
            mPosZ = tz;
            mAreaId = obj->param & 0xf;
            mAngleY = ((obj->param >> 4) & 0xf) << 12;
            mTargetAngleY = mAngleY;
            mGroundY = 0x80000000;
            unk_64c = 0x80000000;
            func_02035860(((char*)this) + 0x380, ((char*)this) + 0x5c);
            mOpacity = 0;
            func_0200d1e4(data_0209f318);
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x19, ((char*)this) + 0x74);
            _ZN5Sound9PlayBank0EjRK7Vector3(0x19, ((char*)this) + 0x74);
            mStateStep = 1;
        }
        break;
    case 1:
        if (mStateWork != 0) {
            if (mStateWork >= 0x10) {
                if (data_0209f32c > mPosY + 0x64000)
                    mIsUnderwater = 1;
            }
            (*(u8*)(((int)((char*)this) + 0x6e5) & 0xFFFFFFFFFFFFFFFF))--;
            break;
        }
        (*(u8*)(((int)((char*)this) + 0x6f5) & 0xFFFFFFFFFFFFFFFF))++;
        func_02020388(mPlayerNo);
        if (mOpacity >= 0x1f) {
            int hit = 0;
            mOpacity = 0x1f;
            if (data_0209f32c >= mPosY + 0x64000) {
                Vector3 v;
                char rc[0x50];
                _ZN13RaycastGroundC1Ev(rc);
                {
                    int vz = mPosZ;
                    int vy = data_0209f32c + 0x64000;
                    int vx = mPosX;
                    v.x = vx;
                    v.y = vy;
                    v.z = vz;
                }
                _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rc, &v, ((char*)this));
                if (_ZN13RaycastGround10DetectClsnEv(rc))
                    hit = 1;
                _ZN13RaycastGroundD1Ev(rc);
            }
            if (hit)
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211067c);
            else
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
            data_0209f284 = 1;
            unk_6c8 = 0x3c;
            func_02012790(0x24);
        }
        break;
    }
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16*)0x4000050, 0x3f, mStateWork);
    func_ov002_020bedd4(((char*)this));
    return 1;
}
