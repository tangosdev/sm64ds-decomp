typedef int Fix12i;

struct Data {
    short x;   /* 0 */
    short y;   /* 2 */
    short z;   /* 4 */
    short rx;  /* 6 */
    short ry;  /* 8 */
    short rz;  /* 0xa */
};

struct Obj {
    char pad[0x8c];
    Fix12i px; /* 0x8c */
    Fix12i py; /* 0x90 */
    Fix12i pz; /* 0x94 */
};

extern short ReadUnalignedShort(const void *p);
extern void Math_Function_0203b0fc(int *dest, int target, int scale, int max);

int func_02007e5c(struct Obj *self, struct Data *data) {
    int pos[3];
    int rot[3];
    int *pp, *rp;
    Fix12i px, py, pz;
    int rx, ry, rz;
    pz = ReadUnalignedShort(&data->z) << 12;
    py = ReadUnalignedShort(&data->y) << 12;
    px = ReadUnalignedShort(&data->x) << 12;
    pos[0] = px;
    pos[1] = py;
    pos[2] = pz;
    rz = ReadUnalignedShort(&data->rz);
    ry = ReadUnalignedShort(&data->ry);
    rx = ReadUnalignedShort(&data->rx);
    rot[0] = rx;
    rot[1] = ry;
    rot[2] = rz;
    pp = pos;
    rp = rot;
    Math_Function_0203b0fc(&self->px, pp[0], rp[0], 0x7fffffff);
    pp++; rp++;
    Math_Function_0203b0fc(&self->py, pp[0], rp[0], 0x7fffffff);
    pp++; rp++;
    Math_Function_0203b0fc(&self->pz, pp[0], rp[0], 0x7fffffff);
    return 1;
}
