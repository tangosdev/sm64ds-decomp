typedef int Fix12i;
typedef short s16;

struct Vec { Fix12i x, y, z; };

extern int RandomIntInternal(int* seed);
extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern s16 Vec3_HorzAngle(struct Vec* v0, struct Vec* v1);
extern int _Z14ApproachLinearRsss(s16* a, s16 b, s16 c);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int b, int fix, unsigned int e);
extern void Vec3_Sub(struct Vec* out, struct Vec* a, struct Vec* b);
extern int Vec3_HorzLen(struct Vec* v);

extern int RNG_STATE;
extern void* data_ov100_021489cc[];
extern void** data_ov100_021473b0[];

void func_ov100_02146640(char* c)
{
    struct Vec d;
    int st = *(int*)(c + 0x150);
    if (st == 0) {
        unsigned r = RandomIntInternal(&RNG_STATE);
        *(int*)(c + 0x98) = ((r >> 15) & 0x1fff) + 0x3000;
        r = RandomIntInternal(&RNG_STATE);
        *(int*)(c + 0x148) = ((r % 500) + 0x96) << 0xc;
        *(int*)(c + 0x130) = 0x2000;
    } else if (st == 0xa) {
        *(int*)(c + 0x130) = 0x1000;
    }

    char* p = (char*)_ZN5Actor13ClosestPlayerEv(c);
    if (p) {
        s16 ang = Vec3_HorzAngle((struct Vec*)(c + 0x5c), (struct Vec*)(p + 0x5c));
        if (_Z14ApproachLinearRsss((s16*)(c + 0x94), ang, 0x400) != 0) {
            if (*(unsigned char*)(c + 0x15b) == 0) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    c + 0xd4, data_ov100_021489cc[1], 0, 0x1000, 0);
            }
        }
        Vec3_Sub(&d, (struct Vec*)(c + 0x5c), (struct Vec*)(p + 0x5c));
        if (Vec3_HorzLen(&d) >= *(int*)(c + 0x148))
            return;
        *(int*)(c + 0x150) = -1;
        *(int*)(c + 0x14c) = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            c + 0xd4, data_ov100_021473b0[*(unsigned char*)(c + 0x15b)][1], 0, 0x1000, 0);
    } else {
        *(int*)(c + 0x150) = -1;
        *(int*)(c + 0x14c) = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            c + 0xd4, data_ov100_021473b0[*(unsigned char*)(c + 0x15b)][1], 0, 0x1000, 0);
    }
}