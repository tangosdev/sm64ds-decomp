extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, void* a, void* b, void* actor);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void Vec3_Asr(void* d, void* s, int sh);
extern int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void* m, void* v, void* w, int fix, void* out);
extern void _ZN11RaycastLineD1Ev(void* self);
extern void func_ov030_02112094(void* c);

extern char data_0209f43c;
extern char data_0209b3ec;

struct Vec3 { int x, y, z; };

void func_ov030_02111734(char* c)
{
    struct Vec3 a, b, out, asr;
    char rc[0x7c];

    if (DecIfAbove0_Byte((unsigned char*)(c + 0x3cb)))
        return;

    _ZN11RaycastLineC1Ev(rc);

    a.x = *(int*)(c + 0x5c);
    a.y = *(int*)(c + 0x60);
    a.z = *(int*)(c + 0x64);
    a.y = a.y + 0x32000;
    b.x = *(int*)(c + 0x5c);
    b.y = *(int*)(c + 0x60);
    b.z = *(int*)(c + 0x64);
    b.y = b.y - 0x96000;
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rc, &a, &b, c);

    if (*(int*)(c + 0x384) - *(int*)(c + 0x60) <= 0x96000) {
        if (!_ZN11RaycastLine10DetectClsnEv(rc))
            goto done;
    }

    Vec3_Asr(&asr, c + 0x380, 3);

    if (_ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(&data_0209f43c, &data_0209b3ec, &asr, 0x1f400, &out) <= 0xc350000)
        goto done;

    *(int*)(c + 0x5c) = *(int*)(c + 0x380);
    *(int*)(c + 0x60) = *(int*)(c + 0x384);
    *(int*)(c + 0x64) = *(int*)(c + 0x388);
    *(int*)(c + 0x38c) = *(int*)(c + 0x380);
    *(int*)(c + 0x390) = *(int*)(c + 0x384);
    *(int*)(c + 0x394) = *(int*)(c + 0x388);
    *(int*)(c + 0x68) = *(int*)(c + 0x380);
    *(int*)(c + 0x6c) = *(int*)(c + 0x384);
    *(int*)(c + 0x70) = *(int*)(c + 0x388);
    func_ov030_02112094(c);
    *(unsigned char*)(c + 0x3cb) = 0x96;

done:
    _ZN11RaycastLineD1Ev(rc);
}
