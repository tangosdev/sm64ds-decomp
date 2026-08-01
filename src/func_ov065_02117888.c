typedef struct { int x, y, z; } Vector3;
typedef int Fix12i;
extern char* _ZN5Actor22ClosestNonVanishPlayerEv(void);
extern Fix12i Vec3_Dist(const Vector3* a, const Vector3* b);
extern void func_ov065_02117944(void* c, void* p);
extern void* data_ov065_0211d710;

int func_ov065_02117888(char* c)
{
    char* p = _ZN5Actor22ClosestNonVanishPlayerEv();
    if (p) {
        Vector3 v;
        int* q = (int*)(((int)p + 0x5c));
        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
        if (Vec3_Dist((const Vector3*)(c + 0x5c), &v) < 0x3e8000) {
            *(short*)(c + 0x100) = 0x14;
            func_ov065_02117944(c, &data_ov065_0211d710);
        }
    }
    return 1;
}
