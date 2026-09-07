// @symbol func_ov080_021274ac
/* recovered: shared common types */
#include "common.h"
extern int _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* o, int bmd, int a, int b);
extern void func_ov080_02127094(char* t);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* o);
extern int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* f);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* o, int kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(int* p, int v);
extern void func_020393c4(int* p, int v);
extern void Vec3_Sub(struct Vector3* out, struct Vector3* a, struct Vector3* b);
extern int LenVec3(struct Vector3* v);
extern short Vec3_HorzAngle(struct Vector3* a, struct Vector3* b);
extern int AngleDiff(int a, int b);
extern int _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern int func_ov080_02127658;
int func_ov080_021274ac(char* self, char** arg) {
    struct Vector3 d;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(arg[0]), 1, -1);
    func_ov080_02127094(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(self + 0x124, _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(arg[1]), self + 0x2ec, 0x1000, *(short*)(self + 0x8e), arg[2]);
    func_020393d4((int*)(self + 0x124), (int)&_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4((int*)(self + 0x124), (int)&func_ov080_02127658);
    *(int*)(self + 0x320) = *(int*)(self + 0x5c);
    *(int*)(self + 0x324) = *(int*)(self + 0x60);
    *(int*)(self + 0x328) = *(int*)(self + 0x64);
    Vec3_Sub(&d, (struct Vector3*)(self + 0x5c), (struct Vector3*)(self + 0x320));
    LenVec3(&d);
    *(int*)(self + 0x338) = AngleDiff(Vec3_HorzAngle((struct Vector3*)(self + 0x5c), (struct Vector3*)(self + 0x320)), *(short*)(self + 0x8e));
    return 1;
}
