//cpp
struct Camera;

extern "C" int func_ov100_02144fcc(void);
extern "C" int _Z14ApproachLinearRiii(int &x, int a, int b);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);
extern "C" void func_02012694(int a, void *b, int c);
extern "C" void _ZN6Camera14GoBehindPlayerEj(Camera *self, unsigned int a);
extern "C" void func_ov100_02145f68(void *c, void *p, int a2);
extern "C" void ChangeArea(int areaID);

struct V3 { int x, y, z; };
extern "C" void Vec3_RotateYAndTranslate(V3 *out, void *m, short angle, V3 *in);

extern unsigned char data_0209f250;
extern Camera *data_0209f318;
extern char data_ov100_02148974;
extern char data_020a0ebc;
extern V3 data_ov100_02148948;

extern "C" int func_ov100_02145988(char *self, char *arg1)
{
    int r5;
    V3 vec;
    int gy, gz, vd;

    r5 = func_ov100_02144fcc();
    if (_Z14ApproachLinearRiii(*(int *)(self + 0x98), *(int *)(self + 0xa0), 0x8000) != 0) {
        if (*(int *)(self + 0xa0) != 0) {
            if (*(unsigned char *)(self + 0x114) == 0) {
                *(unsigned char *)(self + 0x114) = 0x1c;
            } else if (DecIfAbove0_Byte((unsigned char *)(self + 0x114)) == 0) {
                *(int *)(self + 0xa0) = 0;
                func_02012694(0x4f, self + 0x74, 0);
            } else if (*(unsigned char *)(self + 0x114) == 8) {
                _ZN6Camera14GoBehindPlayerEj(data_0209f318, data_0209f250);
            }
        } else if (r5 != 0) {
            func_ov100_02145f68(self, &data_ov100_02148974, (int)arg1);
            {
                int t;
                if (*(int *)(self + 0x88) < 0)
                    t = *(short *)(self + 0x8c);
                else
                    t = *(short *)(self + 0x90);
                t = (signed char)t;
                *(signed char *)(arg1 + 0xcc) = t;
                ChangeArea(t);
            }
        }
    }

    vd = *(int *)(self + 0x98);
    gy = data_ov100_02148948.y;
    gz = data_ov100_02148948.z;
    data_ov100_02148948.x = vd;
    vec.x = vd;
    vec.z = gz;
    vec.y = gy;
    Vec3_RotateYAndTranslate((V3 *)(self + 0xa4), &data_020a0ebc, *(short *)(self + 0x8e), &vec);
    return 1;
}
