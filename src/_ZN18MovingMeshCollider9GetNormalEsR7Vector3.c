struct Vector3 { int x, y, z; };
extern void func_02039db8(void *thiz, struct Vector3 *vec, struct Vector3 *res);
void _ZN18MovingMeshCollider9GetNormalEsR7Vector3(char *c, int idx, struct Vector3 *res) {
    char *p = *(char **)(c + 0x20);
    char *ip = *(char **)(p + 8);
    char *verts = *(char **)(p + 4);
    unsigned short n = *(unsigned short *)(ip + idx*16 + 6);
    short *v = (short *)(verts + n*6);
    struct Vector3 tmp;
    tmp.x = v[0] << 2;
    tmp.y = v[1] << 2;
    tmp.z = v[2] << 2;
    func_02039db8(c, &tmp, res);
}
