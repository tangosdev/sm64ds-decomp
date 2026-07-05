typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

extern s16 data_02082214[];
extern int* data_ov079_021275ec[];

extern s16 Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
extern int AngleDiff(int a, int b);
extern void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(char* self, const struct Vector3* pos, u32 n, int fix, s16 s);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* self, void* bca, int n, int fix, u32 flags);

void func_ov079_02123804(char* self, char* other)
{
    struct Vector3 v;
    int idx;
    s16 cosv, sinv;
    int ang;

    {
        int* q = (int*)(((int)other + 0x68) & 0xFFFFFFFFFFFFFFFFLL);
        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
    }

    idx = *(u16*)(other + 0x94) >> 4;
    cosv = data_02082214[idx * 2];
    sinv = data_02082214[idx * 2 + 1];
    v.x = v.x - cosv * 0x50;
    v.z = v.z - sinv * 0x50;

    ang = Vec3_HorzAngle((struct Vector3*)(self + 0x5c), &v);

    if (AngleDiff(ang, (s16)(*(s16*)(self + 0x8e) + *(s16*)(self + 0x3e2))) < 0x3c00) {
        int r1 = *(int*)(self + 0x3b0);
        if (r1 == 1 || r1 == 2 || r1 == 7 || r1 == 0xa) goto action1;
        if (r1 != 0) return;
        if (*(u8*)(self + 0x414) != 0) {
            if (*(u8*)(self + 0x40c) != 0) return;
        }
    action1:
        if (*(int*)(self + 0x3b0) != 0xa) {
            *(int*)(self + 0x3b4) = *(volatile int*)(self + 0x3b0);
            *(s16*)(self + 0x3fe) = *(u16*)(self + 0x100);
            *(u8*)(self + 0x40d) = *(u8*)(self + 0x40c);
        }
        *(u8*)(self + 0x400) = 0x5a;
        *(int*)(self + 0x98) = 0;
        *(int*)(self + 0x3b0) = 0xa;
        *(u8*)(self + 0x40b) = 1;
        return;
    }

    if (AngleDiff(ang, (s16)(*(s16*)(self + 0x8e) + *(s16*)(self + 0x3e2))) <= 0x4400) return;

    {
        int r1 = *(int*)(self + 0x3b0);
        if (r1 == 1 || r1 == 2 || r1 == 7 || r1 == 0xa) goto action2;
        if (r1 != 0) return;
        if (*(u8*)(self + 0x414) != 0) {
            if (*(u8*)(self + 0x40c) != 0) return;
        }
    action2:
        if (*(int*)(self + 0x3b0) != 0xa) {
            *(int*)(self + 0x3b4) = *(volatile int*)(self + 0x3b0);
            *(s16*)(self + 0x3fe) = *(u16*)(self + 0x100);
            *(u8*)(self + 0x40d) = *(u8*)(self + 0x40c);
        }
        {
            struct Vector3 pos;
            int* q = (int*)(((int)other + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
            pos.x = q[0];
            pos.y = q[1];
            pos.z = q[2];
            _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &pos, 1, 0, 0);
        }
        *(int*)(self + 0x98) = 0;
        {
            int* bca = data_ov079_021275ec[*(u8*)(self + 0x414) * 5];
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x2cc, (void*)bca[1], 0x40000000, 0x1000, 0);
        }
        *(int*)(self + 0x328) = 0x4000;
        *(int*)(self + 0x3b0) = 0xb;
        *(u8*)(self + 0x40b) = 1;
        return;
    }
}
