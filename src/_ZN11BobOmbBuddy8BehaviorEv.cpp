//cpp
struct Vector3 { int x, y, z; };
extern "C" void func_ov084_0212c9a8(void *c);
extern "C" void *_ZN5Actor13ClosestPlayerEv(void *thiz);
extern "C" int Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern "C" short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern "C" int _Z14ApproachLinearRsss(short *r, short b, short c);
extern "C" void _ZN9Animation7AdvanceEv(void *a);
extern "C" void func_02012694(int a0, void *a1);
extern "C" void func_ov084_0212ce50(void *c);
extern "C" void _ZN12CylinderClsn5ClearEv(void *c);
extern "C" void _ZN12CylinderClsn6UpdateEv(void *c);

extern "C" int _ZN11BobOmbBuddy8BehaviorEv(char *thiz)
{
    Vector3 v;
    char *p;
    func_ov084_0212c9a8(thiz);
    p = (char*)_ZN5Actor13ClosestPlayerEv(thiz);
    if (p != 0) {
        Vector3 *ps = (Vector3 *)(((unsigned long long)(unsigned)(p + 0x5c)) & 0xFFFFFFFFFFFFFFFFULL);
        v = *ps;
        if (Vec3_HorzDist((Vector3*)(thiz + 0x5c), &v) < 0x12c000) {
            short ang = Vec3_HorzAngle((Vector3*)(thiz + 0x5c), &v);
            _Z14ApproachLinearRsss((short*)(thiz + 0x8e), ang, 0x100);
        }
    }
    _ZN9Animation7AdvanceEv(thiz + 0x158);
    if ((unsigned short)(*(int*)(thiz + 0x160) >> 12) == 0)
        func_02012694(0xd7, thiz + 0x74);
    func_ov084_0212ce50(thiz);
    _ZN12CylinderClsn5ClearEv(thiz + 0xd4);
    _ZN12CylinderClsn6UpdateEv(thiz + 0xd4);
    return 1;
}
