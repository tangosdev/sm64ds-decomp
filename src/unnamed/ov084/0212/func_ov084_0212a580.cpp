//cpp
// @symbol func_ov084_0212a580
/* recovered: shared common types */
#include "common.h"

struct Vector3_16_local { unsigned short x, y, z; };

extern "C" {
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* o);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int fix, int t, unsigned int j);
extern short data_02082214[];
}

struct dCapEnemy_c {
    void UpdateCapPos(const Vector3&, const Vector3_16_local&);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16(void *, const Vector3&, const Vector3_16_local&);


extern "C" void func_ov084_0212a580(char* c){
    Vector3_16_local s16;
    Vector3 pos;
    Vector3 arg;
    Vector3_16_local arg16;

    Matrix4x3_FromRotationY(c + 0x38c, *(short*)(c + 0x8e));
    *(int*)(c + 0x3b0) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3b4) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x3b8) = *(int*)(c + 0x64) >> 3;
    s16.x = *(short*)(c + 0x8c);
    s16.y = *(short*)(c + 0x8e);
    s16.z = *(short*)(c + 0x90);
    if ((*(int*)(c + 0xb0) & 0x40000 ? 1 : 0) == 0) {
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x1b4)) {
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x3d4, c + 0x38c, *(int*)(c + 0x80) * 0x50, 0x1e000, 0xf);
        } else {
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x3d4, c + 0x38c, *(int*)(c + 0x80) * 0x50, 0x96000, 0xf);
        }
    }
    pos.x = 0;
    pos.z = 0;
    pos.y = 0x6c000;
    pos.x += ((short*)data_02082214)[(*(unsigned short*)(c + 0x8e) >> 4) * 2] * 10;
    pos.z += ((short*)data_02082214)[(*(unsigned short*)(c + 0x8e) >> 4) * 2 + 1] * 10;
    arg.x = ((int*)&pos)[0];
    arg.y = ((int*)&pos)[1];
    arg.z = ((int*)&pos)[2];
    arg16.x = ((unsigned short*)&s16)[0];
    arg16.y = ((unsigned short*)&s16)[1];
    arg16.z = ((unsigned short*)&s16)[2];
    /* equal-arm ternary forces arg16 setup (r2) before arg (r1) — matches ROM call-arg order */
    _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16((dCapEnemy_c*)c, arg, c ? arg16 : arg16);
}
