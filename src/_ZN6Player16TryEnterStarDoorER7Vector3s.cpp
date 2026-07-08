//cpp
extern "C" {
struct State;
struct Vector3;
extern State _ZN6Player7ST_WALKE;
extern State data_ov002_02110154;
extern State _ZN6Player10ST_ON_WALLE;
extern int data_ov002_0210e164;
extern int data_ov002_0210f344[3];
extern int data_ov002_0210f308;
extern void func_020072c0(void);

extern int _ZN6Player7IsStateERNS_5StateE(void *c, State *st);
extern int _ZN6Player17SetNoControlStateEhih(void *c, unsigned char a, int b, unsigned char d);
extern void func_020731dc(int a, int b, void **node);
extern void Vec3_RotateYAndTranslate(void *out, void *trans, short angle, int *vec);
extern short Vec3_HorzAngle(const void *v0, const void *v1);

int _ZN6Player16TryEnterStarDoorER7Vector3s(char *c, int *pos, short kind)
{
    if (_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player7ST_WALKE) ||
        _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110154) ||
        _ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player10ST_ON_WALLE)) {
        if (_ZN6Player17SetNoControlStateEhih(c, 0xa, -1, 0)) {
            *(int *)(c + 0x750) = pos[0];
            *(int *)(c + 0x754) = pos[1];
            *(int *)(c + 0x758) = pos[2];
            *(short *)(c + 0x69e) = kind;
            *(unsigned char *)(c + 0x70c) = 0;
            if (!(data_ov002_0210e164 & 1)) {
                data_ov002_0210f344[0] = 0;
                data_ov002_0210f344[1] = 0;
                data_ov002_0210f344[2] = 0x64000;
                func_020731dc((int)data_ov002_0210f344, (int)func_020072c0, (void **)&data_ov002_0210f308);
                data_ov002_0210e164 |= 1;
            }
            Vec3_RotateYAndTranslate(c + 0x744, c + 0x750, *(short *)(c + 0x69e), data_ov002_0210f344);
            *(short *)(c + 0x8e) = Vec3_HorzAngle(pos, c + 0x5c) + 0x8000;
            return 1;
        }
    }
    return 0;
}
}
