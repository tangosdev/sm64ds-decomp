extern unsigned short data_0209b274;
extern void *data_0209f318;
extern int data_0208ee44;
extern int data_ov002_0210b970[3];
extern unsigned char data_0209f250;
extern void *data_0209f394[];
extern short data_02082214[];

extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern void Math_Function_0203b0fc(int *p, int target, int scale, int max);
extern short Vec3_HorzAngle(const int *v0, const int *v1);
extern void _Z15ApproachLinear2Rsss(short *v, short t, short s);
extern void _Z11UpdateAngleRssis(void *p, short a, int step, short d);
extern void _Z14ApproachLinearRsss(short *dst, short target, short step);

int func_ov002_020f7780(char *self, void *unused, int mode)
{
    char *c = self;
    char *r4 = (char *)data_0209f318;

    if (mode == data_0209b274) {
        if (mode == 0) {
            data_0208ee44 = 3;
            return 1;
        }

        {
            int v[3];
            unsigned char idx;
            v[0] = *(int *)((char *)data_ov002_0210b970 + 0);
            v[1] = *(int *)((char *)data_ov002_0210b970 + 4);
            v[2] = *(int *)((char *)data_ov002_0210b970 + 8);

            idx = *(volatile unsigned char *)&data_0209f250;
            {
                char *p = (char *)(data_0209f394);
                short ang = *(short *)(*(void **)(p + (idx << 2)) + 0x8e);

                Vec3_RotateYAndTranslate((int *)(c + 0x5c), (int *)(r4 + 0x8c), (short)(ang + 0x8000), v);
            }
        }

        *(short *)(c + 0x92) = 0x1000;
        *(short *)(c + 0x94) = -0x7000;
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        *(unsigned char *)(c + 0x102) = 0x1f;
    } else {
        short state = *(short *)(c + 0xfc);
        if (state <= 0x6a) {
            if (state > 0x3c) {
                Math_Function_0203b0fc((int *)(c + 0xf4), -0xa000, 0xcc, 0x7fffffff);
                short *p94 = (short *)(c + 0x94);
                short *p92 = (short *)(c + 0x92);
                *p94 = *p94 + 0x78;
                *p92 = *p92 + 0x40;

                short h = Vec3_HorzAngle((int *)(c + 0x5c), (int *)(r4 + 0x8c));
                _Z15ApproachLinear2Rsss((short *)(c + 0x8e), h, 0x200);
            }

            state = *(short *)(c + 0xfc);
            if (state > 0x69) {
                *(short *)(c + 0x92) = 0xe00;
            }
        } else {
            Math_Function_0203b0fc((int *)(c + 0xf4), 0x3c000, 0xcc, 0x7fffffff);

            short h = Vec3_HorzAngle((int *)(c + 0x5c), (int *)(r4 + 0x8c));
            _Z15ApproachLinear2Rsss((short *)(c + 0x8e), h, 0x200);

            state = *(short *)(c + 0xfc);
            if (state < 0xa9) {
                _Z11UpdateAngleRssis((void *)(c + 0x94), 0x1800, 0x1e, 0x4000);
            }
            _Z14ApproachLinearRsss((short *)(c + 0x92), -0x3000, 0x70);
        }
    }

    {
        unsigned short a = *(unsigned short *)(c + 0x92);
        int t = *(int *)(c + 0xf4);
        short s0 = data_02082214[((a >> 4) << 1) + 1];
        *(int *)(c + 0x98) = (int)(((long long)t * s0 + 0x800) >> 12);
    }
    {
        unsigned short a = *(unsigned short *)(c + 0x92);
        int t = *(int *)(c + 0xf4);
        int s1 = -data_02082214[(a >> 4) << 1];
        *(int *)(c + 0xa8) = (int)(((long long)t * s1 + 0x800) >> 12);
    }

    {
        short *pfc = (short *)(c + 0xfc);
        *pfc = *pfc + 1;
    }

    return 1;
}
