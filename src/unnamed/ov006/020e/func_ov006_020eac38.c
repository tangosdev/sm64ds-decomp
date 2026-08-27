typedef struct { int x, y; } Vec2;

extern short data_02082214[];
extern int data_ov006_0213c958;
extern void* data_ov006_02142018[];
extern void* data_ov006_021375c0[];
extern int data_ov006_0213c97c[];
extern void* data_ov006_021375a0[];

extern void func_0203d388(int* p, int angle);
extern void func_0203d680(Vec2* out, const Vec2* in, int scale);
extern int func_ov006_020ebb40(char* o, int i);
extern int func_ov006_020ebc08(char* c, int n);
extern int func_02053200(int x);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int b, void* attr, int x, int y, int a, int c, int f, int g);

#pragma opt_strength_reduction off
void func_ov006_020eac38(char* o)
{
    struct { int A, B, urot, va0x, res; } t;
    Vec2 vb0;
    struct { Vec2 vc0; } s3;
    struct { Vec2 pos, va, vb, vc, vout; } s;
    int px, py;
    int a2i;
    int i;
    short rot, ang;

    i = 0;
    vb0.y = -0x18000;
    s3.vc0.y = -0x14000;
    t.A = 0;
    t.B = 0;
    vb0.x = 0x2000;
    s3.vc0.x = 0;
    t.va0x = 0;
    for (; i < 5; i++) {
        rot = ((short*)(o + 0x7a))[i];
        s.pos.x = *(int*)(o + i*8 + 0x18);
        s.pos.y = *(int*)(o + i*8 + 0x1c);
        s.va.x = t.va0x;
        s.va.y = 0x1000;
        ang = (short)(*(short*)(o + 0x84) - t.A);
        func_0203d388(&s.va.x, rot);

        if (*(unsigned char*)(o + 0x94) == 0) {
            if (i == 0) {
                rot = (short)(rot + (data_02082214[((unsigned short)ang >> 4) * 2] >> 1));
            } else {
                int sinv = data_02082214[((unsigned short)ang >> 4) * 2];
                rot = (short)(rot + (short)(((long long)sinv * (0x1800 - t.B) + 0x800) >> 12));
            }
        }

        if (data_ov006_0213c958 >= 6) {
            rot = (short)(rot & 0xfc00);
        }
        func_0203d680(&s.vout, &s.va, data_02082214[((unsigned short)(ang << 1) >> 4) * 2]);
        s.pos.x += s.vout.x;
        s.pos.y += s.vout.y;

        t.res = func_ov006_020ebb40(o, i);
        px = s.pos.y >> 12;
        py = s.pos.x >> 12;
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
            1,
            data_ov006_02142018[(a2i = func_ov006_020ebc08(o, i), (t.urot = (unsigned short)rot), a2i)],
            py, px,
            t.res, -1, 0x1000,
            t.urot);

        if (i == 0) {
            if (*(unsigned char*)(o + 0x94) != 0 && *(int*)(o + 0x48) > 0) {
                int fp2;
                int scale;
                fp2 = (0x3000 - *(int*)(o + 0x48)) >> 12;
                s.vb.x = vb0.x;
                s.vb.y = vb0.y;
                func_0203d388(&s.vb.x, rot);
                s.vb.x += s.pos.x;
                s.vb.y += s.pos.y;
                if (fp2 != 0)
                    scale = 0x1000;
                else
                    scale = func_02053200((data_02082214[((unsigned short)ang >> 4) * 2] >> 2) + 0xc00);
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                    1, data_ov006_021375c0[fp2],
                    s.vb.x >> 12, s.vb.y >> 12,
                    -1, -1, scale, t.urot);
            } else if (*(int*)o == data_ov006_0213c97c[0]) {
                if ((*(int*)(o + 4) == data_ov006_0213c97c[1] || *(int*)o == 0) &&
                    *(short*)(o + 0x90) > 0x1a && *(short*)(o + 0x90) < 0x30) {
                    int idx = s3.vc0.x;
                    s.vc.x = s3.vc0.x;
                    s.vc.y = s3.vc0.y;
                    if (*(short*)(o + 0x90) < 0x26) {
                        int d = (0x30 - *(short*)(o + 0x90)) * 0x600;
                        int vy;
                        s.vc.y = (vy = *(volatile int*)((char*)&s - 4), (idx = 1) ? vy : vy) - d;
                    }
                    func_0203d388(&s.vc.x, rot);
                    s.vc.x += s.pos.x;
                    s.vc.y += s.pos.y;
                    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                        1, data_ov006_021375a0[idx],
                        s.vc.x >> 12, s.vc.y >> 12,
                        -1, -1, 0x1000, (unsigned short)(short)(rot & 0xf800));
                }
            }
        }

        t.A += 0x1800;
        t.B += 0x200;
    }
}
