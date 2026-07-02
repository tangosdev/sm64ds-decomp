//cpp
extern "C" {
struct Vec3 { int x, y, z; };
extern int data_ov002_02110a48;
extern int data_0209f318;
extern int data_0209f43c;
extern int data_0209b3ec;
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void *a, void *b, void *c, int d, void *e);
extern void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned int op, int z);
}

struct ModelBase {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int arg);
};

extern "C" int func_ov002_020ec0a4(char *self)
{
    char *base = (char *)data_0209f318;
    int *iter = &data_ov002_02110a48;
    char *sl = self + 0xd4;
    int i = 0;
    int z5 = 0;
    int z4 = 0;
    int c1f = 0x1f;
    int c1e000 = 0x1e000;
    Vec3 buf;
    int *sb;
    char *r8;
    char *r0;

    do {
        r8 = sl + 0x1c;
        Matrix4x3_FromRotationY(r8, *(short *)(base + 0x17c));
        sb = (int *)*iter;
        while (sb != 0) {
            int v = _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
                        &data_0209f43c, &data_0209b3ec, sb, c1e000, &buf);
            if (v > 0x11000 && v < 0x578000) {
                int op = c1f;
                if (v < 0x2f000) {
                    op = ((v - 0x10000) >> 12) & 0xff;
                }
                _ZN9ModelBase12ApplyOpacityEj(sl, op, z5);
                r0 = sl;
                *(int *)(r8 + 0x24) = sb[0];
                *(int *)(r8 + 0x28) = sb[1] - 0x1e000;
                *(int *)(r8 + 0x2c) = sb[2];
                ((ModelBase *)r0)->m(z4);
            }
            sb = (int *)sb[0x12];
        }
        sl += 0x50;
        i++;
        iter++;
    } while (i < 5);
    return 1;
}