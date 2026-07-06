typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

extern int _ZN5Actor13ClosestPlayerEv(void* thiz);
extern int _ZN6Player9StartTalkER9ActorBaseb(void* thiz, void* ab, int b);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* thiz, void* ab, u32 a, const void* v, u32 c, u32 d);
extern int func_02012694(int a, void* b);
extern int func_ov094_02136188(void* c, void* p);
extern int Vec3_Dist(const void* a, const void* b);
extern s16 Vec3_HorzAngle(const void* a, const void* b);
extern int RandomIntInternal(int* seed);
extern int ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
extern void _Z14ApproachLinearRiii(int* x, int target, int step);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(void* dst, void* mat, void* src);

extern short data_02082214[];
extern int data_0209e650[];
extern char data_ov094_02136b50[];
extern int data_020a0e68[];

struct V6 { int v[6]; };

int func_ov094_02135c28(void* thiz)
{
    char* c = (char*)thiz;
    int result;
    int ang;
    int* p3e8;
    short tbl;
    int idx;
    struct V6 buf;

    if (*(u8*)(c + 0x3d4) == 0) {
        if (_ZN5Actor13ClosestPlayerEv(c) != 0) {
            void* player = *(void**)(c + 0x3d0);
            if (player != 0) {
                if ((u16)(*(u16*)((char*)player + 0x6ce) & 0x800) == 0) {
                    if (_ZN6Player9StartTalkER9ActorBaseb(player, c, 1) != 0) {
                        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(void**)(c + 0x3d0), c, 0xa2, (const void*)(c + 0x5c), 0, 0) == 1) {
                            func_02012694(0x176, (void*)(c + 0x74));
                            *(u8*)(c + 0x3d4) = 1;
                            func_ov094_02136188(c, data_ov094_02136b50);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    if (Vec3_Dist((const void*)(c + 0x5c), (const void*)(c + 0x3d8)) > 0x190000) {
        *(s16*)(c + 0x100) = 0x32;
        *(s16*)(c + 0x3ec) = Vec3_HorzAngle((const void*)(c + 0x5c), (const void*)(c + 0x3d8));
    } else if (*(u16*)(c + 0x100) == 0) {
        *(s16*)(c + 0x3ec) = (s16)(((u32)RandomIntInternal(data_0209e650) >> 8) << 12);
        *(s16*)(c + 0x100) = (s16)((((u32)RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0x32);
    }

    ApproachAngle((short*)(c + 0x94), *(s16*)(c + 0x3ec), 0xa, 0x200, 0x100);
    ApproachAngle((short*)(c + 0x92), 0, 0xa, 0x200, 0x100);

    buf.v[0] = 0;
    buf.v[1] = 0;
    buf.v[2] = 0x5000;
    buf.v[3] = 0;
    buf.v[4] = 0;
    buf.v[5] = 0;

    p3e8 = (int*)(((int)c + 0x3e8) & 0xFFFFFFFFFFFFFFFF);
    *p3e8 += 0x200;
    ang = *(int*)(c + 0x3e8);
    idx = ((u16)(short)ang >> 4) * 2;
    tbl = data_02082214[idx];
    result = (int)(((long long)tbl * 0x64000 + 0x800) >> 12);
    _Z14ApproachLinearRiii((int*)(c + 0x60), *(int*)(c + 0x3dc) + result, 0x3000);

    Matrix4x3_FromRotationY(data_020a0e68, *(s16*)(c + 0x94));
    MulVec3Mat4x3(&buf, data_020a0e68, (void*)(c + 0xa4));
    return 1;
}
