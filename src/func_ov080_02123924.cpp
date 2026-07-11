//cpp
typedef int Fix12i;
struct Vector3 { Fix12i x, y, z; };
struct BCA_File;
struct AnimData { int f[2]; };
extern "C" {
void _ZN9Animation7AdvanceEv(void* self);
int _ZN9Animation8FinishedEv(void* self);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, struct BCA_File* f, int a, int b, unsigned int c);
char* _ZN5Actor13ClosestPlayerEv(char* self);
short Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
int AngleDiff(int a, int b);
Fix12i Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern struct AnimData data_ov080_021283e8;

void func_ov080_02123924(char* c){
    _ZN9Animation7AdvanceEv(c + 0x124);
    if (_ZN9Animation8FinishedEv(c + 0x124)) {
        *(int*)(c + 0x17c) = 5;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (struct BCA_File*)data_ov080_021283e8.f[1], 0x40000000, 0x1000, 0);
        return;
    }
    {
        char* p = _ZN5Actor13ClosestPlayerEv(c);
        struct Vector3 v;
        Fix12i* q;
        if (p == 0) return;
        q = (Fix12i*)(((int)p + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
        if ((short)AngleDiff(*(short*)(c + 0x8e), Vec3_HorzAngle((struct Vector3*)(c + 0x5c), &v)) >= 0x4000) return;
        if (Vec3_HorzDist((struct Vector3*)(c + 0x5c), &v) >= 0x1f4000) return;
        *(int*)(c + 0x17c) = 5;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (struct BCA_File*)data_ov080_021283e8.f[1], 0x40000000, 0x1000, 0);
    }
}
}
