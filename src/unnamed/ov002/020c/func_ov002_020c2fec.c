struct Anim {
    short a;
    short x;
    short y;
    short z;
};

struct Obj {
    char pad0[0x730];
    unsigned char idx;
    char pad1[3];
    int f734;
    int f738;
};

extern struct Anim data_ov002_0210a8b8[];
extern void func_ov002_020c3160(struct Obj *obj, int *dst);

int func_ov002_020c2fec(struct Obj *obj, int *out)
{
    int tmp[4];
    int i;
    int ret;
    struct Anim *row;

    ret = 0;
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    func_ov002_020c3160(obj, tmp);

    row = &data_ov002_0210a8b8[obj->idx];
    for (i = 0; i < 4; i++) {
        out[0] += tmp[i] * row[i].x;
        out[1] += tmp[i] * row[i].y;
        out[2] += tmp[i] * row[i].z;
    }

    obj->f734 += (row[0].a << 12) / 1000;
    if (obj->f734 >= 0x1000) {
        obj->idx++;
        obj->f734 -= 0x1000;
        switch (obj->f738) {
        case 5:
            ret = 1;
            break;
        case 3:
            if (data_ov002_0210a8b8[obj->idx + 2].a == 0) {
                obj->f738 = 4;
            }
            break;
        default:
            obj->f738++;
            break;
        }
    }
    return ret;
}
