typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int Fix12;

typedef struct { int x, y, z; } Vector3;

extern int func_ov063_0211a0dc(void* c);
extern void _Z14ApproachLinearRsss(s16* p, s16 a, s16 b);
extern int _ZN9Animation8FinishedEv(void* self);
extern int func_ov063_0211a8a4(char* thiz);
extern void func_02012694(int a, void* p);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int frame, Fix12 rate, unsigned int flags);
extern int RandomIntInternal(int* seed);
extern int _ZNK9Animation12WillHitFrameEi(void* self, int f);
extern char* _ZN5Actor13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(void* self, const Vector3* v, const void* p, Fix12 a, Fix12 b, unsigned int n);

extern s16 data_ov063_0211e1c0[];
extern int data_ov063_0211edcc;
extern int data_ov063_0211edd4;
extern int data_ov063_0211ede4;
extern int data_0209e650;
extern s16 data_02082214[];

void func_ov063_02118458(void* self)
{
    char* c = (char*)self;
    int r4 = func_ov063_0211a0dc(c);

    _Z14ApproachLinearRsss((s16*)(c + 0x94), *(s16*)(c + 0x5b0),
        data_ov063_0211e1c0[*(u8*)(c + 0x5ca) - 1]);

    if (_ZN9Animation8FinishedEv(c + 0x3d0)) {
        if (func_ov063_0211a8a4(c)) {
            if (*(int*)(c + 0x3e0) == *(int*)(&data_ov063_0211edcc + 1)) {
                func_02012694(0x158, c + 0x74);
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x380,
                    *(void**)(&data_ov063_0211edd4 + 1), 0x40000000, 0x1000, 0);
            } else if (*(u8*)(c + 0x5d2) != 0) {
                *(u8*)(((long long)(int)(c + 0x5d2)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
                *(int*)(c + 0x3d8) = 0;
                func_02012694(0x158, c + 0x74);
            } else {
                int rnd;
                *(u8*)(c + 0x5cc) = 1;
                *(int*)(((long long)(int)(c + 0x19c)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
                rnd = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x3f;
                *(u16*)(c + 0x5be) = rnd + 0xb4;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x380,
                    *(void**)(&data_ov063_0211ede4 + 1), 0, 0x1000, 0);
            }
        } else {
            int rnd;
            *(u8*)(c + 0x5cc) = 1;
            *(int*)(((long long)(int)(c + 0x19c)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
            rnd = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x3f;
            *(u16*)(c + 0x5be) = rnd + 0xb4;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x380,
                *(void**)(&data_ov063_0211ede4 + 1), 0, 0x1000, 0);
        }
    } else {
        if (*(int*)(c + 0x3e0) == *(int*)(&data_ov063_0211edd4 + 1)
            && _ZNK9Animation12WillHitFrameEi(c + 0x3d0, 6)) {
            Vector3 v;
            s16 m = 0x78;
            v.x = *(int*)(c + 0x5c);
            v.y = *(int*)(c + 0x60);
            v.z = *(int*)(c + 0x64);
            v.x = data_02082214[(*(u16*)(c + 0x8e) >> 4) * 2] * m + v.x;
            v.z = data_02082214[(*(u16*)(c + 0x8e) >> 4) * 2 + 1] * m + v.z;
            char* fb;
            *(s16*)(c + 0x8c) = 0x1000;
            func_02012694(0x156, c + 0x74);
            fb = _ZN5Actor13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
                c, &v, (const void*)(c + 0x8c), 0x14000, 0x6999, 4);
            *(s16*)(c + 0x8c) = 0;
            if (((unsigned int)*(u16*)(c + 0x5d4) << 23) >> 31)
                *(u8*)(fb + 0x36e) = 1;
            else
                *(u8*)(fb + 0x36e) = 0;
            *(int*)(fb + 0x364) = 0x3e8000;
            func_02012694(0x122, c + 0x74);
        }
    }

    if (r4 == -1) {
        *(u8*)(c + 0x5cc) = 2;
        return;
    }
    if (r4 != 1)
        return;
    *(u8*)(c + 0x5cc) = 3;
    func_02012694(0x152, c + 0x74);
}
