typedef int s32;
typedef short s16;
typedef unsigned int u32;

typedef struct { s32 x, y, z; } Vector3;

extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern void Matrix4x3_FromRotationXYZExt(void* m, int x, int y, int z);
extern void MulVec3Mat4x3(const Vector3* v, const void* m, Vector3* dst);
extern void Vec3_Add(Vector3* out, const Vector3* a, const Vector3* b);
extern int func_ov002_020b6a80(char* c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void*);

extern Vector3 data_ov002_0210ddd0[4];
extern char data_020a0e68[0x30];

struct Obj {
    char pad0[0x5c];
    Vector3 pos;              /* 0x5c */
    char pad1[0x8c - 0x68];
    s16 rx;                   /* 0x8c */
    s16 ry;                   /* 0x8e */
    s16 rz;                   /* 0x90 */
    char pad2[0xb0 - 0x92];
    u32 flags;                /* 0xb0 */
    char pad3[0x320 - 0xb4];
    u32 ids[4];                /* 0x320 */
};

int func_ov002_020b6b38(struct Obj* c)
{
    int i;
    Vector3 mid;
    Vector3 res;
    short* rzp = (short*)((char*)c + 0x90);
    *rzp = *rzp + 0x100;
    {
        int b = (int)((c->flags & 8) != 0);
        if (b != 0) return 1;
    }
    for (i = 0; i < 4; i++) {
        if (c->ids[i]) {
            void* a = _ZN5Actor10FindWithIDEj(c->ids[i]);
            if (a) {
                mid.x = 0;
                mid.y = 0;
                mid.z = 0;
                Matrix4x3_FromRotationXYZExt(data_020a0e68, c->rx, c->ry, c->rz);
                MulVec3Mat4x3(&data_ov002_0210ddd0[i], data_020a0e68, &mid);
                Vec3_Add(&res, &c->pos, &mid);
                *(int*)((char*)a + 0x5c) = res.x;
                *(int*)((char*)a + 0x60) = res.y;
                *(int*)((char*)a + 0x64) = res.z;
            }
        }
    }
    func_ov002_020b6a80((char*)c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
