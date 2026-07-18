//cpp
struct Vector3 { int x, y, z; };

extern "C" void *_ZN5Actor13ClosestPlayerEv(void *thiz);
extern "C" int Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern "C" short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
extern "C" int AngleDiff(int a, int b);
extern "C" void func_ov102_0214b384(void *thiz, int a);

extern "C" void func_ov102_0214b988(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *pl;
    struct Vector3 v;

    *(int *)(c + 0x38c) = 0;
    pl = (unsigned char *)_ZN5Actor13ClosestPlayerEv(c);
    if (!pl) return;
    if (Vec3_Dist((struct Vector3 *)(c + 0x5c), (struct Vector3 *)(pl + 0x5c)) > 0x190000) return;
    {
        int *src = (int *)(((long long)(int)(pl + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
    }
    if (AngleDiff(Vec3_HorzAngle((struct Vector3 *)(c + 0x5c), &v), *(short *)(c + 0x8e)) >= *(unsigned short *)(c + 0x3ec)) return;
    *(int *)(c + 0x38c) = (int)pl;
    func_ov102_0214b384(c, 0x96);
}
