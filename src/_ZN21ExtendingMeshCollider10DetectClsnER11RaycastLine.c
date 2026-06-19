typedef int s32;
typedef s32 Fix12i;
typedef struct { s32 x, y, z; } Vector3;
struct Actor;

extern char data_020a0d0c[];
extern char data_020a0d60[];
extern char data_020a0d1c[];

extern void func_0203aa74(void* thiz, Vector3* v, Vector3* res);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* line, const Vector3* a, const Vector3* b, struct Actor* act);
extern void func_02035394(void* line, void* arg);
extern int func_01ffb0fc(void* thiz, void* line);
extern void func_0203aa10(void* thiz, const Vector3* v, Vector3* res);
extern void func_020375ec(int* d, int* s);
extern void _ZN10ClsnResultaSERKS_(void* dst, const void* src);

int _ZN21ExtendingMeshCollider10DetectClsnER11RaycastLine(void* thiz, char* line)
{
    Vector3 a, b, gp;
    func_0203aa74(thiz, (Vector3*)(line + 0x38), &a);
    func_0203aa74(thiz, (Vector3*)(line + 0x54), &b);
    unsigned char saved = *(unsigned char*)(line + 0x50);
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(data_020a0d0c, &a, &b, 0);
    if (saved != 0)
        *(char*)(data_020a0d0c + 0x50) = 1;
    func_02035394(data_020a0d0c, line);
    int r = func_01ffb0fc(thiz, data_020a0d0c);
    if (r != 0)
    {
        int n = *(int*)(data_020a0d0c + 0x60);
        func_0203aa10(thiz, (const Vector3*)data_020a0d60, &gp);
        func_020375ec((int*)line, (int*)&gp);
        *(int*)(line + 0x60) = n;
        _ZN10ClsnResultaSERKS_(line + 0x10, data_020a0d1c);
        *(char*)(line + 0x50) = 1;
    }
    return r;
}
