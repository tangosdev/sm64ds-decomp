/* _ZN21ExtendingMeshCollider9GetNormalEsR7Vector3 at 0x0203a4dc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef int Fix12i;
struct Vector3 { int x, y, z; };
extern void NormalizeVec3(struct Vector3 *src, struct Vector3 *dst);
extern void func_02039db8(void *thiz, struct Vector3 *vec, struct Vector3 *res);

void _ZN21ExtendingMeshCollider9GetNormalEsR7Vector3(char *c, int idx, struct Vector3 *res) {
    char *p = *(char **)(c + 0x20);
    char *ip = *(char **)(p + 8);
    char *verts = *(char **)(p + 4);
    unsigned short n = *(unsigned short *)(ip + idx*16 + 6);
    short *v = (short *)(verts + n*6);
    struct Vector3 tmp;
    Fix12i sy;
    tmp.x = v[0] << 2;
    tmp.y = v[1] << 2;
    tmp.z = v[2] << 2;
    sy = *(Fix12i*)(c + 0x1c8);
    tmp.x = (Fix12i)(((long long)tmp.x * sy + 0x800) >> 12);
    sy = *(Fix12i*)(c + 0x1c8);
    tmp.z = (Fix12i)(((long long)tmp.z * sy + 0x800) >> 12);
    NormalizeVec3(&tmp, &tmp);
    func_02039db8(c, &tmp, res);
}