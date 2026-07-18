//cpp
typedef unsigned char u8;
typedef short s16;

struct Vector3 { int x, y, z; };

extern "C" {
char *_ZN5Actor13ClosestPlayerEv(char *self);
int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(char *c, int a, int b);
void _ZN8Platform21UpdateModelPosAndRotYEv(char *c);
void _ZN8Platform19UpdateClsnPosAndRotEv(char *c);
int _ZN16MeshColliderBase9IsEnabledEv(char *c);
void _ZN16MeshColliderBase7DisableEv(char *c);
void Matrix4x3_FromRotationY(void *m, int angle);
void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *dst);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *d);
int Vec3_Dist(void *a, void *b);
int Vec3_HorzDist(void *a, void *b);
char *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, char *p);
extern char data_020a0e68[];
}

extern "C" int _ZN11CannonHatch8BehaviorEv(char *c)
{
    if (*(u8 *)(c + 0x32e) != 0) {
        if (*(u8 *)(_ZN5Actor13ClosestPlayerEv(c) + 0x703) != 0) {
            *(int *)(c + 0x5c) = *(int *)(c + 0x320);
            *(int *)(c + 0x60) = *(int *)(c + 0x324);
            *(int *)(c + 0x64) = *(int *)(c + 0x328);
            if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0)) {
                _ZN8Platform21UpdateModelPosAndRotYEv(c);
                _ZN8Platform19UpdateClsnPosAndRotEv(c);
            }
        } else {
            if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124)) {
                _ZN16MeshColliderBase7DisableEv(c + 0x124);
            }
        }
        return 1;
    }
    if (*(u8 *)(c + 0x32c) != 0) {
        Vector3 in;
        Vector3 out;
        in.x = 0;
        in.y = 0;
        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        if (*(u8 *)(c + 0x32d) != 0) {
            if (*(u8 *)(c + 0x32d) == 1) in.z = 0x2000;
        } else {
            in.y = -0x1000;
        }
        Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x8e));
        MulVec3Mat4x3(&in, data_020a0e68, &out);
        AddVec3((Vector3 *)(c + 0x5c), &out, (Vector3 *)(c + 0x5c));
        if (Vec3_Dist((void *)(c + 0x320), (void *)(c + 0x5c)) > 0xa000) {
            *(u8 *)(c + 0x32d) = 1;
        }
        if (Vec3_HorzDist((void *)(c + 0x320), (void *)(c + 0x5c)) > 0xc8000) {
            char *a = _ZN5Actor15FindWithActorIDEjPS_(0xe, 0);
            while (a != 0) {
                *(u8 *)(a + 0x32e) = 1;
                a = _ZN5Actor15FindWithActorIDEjPS_(0xe, a);
            }
        }
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
    }
    if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    }
    return 1;
}
