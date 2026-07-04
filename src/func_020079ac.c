typedef unsigned char u8;
typedef unsigned short u16;

typedef struct { int x, y, z; } Vector3;

extern short ReadUnalignedShort(unsigned char *p);
extern void func_02007c9c(const Vector3 *v0, const Vector3 *v1, int *outDist,
                          short *outVertAng, short *outHorzAng);
extern void func_02007c14(Vector3 *res, const Vector3 *trans, int mag,
                          short ang, short angY);

extern u16 data_0209b274;

int func_020079ac(char *self, unsigned char *p, int tag) {
    int dist;
    short vert, horz;
    if (tag == data_0209b274) {
        *(int*)(self + 0xb0) = *(int*)(self + 0x80);
        *(int*)(self + 0xb4) = *(int*)(self + 0x84);
        *(int*)(self + 0xb8) = *(int*)(self + 0x88);
    }
    *(short*)(((int)self + 0x1a2) & 0xFFFFFFFFFFFFFFFF) -= ReadUnalignedShort(p);
    *(short*)(((int)self + 0x1a4) & 0xFFFFFFFFFFFFFFFF) += ReadUnalignedShort(p + 2);
    func_02007c9c((const Vector3*)(self + 0x8c), (const Vector3*)(self + 0xb0),
                  &dist, &vert, &horz);
    vert = (short)(vert + *(short*)(self + 0x1a2));
    horz = (short)(horz + *(short*)(self + 0x1a4));
    func_02007c14((Vector3*)(self + 0x80), (const Vector3*)(self + 0x8c),
                  dist, vert, horz);
    return 1;
}
