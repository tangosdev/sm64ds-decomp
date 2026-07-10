typedef struct Vector3 {
    int x, y, z;
} Vector3;

typedef struct RaycastLine {
    char pad[0x78];
} RaycastLine;

extern signed char data_0209f2f8;
extern char data_020a0e68[];

extern void _ZN11RaycastLineC1Ev(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(
    void *self, Vector3 *a, Vector3 *b, void *actor);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angle);
extern void MulVec3Mat4x3(void *in, void *m, void *out);

int func_ov077_02126300(char *c)
{
    RaycastLine ray1;
    RaycastLine ray2;
    Vector3 start;
    Vector3 end;
    Vector3 dir;
    Vector3 out;
    int y;

    if (data_0209f2f8 == 0x2a) {
        _ZN11RaycastLineC1Ev(&ray1);
        _ZN11RaycastLineC1Ev(&ray2);

        start.x = 0;
        start.y = 0;
        start.z = 0;
        end.x = 0;
        end.y = 0;
        end.z = 0;
        dir.x = 0;
        dir.y = 0;
        dir.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;

        start.x = *(int *)(c + 0x5c);
        y = *(int *)(c + 0x60);
        start.y = y;
        start.z = *(int *)(c + 0x64);
        start.y = y + 0x28000;
        dir.z = 0xc8000;
        Matrix4x3_FromRotationY(data_020a0e68, *(short *)(c + 0x8e));
        MulVec3Mat4x3(&dir, data_020a0e68, &out);
        {
            int sx = start.x;
            int ox = out.x;
            int sy = start.y;
            int sz = start.z;
            int oy;
            int oz;
            end.x = sx;
            end.x = sx + ox;
            oy = out.y;
            oz = out.z;
            end.y = sy;
            end.y = sy + oy;
            end.z = sz;
            end.z = sz + oz;
        }
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&ray1, &start, &end, c);

        start.x = *(int *)(c + 0x5c);
        y = *(int *)(c + 0x60);
        start.y = y;
        start.z = *(int *)(c + 0x64);
        start.y = y + 0x28000;
        dir.x = 0;
        dir.y = 0;
        dir.z = 0x2c000;
        Matrix4x3_FromRotationY(data_020a0e68, *(short *)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, 0x3000);
        MulVec3Mat4x3(&dir, data_020a0e68, &out);
        {
            int sx = start.x;
            int ox = out.x;
            int sy = start.y;
            int sz = start.z;
            int oy;
            int oz;
            end.x = sx;
            end.x = sx + ox;
            oy = out.y;
            oz = out.z;
            end.y = sy;
            end.y = sy + oy;
            end.z = sz;
            end.z = sz + oz;
        }
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&ray2, &start, &end, c);

        if (_ZN11RaycastLine10DetectClsnEv(&ray1) != 0 ||
            _ZN11RaycastLine10DetectClsnEv(&ray2) == 0) {
            *(int *)(c + 0x5c) = *(int *)(c + 0x410);
            *(int *)(c + 0x60) = *(int *)(c + 0x414);
            *(int *)(c + 0x64) = *(int *)(c + 0x418);
            *(int *)(c + 0x98) = 0;
            _ZN11RaycastLineD1Ev(&ray2);
            _ZN11RaycastLineD1Ev(&ray1);
            return 1;
        }
        _ZN11RaycastLineD1Ev(&ray2);
        _ZN11RaycastLineD1Ev(&ray1);
    }
    return 0;
}
