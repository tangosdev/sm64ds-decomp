#include "types.h"
typedef struct { s32 m[12]; } Matrix4x3;

typedef struct {
    char pad0[4];
    u32 count;         /* +4 */
    u32 param8;        /* +8 */
    char padC[0x24];
    u32 unk30;         /* +0x30 */
    u32* matData;      /* +0x34 */
} Obj;

extern void Matrix4x3_LoadIdentity(Matrix4x3* mat);
extern void func_02044f74(Obj* self, u32 param);
extern void InvMat4x3(Matrix4x3* a, Matrix4x3* b);
extern void _ZN4CP1514FlushDataCacheEv(void);

/* Note: only 1 word differs: bls vs beq at count>0 check (+0x3c) */
void func_02044efc(Obj* self) {
    if (!self->unk30) return;
    Matrix4x3* matrices = (Matrix4x3*)self->matData[3];
    Matrix4x3_LoadIdentity(matrices);
    func_02044f74(self, self->param8);
    u32 count = self->count;
    u32 i = 0;
    if (i < count) {
        u32 byteOff = 0;
        do {
            Matrix4x3* mat = (Matrix4x3*)((char*)((Matrix4x3*)self->matData[3]) + byteOff);
            InvMat4x3(mat, mat);
            count = self->count;
            i++;
            byteOff += 0x30;
        } while (i < count);
    }
    _ZN4CP1514FlushDataCacheEv();
}
