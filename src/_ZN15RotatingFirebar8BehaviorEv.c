#pragma opt_propagation off
#pragma opt_dead_assignments off
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;

#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))&0xFFFFFFFFFFFFFFFFLL))
#define LI(v) ((int)(((long long)(v)) & 0xFFFFFFFFFFFFFFFFLL))

struct Vector3 { int x, y, z; };

extern int _ZN16MeshColliderBase9IsEnabledEv(void* p);
extern void _ZN16MeshColliderBase7DisableEv(void* p);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* c);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(void* a, void* m, void* b);
extern void AddVec3(void* a, void* b, void* c);
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN6Player4BurnEv(void* p);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int x, int y, int z, int v, int cb);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN12CylinderClsn6UpdateEv(void* p);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* c, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);

extern int data_020a0e68[];

typedef struct Firebar {
    char pad0[0x320];
    u32 arr320[8];
    u32 arr340[8];
} Firebar;

int _ZN15RotatingFirebar8BehaviorEv(char* c)
{
    int i;
    char* w;
    char* cyl;
    int b;

    *(s16*)AT(c, 0x8e) = *(s16*)AT(c, 0x8e) - 0x100;

    b = ((*(int*)(c + 0xb0) & 8) != 0) ? 1 : 0;
    if (b) {
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124) != 0) {
            _ZN16MeshColliderBase7DisableEv(c + 0x124);
        }
        return 1;
    }

    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    Matrix4x3_FromRotationY(data_020a0e68, *(s16*)(c + 0x8e));

    w = c;
    cyl = c + 0x360;
    i = 0;

    do {
        struct Vector3 in, out;
        void* p;
        u32 id;
        int is_burn;

        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        in.x = 0;
        in.y = 0;
        in.z = (i < 4) ? (LI(i) - 4) * 0x96000 - 0x64000
                       : (LI(i) - 3) * 0x96000 + 0x64000;

        MulVec3Mat4x3(&in, data_020a0e68, &out);
        AddVec3(&out, c + 0x5c, &out);

        *(int*)(w + 0x390) = out.x;
        *(int*)(w + 0x394) = out.y;
        *(int*)(w + 0x398) = out.z;

        id = *(u32*)(w + 0x384);
        if (id != 0) {
            p = _ZN5Actor10FindWithIDEj(id);
            if (p != 0) {
                is_burn = (*(u16*)((char*)p + 0xc) == 0xbf) ? 1 : 0;
                if (is_burn != 0)
                    _ZN6Player4BurnEv(p);
            }
        }

        out.y = out.y + 0x64000;
        ((Firebar*)c)->arr320[i] = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            ((Firebar*)c)->arr320[i], 0x4b, out.x, out.y, out.z, 0, 0);
        ((Firebar*)c)->arr340[i] = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            ((Firebar*)c)->arr340[i], 0x4c, out.x, out.y, out.z, 0, 0);

        _ZN12CylinderClsn5ClearEv(cyl);
        _ZN12CylinderClsn6UpdateEv(cyl);

        w += 0x3c;
        cyl += 0x3c;
        i += 1;
    } while (i < 8);

    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
