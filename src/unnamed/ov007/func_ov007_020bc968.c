#include "types.h"
extern void Matrix4x3_LoadIdentity(void* mat);
extern void func_ov007_020c39f8(void* mat, int a, int b, int c);
extern void MulMat4x3Mat4x3(void* d, void* a, void* b);

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc434, only the slots this file touches. */
typedef struct Ov007Model {
    char pad_00[0x34];
    Vector3 pos; /* 0x34: copied into the matrix translation */
    char pad_40[0xc];
    u16 rotX; /* 0x4c: func_ov007_020c39f8's angles */
    u16 rotY; /* 0x4e */
    u16 rotZ; /* 0x50 */
    char pad_52[2];
    /* 0x54: the Matrix4x3 func_ov007_020bbff0 renders the model with;
     * [9..11] is its translation */
    s32 matrix[12];
} Ov007Model;

typedef char Ov007Model_size_must_be_0x84[sizeof(Ov007Model) == 0x84 ? 1 : -1];

/* Builds the model's matrix: rotation, then pos as the translation, then
 * combined with `other` through MulMat4x3Mat4x3. */
void func_ov007_020bc968(char* c, void* other) {
    Ov007Model *m = (Ov007Model *)c;
    Matrix4x3_LoadIdentity(m->matrix);
    func_ov007_020c39f8(m->matrix, m->rotX, m->rotY, m->rotZ);
    m->matrix[9] = m->pos.x;
    m->matrix[10] = m->pos.y;
    m->matrix[11] = m->pos.z;
    MulMat4x3Mat4x3(m->matrix, other, m->matrix);
}
