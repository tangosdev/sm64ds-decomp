/* func_ov002_020bbac8 — teleport self to linked actor's position (+100fx up),
 * clear two fields, move the link pointer from 0x59c to 0x5a0.
 * No callees.
 */

typedef unsigned int u32;

typedef struct Vec3 {
    int x, y, z;
} Vec3;

struct Obj {
    char _pad0[0x5c];
    Vec3 pos;                 /* 0x5c */
    char _pad1[0x98 - 0x68];
    u32 unk98;                /* 0x98 */
    char _pad2[0xa8 - 0x9c];
    u32 unkA8;                /* 0xa8 */
    char _pad3[0x59c - 0xac];
    struct Obj* unk59C;       /* 0x59c */
    struct Obj* unk5A0;       /* 0x5a0 */
};

void func_ov002_020bbac8(struct Obj* self)
{
    struct Obj* other;
    int* py;
    Vec3* src;
    self->unk98 = 0;
    self->unkA8 = 0;
    other = self->unk59C;
    py = (int*)(((long long)(int)((char*)self + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
    src = (Vec3*)(((long long)(int)((char*)other + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    self->pos.x = src->x;
    self->pos.y = src->y;
    self->pos.z = src->z;
    *py += 0x64000;
    self->unk5A0 = self->unk59C;
    self->unk59C = 0;
}
