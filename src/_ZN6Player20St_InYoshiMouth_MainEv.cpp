//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct Vector3 { int x, y, z; } Vector3;

extern "C" {
extern int func_ov002_020d5cec(char *c);
extern int func_ov002_020beb38(char *c);
extern void Vec3_RotateYAndTranslate(Vector3 *res, Vector3 *trans, short angY, Vector3 *add);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *c, void *s);
extern int _ZN6Player12FinishedAnimEv(char *c);
extern void func_ov002_020bedd4(char *c);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *thiz, void *v, u32 a, int b, u32 cc, u32 d, u32 e);

extern int data_ov002_0211019c[];
extern int data_ov002_02110094[];
extern int data_ov002_0211007c[];
}

extern "C" int _ZN6Player20St_InYoshiMouth_MainEv(char *c)
{
    switch (*(u8 *)(c + 0x6e3)) {
    case 0:
        break;
    case 1:
        {
            char *r4 = *(char **)(c + 0xd0);
            u32 t;
            int d;
            if (r4 != 0 && *(char **)(r4 + 0x360) != c) {
                if (func_ov002_020d5cec(c))
                    return 1;
            }
            {
                int *p = (int *)(int)(((long long)(int)(r4 + 0x5c)) & 0xffffffffffffffffLL);
                *(int *)(c + 0x5c) = p[0];
                *(int *)(c + 0x60) = p[1];
                *(int *)(c + 0x64) = p[2];
            }
            d = func_ov002_020beb38(c);
            t = *(u16 *)(r4 + 0x6c6);
            if (*(u8 *)(r4 + 0x709) == 0 && *(u8 *)(r4 + 0x708) == 0) {
                t = (u16)(t - d);
                *(u8 *)(((long long)(int)(c + 0x6e6)) & 0xffffffffffffffffLL) += d;
                if ((s16)t < 0)
                    t = 0;
            }
            if (t <= 1 && *(u8 *)(c + 0x6e6) >= 0x1e) {
                Vector3 res;
                Vector3 addv;
                Vector3 hv;
                addv.x = 0;
                addv.y = 0x64000;
                addv.z = 0x64000;
                Vec3_RotateYAndTranslate(&res, (Vector3 *)(r4 + 0x5c), *(s16 *)(r4 + 0x8e), &addv);
                hv.x = res.x;
                hv.y = res.y;
                hv.z = res.z;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &hv, 1, 0, 0, (*(int *)(r4 + 8) + 1) & 0xff, 1);
                _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211019c);
                *(int *)(r4 + 0x360) = 0;
                *(int *)(c + 0xd0) = 0;
                return 1;
            }
            *(u16 *)(r4 + 0x6c6) = (s16)t;
            break;
        }
    case 2:
        if (*(u16 *)(c + 0x6a4) == 0)
            *(int *)(((long long)(int)(c + 0x2ec)) & 0xffffffffffffffffLL) |= 0x2000;
        if (_ZN6Player12FinishedAnimEv(c)) {
            *(u8 *)(c + 0x6e3) = 3;
            *(int *)(c + 0x674) = 1;
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110094);
        }
        break;
    case 3:
        {
            int d = func_ov002_020beb38(c);
            *(u16 *)(((long long)(int)(c + 0x6a4)) & 0xffffffffffffffffLL) -= (d << 2);
            *(u8 *)(c + 0x6e5) = d;
            if ((s16)*(u16 *)(c + 0x6a4) <= 0) {
                *(u16 *)(c + 0x6a4) = 0;
                *(u8 *)(c + 0x6f5) = 0x1f;
                _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211007c);
            }
        }
        break;
    }
    func_ov002_020bedd4(c);
    return 1;
}
