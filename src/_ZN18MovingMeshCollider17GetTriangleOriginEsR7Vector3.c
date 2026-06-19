struct Vector3 { int x, y, z; };
extern void func_02039e30(void *thiz, struct Vector3 *v, struct Vector3 *res);
void _ZN18MovingMeshCollider17GetTriangleOriginEsR7Vector3(char *c, int idx, struct Vector3 *res) {
    char *p = *(char **)(c + 0x20);
    char *ip = *(char **)(p + 8);
    char *verts = *(char **)(p);
    unsigned short n = *(unsigned short *)(ip + idx*16 + 4);
    int *v = (int *)(verts + n*12);
    struct Vector3 tmp;
    tmp.x = v[0] << 6;
    tmp.y = v[1] << 6;
    tmp.z = v[2] << 6;
    func_02039e30(c, &tmp, res);
}
