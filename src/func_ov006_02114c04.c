typedef int Fix12i;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct Vector3 { Fix12i x, y, z; } Vector3;

extern Fix12i func_0203d614(const Vector3* v);
extern int func_0203d434(Fix12i* in);
extern void func_0203d630(int *p, int m);
extern int RandomIntInternal(int* seed);
extern void func_0203d388(int *p, int angle);

extern int data_0209d4b8;

#define L(a) (*(int*)(((long long)(int)(a)) & 0xFFFFFFFFFFFFFFFFLL))
#define M(a) (*(int*)(((long long)(unsigned)(a)) & 0xFFFFFFFFFFFFFFFFLL))
#define N(a) (*(int*)(((long long)(long)(a)) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov006_02114c04(char* o)
{
    int i;
    char* b;
    char* v;
    char* p;
    Fix12i d;

    b = o;
    v = o + 0x59ac;
    p = o + 0x59a4;
    i = 0;
    do {
        if (*(unsigned char*)(b + 0x59bc)) {
            L(b + 0x59a4) += *(int*)(b + 0x59ac);
            L(b + 0x59a8) += *(int*)(b + 0x59b0);
            d = func_0203d614((const Vector3*)v) * 7 / 8;
            if (func_0203d434((Fix12i*)v))
                func_0203d630((int*)v, d);
            N(b + 0x59b4)++;
            if (*(int*)(b + 0x59b4) >= 0x10) {
                *(int*)(b + 0x59b4) = 0;
                N(b + 0x59b8)--;
                if (*(int*)(b + 0x59b8) <= 0) {
                    *(unsigned char*)(b + 0x59bc) = 0;
                } else {
                    d = RandomIntInternal(&data_0209d4b8);
                    *(int*)(b + 0x59a4) = 0;
                    *(int*)(b + 0x59a8) = (u16)((u32)d >> 16 & 0x7fff) * 2;
                    func_0203d388((int*)p, (short)(u16)(((u32)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 2));
                    M(b + 0x59a8) *= 2;
                    M(b + 0x59a4) += *(int*)(b + 0x599c);
                    M(b + 0x59a8) += *(int*)(b + 0x59a0);
                    *(int*)(b + 0x59ac) = 0;
                    *(int*)(b + 0x59b0) = 0x10;
                }
            }
        }
        b += 0x24;
        v += 0x24;
        p += 0x24;
        i++;
    } while (i < 0x40);
}
