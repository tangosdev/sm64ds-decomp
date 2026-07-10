//cpp
struct Vector3 { int x, y, z; };
class Actor;
extern "C" int func_ov072_02121d50(Actor *a);
extern "C" int func_0201267c(int id, void *p);
extern bool _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(Actor *thiz, Vector3 &a, Vector3 &b, bool c);
extern void _ZN9Animation7AdvanceEv(void *anim);
extern void _ZN12CylinderClsn5ClearEv(void *clsn);
extern "C" int func_ov072_021217ac(Actor *thiz)
{
    char *c = (char *)thiz;
    short h = *(short *)(*(char **)(c + 0x360) + 0x8e);
    *(short *)(c + 0x8e) = h;
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    int flags = *(int *)(c + 0xb0);
    int b1 = (int)((flags & 0x100) != 0);
    if (b1 != 0) {
        int b2 = (int)((flags & 0x2000) != 0);
        if (b2 == 0) goto after_ray;
    }
    {
        char *q = *(char **)(c + 0x360);
        Vector3 v;
        int y = *(int *)(q + 0x60);
        int z = *(int *)(q + 0x64);
        /* demand x first into a live, then y+ so first-demanded gets higher reg (ip) */
        int x = *(int *)(q + 0x5c);
        int y2 = y + 0x32000;
        v.x = x;
        v.y = y2;
        v.z = z;
        _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(thiz, v, *(Vector3 *)(c + 0x5c), true);
        {
            int z0 = 0;
            *(int *)(c + 0x360) = z0;
            func_ov072_02121d50(thiz);
        }
    }
after_ray:
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN12CylinderClsn5ClearEv(c + 0x160);
    if (*(int *)(c + 8) == 0) {
        unsigned int t = (unsigned int)(*(int *)(c + 0x12c) << 4) >> 16;
        if (t == 0x10 || t == 0x25) {
            func_0201267c(0xf2, c + 0x74);
        }
    }
    return 1;
}
