typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef long long s64;

extern void func_ov006_020c8270(char *c);
extern void func_ov006_020c7ba4(char *c);
extern void func_ov006_020c79a8(char *c);
extern void func_ov006_020c85a0(char *c);
extern void Sound_PlayBank1Panned(int a0, int a1, int a2);
extern void func_02012718(int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisPtr, void *file, int i, int fx, unsigned int flags);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _Z11UpdateAngleRssis(s16 *a, int b, int cc, int d);

extern int data_ov006_0213b008;
extern int data_ov006_0213b00c;
extern int data_ov006_0213b01c;
extern void *data_ov006_0213b098[];
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

void func_ov006_020c7c68(char *c)
{
    u16 st = *(u16 *)(c + 0x10);

    if (st == 1) {
        *(u16 *)(c + 0x10) = 0;
        *(int *)(c + 0x24) = data_ov006_0213b008;
        if (*(int *)(c + 0x44) != 0)
            *(int *)(c + 0x44) = 0;
        else
            *(int *)(c + 0x44) = 1;
        func_ov006_020c8270(c);
        return;
    }
    if (st == 2) {
        *(u16 *)(c + 0x10) = 0;
        func_ov006_020c7ba4(c);
        return;
    }

    {
        int flag = 0;
        u8 idx = data_020a0e40;
        if (data_020a0de8[idx * 4] != 0 && data_020a0de9[idx * 4] != 0)
            flag = 1;

        if (flag != 0) {
            s16 y = *(s16 *)(c + 0x38);
            int ax = (int)data_020a0dea[idx * 4];
            int az = (int)data_020a0deb[idx * 4];
            int dx = (int)*(s16 *)(c + 0x36) - ax;
            int ym = (int)y - 0x20;
            int t = dx < 0 ? -dx : dx;

            if (t < 0x18) {
                t = ym - az;
                if (t < 0) t = -t;
                if (t < 0x26 && (int)y < 0xbc) {
                    int v[3];
                    int *p = &data_ov006_0213b00c;
                    int t0 = (ax - 0x80) << 12;
                    int t1 = (-az) << 12;
                    v[0] = t0;
                    v[1] = t1;
                    *(volatile int *)&v[2] = 0;
                    *(int *)(c + 0x24) = *p;
                    *(int *)(c + 0x20) = data_ov006_0213b01c * dx;
                    if (*(int *)(c + 0x44) != 0)
                        *(int *)(c + 0x44) = 0;
                    else
                        *(int *)(c + 0x44) = 1;
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, *(void **)data_ov006_0213b098[*(int *)(c + 0x44)], 0x40000000, 0x800, 0);
                    *(int *)(c + 0xa4) = 0;
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xed, *(int *)(c + 0x14) << 3, *(int *)(c + 0x18) << 3, *(int *)(c + 0x1c) << 3);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xee, v[0] << 3, v[1] << 3, v[2] << 3);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xef, v[0] << 3, v[1] << 3, v[2] << 3);
                    Sound_PlayBank1Panned(0, *(int *)(c + 0x44), *(int *)(c + 0x14));
                    func_02012718(0x1c6, *(s16 *)(c + 0x36) << 12);
                    func_ov006_020c85a0(c);
                }
            }
        }
    }

    if (*(s16 *)(c + 0x38) > 0xbc) {
        func_ov006_020c79a8(c);
        return;
    }

    {
        int pos = *(int *)(c + 0x14);
        if (pos < -0x6c000 && *(int *)(c + 0x20) < 0) {
            *(int *)(c + 0x20) = -(int)(((s64)*(int *)(c + 0x20) * 0xd00 + 0x800) >> 12);
            if (*(int *)(c + 0x24) > 0) {
                *(int *)(c + 0x44) = 2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, *(void **)data_ov006_0213b098[*(int *)(c + 0x44)], 0x40000000, 0x800, 0);
                *(int *)(c + 0xa4) = 0;
                Sound_PlayBank1Panned(0, *(int *)(c + 0x44), *(int *)(c + 0x14));
            }
        } else if (pos > 0x6c000 && *(int *)(c + 0x20) > 0) {
            *(int *)(c + 0x20) = -(int)(((s64)*(int *)(c + 0x20) * 0xd00 + 0x800) >> 12);
            if (*(int *)(c + 0x24) > 0) {
                *(int *)(c + 0x44) = 2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, *(void **)data_ov006_0213b098[*(int *)(c + 0x44)], 0x40000000, 0x800, 0);
                *(int *)(c + 0xa4) = 0;
                Sound_PlayBank1Panned(0, *(int *)(c + 0x44), *(int *)(c + 0x14));
            }
        }
    }

    if (*(int *)(c + 0x20) > 0) {
        _Z11UpdateAngleRssis((s16 *)(c + 0x2e), 0x2800, 2, 0x1000);
        return;
    }
    _Z11UpdateAngleRssis((s16 *)(c + 0x2e), -0x2800, 2, 0x1000);
}
