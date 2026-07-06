//cpp
struct C3;
typedef void (C3::*Fn)();
struct Obj { char pad[8]; Fn fn; };
struct Vector3 { int x, y, z; };

struct PathPtr {
    char pad[8];
    PathPtr();
    void FromID(unsigned int id);
    void GetNode(Vector3& out, unsigned int idx) const;
};

extern "C" {
unsigned short DecIfAbove0_Short(unsigned short* p);
void Vec3_Sub(Vector3* out, Vector3* a, Vector3* b);
int LenVec3(Vector3* v);
short Vec3_HorzAngle(Vector3* v0, Vector3* v1);
short Vec3_VertAngle(Vector3* v0, Vector3* v1);
void Matrix4x3_FromRotationY(void* m, int angle);
void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angle);
void MulVec3Mat4x3(Vector3* v, void* m, Vector3* out);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void* thiz, void* clsn);
void func_ov090_02133904(void* c);
void _ZN12CylinderClsn5ClearEv(void* c);
char* _ZN5Actor13ClosestPlayerEv(void* c);
void _ZN12CylinderClsn6UpdateEv(void* c);
extern char data_020a0e68[];
}

void ApproachLinear(short& v, short target, short step);

extern "C" int func_ov090_021339d8(char* c)
{
    DecIfAbove0_Short((unsigned short*)(c + 0x100));
    {
        Obj* o = *(Obj**)(c + 0x370);
        if (*(int*)((char*)o + 8) != 0) {
            (((C3*)c)->*(o->fn))();
        }
    }
    {
        PathPtr p;
        Vector3 node;
        Vector3 diff;
        Vector3 v;
        int len;

        p.FromID(*(unsigned int*)(c + 0x388));
        p.GetNode(node, *(unsigned int*)(c + 0x390));
        Vec3_Sub(&diff, (Vector3*)(c + 0x5c), &node);
        len = LenVec3(&diff);
        if (len == 0 || len <= 0x258000) {
            (*(int*)(((int)c + 0x390) & 0xFFFFFFFFFFFFFFFF))++;
            if (*(int*)(c + 0x390) >= *(int*)(c + 0x38c))
                *(int*)(c + 0x390) = 0;
        }
        ApproachLinear(*(short*)(c + 0x94), Vec3_HorzAngle((Vector3*)(c + 0x5c), &node), 0x180);
        ApproachLinear(*(short*)(c + 0x92), Vec3_VertAngle((Vector3*)(c + 0x5c), &node), 0x40);
        *(short*)(c + 0x8c) = *(short*)(c + 0x92);
        *(short*)(c + 0x8e) = *(short*)(c + 0x94);
        *(short*)(c + 0x90) = *(short*)(c + 0x96);
        v.y = v.x = v.z = 0;
        v.z = 0x14000;
        Matrix4x3_FromRotationY(data_020a0e68, *(short*)(c + 0x94));
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(short*)(c + 0x92));
        MulVec3Mat4x3(&v, data_020a0e68, (Vector3*)(c + 0xa4));
    }
    {
        int s = *(int*)(c + 0xa8) + *(int*)(c + 0x9c);
        int m2 = *(int*)(c + 0xa0);
        int ac = *(int*)(c + 0xac);
        if (s >= m2) m2 = s;
        *(int*)(c + 0xa8) = m2;
        *(int*)(c + 0xac) = ac;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, c + 0x110);
    func_ov090_02133904(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        char* p = _ZN5Actor13ClosestPlayerEv(c);
        if (p != 0 && *(unsigned char*)(p + 0x6fb) == 0)
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
    }
    return 1;
}
