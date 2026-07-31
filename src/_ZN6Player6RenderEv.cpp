//cpp
// @symbol _ZN6Player6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct VObj {
    virtual void d00();
    virtual void d04();
    virtual void d08();
    virtual void d0c();
    virtual void d10();
    virtual void m14(void* a);
    virtual void m18(void* a, void* b);
};

typedef struct M34 { int m[12]; } M34;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))

#pragma opt_common_subs off

extern "C" {

extern int _ZN6Player7IsStateERNS_5StateE(char* self, void* state);
extern int _ZNK6Player14GetBodyModelIDEjb(char* self, u32 a, int b);
extern int func_ov002_020becf4(char* self, u32 a, int b);
extern void _ZN9ModelBase12ApplyOpacityEj(void* self, u32 a, int b);
extern void _ZN5Model6RenderEPK7Vector3(void* self, void* pos);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* self, void* mc);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void* m, int ang);
extern void MulMat4x3Mat4x3(void* a, void* b, void* c);
extern int func_ov002_020d225c(char* self);
extern u8 data_0209f2d8;
extern u8 data_0209fc5c[];
extern int data_ov002_021100c4;
extern int data_020a0e68;
}

int Player::Render()
{
    int t = (data_0209f2d8 == 1);
    if (t != false) {
        if (data_0209fc5c[mPlayerNo] == 0)
            return 1;
    }
    if (_ZN6Player7IsStateERNS_5StateE(((char*)this), &data_ov002_021100c4)) {
        if (mInvincibleTimer & 2)
            return 1;
    } else {
        if (mInvincibleTimer & 1)
            return 1;
    }
    if (mOpacity == 0)
        return 1;
    if (!(unk_0b0 & 0x10)) {
        char* bodyMdl;
        int i;
        bodyMdl = *(char**)(((char*)this) + 0xdc + _ZNK6Player14GetBodyModelIDEjb(((char*)this), unk_6db, 1) * 4);
        if (mIsBalloon == 0) {
            _ZN9ModelBase12ApplyOpacityEj(bodyMdl, mOpacity, 0);
            {
                int t2 = (data_0209f2d8 == 1);
                if (t2 != false) {
                    if (mParam == 3) {
                        int* p = (int*)AT(bodyMdl, 8);
                        char* hdr = (char*)p[0];
                        char* rec = (char*)p[1];
                        u32 i2;
                        for (i2 = 0; i2 < *(u32*)(hdr + 0x24); i2++) {
                            *(int*)(rec + 0x20) = unk_61c;
                            rec += 0x30;
                        }
                    }
                }
            }
            _ZN5Model6RenderEPK7Vector3(bodyMdl, ((char*)this) + 0x80);
            if (mParam == 1) {
                _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this) + 0x254, *(char**)((char*)&unk_0e0) + 8);
                unk_25c = unk_6fc << 12;
            }
            func_ov002_020e3e00(bodyMdl, ((char*)this) + 0x80, mOpacity);
            if (unk_700 != 0) {
                _ZN9ModelBase12ApplyOpacityEj(((char*)this) + 0x174, mOpacity, 0);
                if (unk_6db == 3) {
                    Matrix4x3_FromTranslation(&data_020a0e68, -0x1b33, -0x666, 0);
                    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, 0x4000);
                    MulMat4x3Mat4x3(&data_020a0e68, *(char**)(bodyMdl + 0x14) + 0x180, &data_020a0e68);
                    ((VObj*)((char*)&mModelAnim4))->m18(&data_020a0e68, ((char*)this) + 0x80);
                } else {
                    ((VObj*)((char*)&mModelAnim4))->m18(*(char**)(bodyMdl + 0x14) + 0x2d0, ((char*)this) + 0x80);
                }
            }
        } else {
            _ZN9ModelBase12ApplyOpacityEj(((char*)this) + 0xf0, mOpacity, 0);
            ((VObj*)((char*)&mModelAnim3))->m14((char*)&mScaleX);
        }
        i = func_ov002_020becf4(((char*)this), unk_6db, 1);
        {
            char* mdl4 = *(char**)(((char*)this) + 0x154 + i * 4);
            if (mdl4 != 0 && i != 9 && i != 8) {
                _ZN9ModelBase12ApplyOpacityEj(mdl4, mOpacity, 0);
                {
                    int t3 = (data_0209f2d8 == 1);
                    if (t3 != false) {
                        if (mParam == 3) {
                            int* p = (int*)AT(*(char**)(((char*)this) + 0x154 + i * 4), 8);
                            char* hdr = (char*)p[0];
                            char* rec = (char*)p[1];
                            u32 i2;
                            for (i2 = 0; i2 < *(u32*)(hdr + 0x24); i2++) {
                                *(int*)(rec + 0x20) = unk_61c;
                                rec += 0x30;
                            }
                        }
                    }
                }
                if (i == 3) {
                    _ZN5Model6RenderEPK7Vector3(*(char**)(((char*)this) + 0x154 + i * 4), ((char*)this) + 0x80);
                } else {
                    char* m = *(char**)(((char*)this) + 0x154 + i * 4);
                    char* dst = *(char**)(m + 0x14);
                    char* src = *(char**)(bodyMdl + 0x14) + 0x2d0;
                    *(M34*)dst = *(M34*)src;
                    ((VObj*)*(char**)(((char*)this) + 0x154 + i * 4))->m14((char*)&mScaleX);
                }
                if (mParam == 1) {
                    _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this) + 0x268, *(char**)((char*)&unk_158) + 8);
                    unk_270 = unk_6fc << 12;
                }
                if (func_ov002_020bea7c(((char*)this)) == 0) {
                    _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this) + 0x1dc + unk_6db * 0x14, *(char**)(((char*)this) + 0x154 + i * 4) + 8);
                }
                func_ov002_020e3e00(*(char**)(((char*)this) + 0x154 + i * 4), ((char*)this) + 0x80, mOpacity);
                if (func_ov002_020d225c(((char*)this))) {
                    ((VObj*)*(char**)((char*)&unk_1d8))->m14((char*)&unk_56c);
                }
            }
        }
    }
    return 1;
}
