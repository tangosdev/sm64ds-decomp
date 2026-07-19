//cpp
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

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

#pragma opt_common_subs off

extern "C" {

extern int _ZN6Player7IsStateERNS_5StateE(char* self, void* state);
extern int _ZNK6Player14GetBodyModelIDEjb(char* self, u32 a, int b);
extern int func_ov002_020becf4(char* self, u32 a, int b);
extern void _ZN9ModelBase12ApplyOpacityEj(void* self, u32 a, int b);
extern void _ZN5Model6RenderEPK7Vector3(void* self, void* pos);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* self, void* mc);
extern void func_ov002_020e3e00(void* mdl, void* pos, u32 a);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void* m, int ang);
extern void MulMat4x3Mat4x3(void* a, void* b, void* c);
extern int func_ov002_020bea7c(char* self);
extern int func_ov002_020d225c(char* self);
extern u8 data_0209f2d8;
extern u8 data_0209fc5c[];
extern int data_ov002_021100c4;
extern int data_020a0e68;

int _ZN6Player6RenderEv(char* c)
{
    int t = (data_0209f2d8 == 1);
    if (t != false) {
        if (data_0209fc5c[*(u8*)(c + 0x6d8)] == 0)
            return 1;
    }
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021100c4)) {
        if (*(u16*)(c + 0x6a0) & 2)
            return 1;
    } else {
        if (*(u16*)(c + 0x6a0) & 1)
            return 1;
    }
    if (*(u8*)(c + 0x6f5) == 0)
        return 1;
    if (!(*(int*)(c + 0xb0) & 0x10)) {
        char* bodyMdl;
        int i;
        bodyMdl = *(char**)(c + 0xdc + _ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6db), 1) * 4);
        if (*(u8*)(c + 0x6fd) == 0) {
            _ZN9ModelBase12ApplyOpacityEj(bodyMdl, *(u8*)(c + 0x6f5), 0);
            {
                int t2 = (data_0209f2d8 == 1);
                if (t2 != false) {
                    if (*(int*)(c + 8) == 3) {
                        int* p = (int*)AT(bodyMdl, 8);
                        char* hdr = (char*)p[0];
                        char* rec = (char*)p[1];
                        u32 i2;
                        for (i2 = 0; i2 < *(u32*)(hdr + 0x24); i2++) {
                            *(int*)(rec + 0x20) = *(int*)(c + 0x61c);
                            rec += 0x30;
                        }
                    }
                }
            }
            _ZN5Model6RenderEPK7Vector3(bodyMdl, c + 0x80);
            if (*(int*)(c + 8) == 1) {
                _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x254, *(char**)(c + 0xe0) + 8);
                *(int*)(c + 0x25c) = *(u8*)(c + 0x6fc) << 12;
            }
            func_ov002_020e3e00(bodyMdl, c + 0x80, *(u8*)(c + 0x6f5));
            if (*(u8*)(c + 0x700) != 0) {
                _ZN9ModelBase12ApplyOpacityEj(c + 0x174, *(u8*)(c + 0x6f5), 0);
                if (*(u8*)(c + 0x6db) == 3) {
                    Matrix4x3_FromTranslation(&data_020a0e68, -0x1b33, -0x666, 0);
                    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, 0x4000);
                    MulMat4x3Mat4x3(&data_020a0e68, *(char**)(bodyMdl + 0x14) + 0x180, &data_020a0e68);
                    ((VObj*)(c + 0x174))->m18(&data_020a0e68, c + 0x80);
                } else {
                    ((VObj*)(c + 0x174))->m18(*(char**)(bodyMdl + 0x14) + 0x2d0, c + 0x80);
                }
            }
        } else {
            _ZN9ModelBase12ApplyOpacityEj(c + 0xf0, *(u8*)(c + 0x6f5), 0);
            ((VObj*)(c + 0xf0))->m14(c + 0x80);
        }
        i = func_ov002_020becf4(c, *(u8*)(c + 0x6db), 1);
        {
            char* mdl4 = *(char**)(c + 0x154 + i * 4);
            if (mdl4 != 0 && i != 9 && i != 8) {
                _ZN9ModelBase12ApplyOpacityEj(mdl4, *(u8*)(c + 0x6f5), 0);
                {
                    int t3 = (data_0209f2d8 == 1);
                    if (t3 != false) {
                        if (*(int*)(c + 8) == 3) {
                            int* p = (int*)AT(*(char**)(c + 0x154 + i * 4), 8);
                            char* hdr = (char*)p[0];
                            char* rec = (char*)p[1];
                            u32 i2;
                            for (i2 = 0; i2 < *(u32*)(hdr + 0x24); i2++) {
                                *(int*)(rec + 0x20) = *(int*)(c + 0x61c);
                                rec += 0x30;
                            }
                        }
                    }
                }
                if (i == 3) {
                    _ZN5Model6RenderEPK7Vector3(*(char**)(c + 0x154 + i * 4), c + 0x80);
                } else {
                    char* m = *(char**)(c + 0x154 + i * 4);
                    char* dst = *(char**)(m + 0x14);
                    char* src = *(char**)(bodyMdl + 0x14) + 0x2d0;
                    *(M34*)dst = *(M34*)src;
                    ((VObj*)*(char**)(c + 0x154 + i * 4))->m14(c + 0x80);
                }
                if (*(int*)(c + 8) == 1) {
                    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x268, *(char**)(c + 0x158) + 8);
                    *(int*)(c + 0x270) = *(u8*)(c + 0x6fc) << 12;
                }
                if (func_ov002_020bea7c(c) == 0) {
                    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x1dc + *(u8*)(c + 0x6db) * 0x14, *(char**)(c + 0x154 + i * 4) + 8);
                }
                func_ov002_020e3e00(*(char**)(c + 0x154 + i * 4), c + 0x80, *(u8*)(c + 0x6f5));
                if (func_ov002_020d225c(c)) {
                    ((VObj*)*(char**)(c + 0x1d8))->m14(c + 0x56c);
                }
            }
        }
    }
    return 1;
}

}
