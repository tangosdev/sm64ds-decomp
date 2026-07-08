typedef int s32; typedef short s16;
typedef struct { s32 x,y,z; } Vector3;
struct Matrix4x3 { s32 m[9]; s32 tx,ty,tz; };
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;
extern void Matrix4x3_FromRotationY(struct Matrix4x3*, s16);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3*, s16);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3*, s32, s32, s32);
extern void AddVec3(const Vector3*, const Vector3*, Vector3*);
struct Actor { char pad[0x40]; Vector3 f40; char pad2[0xc]; s16 f58; s16 f5a; };
void func_ov027_021123b0(struct Actor* c) {
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, c->f5a);
    Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, c->f58);
    Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, 0, 0x50000);
    Vector3 v;
    v.x = MATRIX_SCRATCH_PAPER.tx;
    v.y = MATRIX_SCRATCH_PAPER.ty;
    v.z = MATRIX_SCRATCH_PAPER.tz;
    AddVec3(&c->f40, &v, &c->f40);
}
