typedef int Fix12i;
typedef struct { int x, y, z; } Vector3;

extern void func_02007c9c(const Vector3 *v0, const Vector3 *v1, int *outDist, short *outVertAng, short *outHorzAng);
extern int func_0200e728(unsigned char *p);
extern short ReadUnalignedShort(unsigned char *p);
extern void func_02007c14(Vector3 *res, const Vector3 *trans, Fix12i mag, short ang, short angY);

int func_02007910(void *thiz, unsigned char *p)
{
    char *t = (char *)thiz;
    Vector3 *v = (Vector3 *)(*(char **)(t + 0x110) + 0x5c);
    int dist;
    short vert;
    short horz;

    func_02007c9c(v, (Vector3 *)(t + 0x8c), &dist, &vert, &horz);
    dist = func_0200e728(p) + dist;
    vert = (short)(ReadUnalignedShort(p + 4) + vert);
    horz = (short)(ReadUnalignedShort(p + 6) + horz);
    func_02007c14((Vector3 *)(t + 0x8c), v, dist, vert, horz);
    return 1;
}
