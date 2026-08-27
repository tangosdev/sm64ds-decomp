typedef struct { int x, y, z; } Vec3;
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
extern void Sound_PlayBank1Panned(int a0, char *a1, void *a2);
struct G { int w[2]; };
extern int data_ov006_02140408[];
extern struct G data_ov006_0213b028;

void func_ov006_020c7ba4(char *p) {
    Vec3 v;
    Vec3_Sub(&v, (Vec3 *)(p + 0x14), (Vec3 *)(p + 4));
    if (NormalizeVec3IfNonZero(&v) != 0) {
        *(int *)(p + 0x20) = v.x;
        *(int *)(p + 0x24) = v.y;
        *(int *)(p + 0x28) = v.z;
    } else {
        *(int *)(p + 0x20) = -*(int *)(p + 0x20);
        *(int *)(p + 0x24) = -*(int *)(p + 0x24);
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p + 0x4c, (void *)data_ov006_02140408[0], 0x40000000, 0x800, 0);
    *(int *)(p + 0xa4) = 0;
    Sound_PlayBank1Panned(0, (char *)6, *(void **)(p + 0x14));
    *(short *)(p + 0x32) = 0x20;
    *(struct G *)(p + 0x3c) = data_ov006_0213b028;
}
