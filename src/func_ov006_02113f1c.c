typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct V2 {
    int x;
    int z;
} V2;

extern void func_0203d6d0(V2 *out, V2 *a, V2 *b);
extern int func_0203d614(V2 *p);
extern int func_0203d434(V2 *p);
extern void func_0203d630(V2 *p, int scale);
extern void func_0203d480(V2 *out, V2 *in);
extern int func_0203d524(V2 *a, V2 *b);
extern int func_ov006_02114590(char *c, V2 *a, V2 *b, V2 *d, V2 *e);
extern int func_ov006_02111dcc(char *c, int value);
extern void func_ov006_02111e48(char *c);
extern void func_ov006_021128fc(char *c);
extern void func_ov006_02112ad8(char *c);

#define LAUNDER(p) ((int)((long long)(int)(p) & 0xffffffffffffffffLL))

void func_ov006_02113f1c(char *c)
{
    V2 vc;
    V2 vd;
    V2 d1;
    V2 pa;
    V2 pb;
    V2 pc;
    V2 pd;
    V2 pe;
    V2 pf;
    V2 pg;
    V2 ph;
    V2 v5c;
    V2 d2;
    int t;
    int spd;
    int mag;
    int dist;

    if (*(int *)(c + 0x124) > 0)
        *(int *)LAUNDER(c + 0x124) -= 1;
    if (*(u8 *)(c + 0x30) == 0)
        return;
    if (*(int *)(c + 0x40) > 0) {
        *(int *)LAUNDER(c + 0x40) -= 1;
        if (*(int *)(c + 0x40) <= 0) {
            *(u8 *)(c + 0x30) = 0;
            return;
        }
    }
    if (*(u8 *)(c + 0x110) == 1) {
        *(int *)(c + 8) = *(int *)(c + 0x108);
        *(int *)(c + 0xc) = *(int *)(c + 0x10c);
        *(int *)(c + 0x20) = 0;
        *(int *)(c + 0x24) = 0;
        return;
    }
    if (*(int *)(c + 8) >= 0x8000 && *(int *)(c + 8) < 0xd8000 &&
        *(int *)(c + 0xc) >= 0x74000 && *(int *)(c + 0xc) < 0x7c000) {
        if (*(int *)(c + 0x3c) < 0x1e) {
            *(int *)LAUNDER(c + 0x3c) += 1;
        } else if (*(int *)(c + 0x3c) < 0x12c) {
            *(u8 *)(c + 0x3b) = 1;
            *(int *)LAUNDER(c + 0x3c) += 1;
        } else {
            *(u8 *)(c + 0x3b) = 1;
            if (func_ov006_02111dcc(c, 1) != 0) {
                func_ov006_02111e48(c);
                return;
            }
        }
    } else {
        *(int *)(c + 0x3c) = 0;
    }
    if (*(int *)(c + 0x2c) == *(int *)(*(char **)(c + 4) + 0x4664)) {
        if (*(u8 *)(c + 0x121) != 0)
            *(u8 *)(c + 0x100) = 0;
    }
    *(int *)(c + 0x10) = *(int *)(c + 8);
    *(int *)(c + 0x14) = *(int *)(c + 0xc);
    if (*(int *)(c + 8) < 0 || *(int *)(c + 8) >= 0x100000 ||
        *(int *)(c + 0xc) < -0xf0000 || *(int *)(c + 0xc) >= 0xc0000) {
        *(int *)LAUNDER(c + 0x11c) += 1;
    } else {
        if ((*(int *)(*(char **)(c + 4) + 8) & 0xff) != 0) {
            func_0203d6d0(&d1, (V2 *)(c + 8), (V2 *)(c + 0x114));
            t = func_0203d614(&d1) < 0x8000;
            if (t != 0) {
                *(int *)LAUNDER(c + 0x11c) += 1;
                goto after2;
            }
        }
        *(int *)(c + 0x11c) = 0;
        *(int *)(c + 0x114) = *(int *)(c + 8);
        *(int *)(c + 0x118) = *(int *)(c + 0xc);
    }
after2:
    if (*(int *)(c + 0xfc) > 0)
        *(int *)LAUNDER(c + 0xfc) -= 1;
    if (*(int *)(c + 0x34) > 0)
        *(int *)LAUNDER(c + 0x34) -= 1;
    if (*(u8 *)(c + 0x3a) != 1) {
        if (*(u8 *)(c + 0x38) == 0) {
            if (*(int *)(c + 8) >= 0xe8000 && *(int *)(c + 8) < 0xf0000 &&
                *(int *)(c + 0xc) >= 0x78000 && *(int *)(c + 0xc) < 0xa8000) {
                if (*(int *)(c + 0x34) <= 0) {
                    *(u8 *)(c + 0x38) = 1;
                    *(u8 *)(c + 0x39) = 0;
                    *(u8 *)(c + 0x3a) = 0;
                }
            }
        }
    }
    if (*(u8 *)(c + 0x3a) == 0) {
        pa.x = *(int *)(c + 8);
        pa.z = *(int *)(c + 0xc);
        pb.x = 0x8000;
        pb.z = 0x8c000;
        pc.x = 0x8000;
        pc.z = 0x92000;
        pd.x = 0xe0000;
        pd.z = 0x96000;
        if (func_ov006_02114590(c, &pa, &pb, &pc, &pd) != 0)
            goto bounce;
        pe.x = *(int *)(c + 8);
        pe.z = *(int *)(c + 0xc);
        pf.x = 0x8000;
        pf.z = 0x92000;
        pg.x = 0xe0000;
        pg.z = 0x96000;
        ph.x = 0xe0000;
        ph.z = 0x9e000;
        if (func_ov006_02114590(c, &pe, &pf, &pg, &ph) != 0) {
bounce:
            *(int *)LAUNDER(c + 0x20) += 0x80;
            if (*(int *)(c + 0x20) >= 0x2000)
                *(int *)(c + 0x20) = 0x2000;
            *(int *)LAUNDER(c + 0x24) += 0x80;
        } else {
            *(int *)LAUNDER(c + 0x24) += 0x100;
        }
    }
    if (*(int *)(c + 0x20) != 0 || *(int *)(c + 0x24) != 0) {
        spd = func_0203d614((V2 *)(c + 0x20));
        if (spd >= 0x8000)
            spd = 0x8000;
        if (func_0203d434((V2 *)(c + 0x20)) != 0)
            func_0203d630((V2 *)(c + 0x20), spd);
    }
    *(int *)LAUNDER(c + 8) += *(int *)(c + 0x20);
    *(int *)LAUNDER(c + 0xc) += *(int *)(c + 0x24);
    if (*(u8 *)(c + 0x100) == 1) {
        func_ov006_021128fc(c);
    } else {
        func_ov006_02112ad8(c);
        if (*(u8 *)(c + 0x129) == 0) {
            vc.x = 0x20000;
            vc.z = -0x8000;
            func_0203d480(&v5c, &vc);
            {
                int dz = *(int *)(c + 0xc) + 0x80000;
                int dx = *(int *)(c + 8) - 0xd8000;
                vd.x = dx;
                vd.z = dz;
            }
            if (func_0203d524(&vc, &vd) < -*(int *)(c + 0x28))
                *(u8 *)(c + 0x129) = 1;
        }
    }
    mag = func_0203d614((V2 *)(c + 0x20));
    func_0203d6d0(&d2, (V2 *)(c + 8), (V2 *)(c + 0x10));
    dist = func_0203d614(&d2);
    if (*(u8 *)(c + 0xcd) != 0)
        return;
    if (mag < dist)
        return;
    if (mag < 0x800)
        return;
    if (func_0203d434((V2 *)(c + 0x20)) == 0)
        return;
    func_0203d630((V2 *)(c + 0x20), dist);
}
