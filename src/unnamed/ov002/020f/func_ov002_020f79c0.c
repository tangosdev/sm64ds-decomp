extern unsigned short data_0209b274;
extern int data_ov002_0210b964[3];
extern int data_ov002_02110ddc[3];
extern short data_02082214[];

extern void Math_Function_0203b0fc(int *p, int target, int scale, int max);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);
extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);

int func_ov002_020f79c0(char *self, void *unused, int mode)
{
    char *c = self;

    if (mode == data_0209b274) {
        *(int *)(c + 0xf4) = 0x578000;
        *(short *)(c + 0xfc) = -0x1000;
        *(short *)(c + 0xfe) = 0x800;
        *(int *)(c + 0xf8) = -0xc8000;
        *(short *)(c + 0x94) = -0x8000;
        *(short *)(c + 0x8e) = -0x4000;
        *(short *)(c + 0x92) = 0x800;
        *(unsigned char *)(c + 0x102) = 0x1f;
    } else {
        int v[3];
        int w[3];
        int h;
        int t;

        {
            short *p94 = (short *)(c + 0x94);
            *p94 = *p94 + 0x200;
        }
        Math_Function_0203b0fc((int *)(c + 0xf4), 0x64000, 0x7a, 0x7fffffff);

        *(short *)(c + 0x8c) = _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(c + 0x60) - 0x190000, 0xc8000);

        ApproachAngle((short *)(c + 0x8e), (short)(*(short *)(c + 0x94) + 0x8000), 4, 0x4000, 0);

        data_ov002_02110ddc[2] = *(int *)(c + 0xf4);
        v[0] = data_ov002_0210b964[0];
        v[1] = data_ov002_0210b964[1];
        v[2] = data_ov002_0210b964[2];
        w[0] = data_ov002_02110ddc[0];
        w[1] = data_ov002_02110ddc[1];
        w[2] = data_ov002_02110ddc[2];
        Vec3_RotateYAndTranslate((int *)(c + 0x5c), v, *(short *)(c + 0x94), w);

        h = *(unsigned short *)(c + 0xfc);
        t = data_02082214[(h >> 4) * 2 + 1];
        {
            int *p60 = (int *)(c + 0x60);
            *p60 = *p60 + (int)((((long long)t * 0x96000LL) + 0x800) >> 12);
        }

        {
            short *pfc = (short *)(c + 0xfc);
            *pfc = *pfc + *(short *)(c + 0xfe);
        }
        ApproachAngle((short *)(c + 0xfe), 0x200, 0x14, 0x4000, 0);

        {
            int *p5c = (int *)(c + 0x5c);
            *p5c = *p5c + *(int *)(c + 0xf8);
        }
        Math_Function_0203b0fc((int *)(c + 0xf8), 0, 0xcc, 0x7fffffff);
    }
    return 1;
}
