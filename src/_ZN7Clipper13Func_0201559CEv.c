/* _ZN7Clipper13Func_0201559CEv at 0x0201559c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef int s32;
typedef s32 Fix12i;
typedef struct { s32 x, y, z; } Vector3;

struct Clipper {
    char pad0[4];        /* 0x00 */
    Vector3 v[4];        /* 0x04, 0x10, 0x1c, 0x28 */
    char pad1[0x18];     /* 0x34 .. 0x4b */
    s32 m4c;             /* 0x4c */
    s32 m50;             /* 0x50 */
    char pad2[4];        /* 0x54 */
    unsigned short m58;  /* 0x58 */
};

extern int _ZN4cstd4fdivEii(int a, int b);
extern void CrossVec3(Vector3* a, Vector3* b, Vector3* result);
extern void NormalizeVec3(Vector3* src, Vector3* dst);
extern short data_02082214[];

void _ZN7Clipper13Func_0201559CEv(struct Clipper* thiz)
{
    int idx = (int)thiz->m58 >> 4;
    int q = _ZN4cstd4fdivEii(data_02082214[2 * idx], data_02082214[2 * idx + 1]);
    Fix12i b = (Fix12i)(((long long)thiz->m50 * q + 0x800) >> 12);
    Fix12i c = (Fix12i)(((long long)thiz->m4c * b + 0x800) >> 12);
    Vector3 v0, v1, v2, v3;
    v0.x = -c; v0.y = -b; v0.z = -thiz->m50;
    v1.x = -c; v1.y = b;  v1.z = -thiz->m50;
    v2.x = c;  v2.y = b;  v2.z = -thiz->m50;
    v3.x = c;  v3.y = -b; v3.z = -thiz->m50;
    CrossVec3(&v1, &v0, &thiz->v[0]);
    CrossVec3(&v2, &v1, &thiz->v[1]);
    CrossVec3(&v3, &v2, &thiz->v[2]);
    CrossVec3(&v0, &v3, &thiz->v[3]);
    NormalizeVec3(&thiz->v[0], &thiz->v[0]);
    NormalizeVec3(&thiz->v[1], &thiz->v[1]);
    NormalizeVec3(&thiz->v[2], &thiz->v[2]);
    NormalizeVec3(&thiz->v[3], &thiz->v[3]);
}