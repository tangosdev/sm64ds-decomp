struct Vector3 { int x, y, z; };

extern void* _ZN5Actor13ClosestPlayerEv(void* actor);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, struct Vector3* v, unsigned int d);
extern void func_ov073_0211f144(void* actor);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void MulVec3Mat4x3(void* in, void* m, void* out);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, struct Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, struct Vector3* v);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern void _ZN7Message11PrepareTalkEv(void);
extern int _ZN6Player9StartTalkER9ActorBaseb(void* self, void* actor, int b);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* self, void* actor, unsigned int msg, const struct Vector3* pos, unsigned int a, unsigned int b);
extern void func_02012694(int a, void* p);
extern int func_ov073_0212157c(void* c, void* p);

extern void* CAMERA;
extern int MATRIX_SCRATCH_PAPER[];
extern void* data_ov073_02123410;

int func_ov073_0211fc78(char* c) {
    struct Vector3 msgpos[2];
    struct Vector3 la, ps, in, out;
    void* player;
    void* cam;

    player = _ZN5Actor13ClosestPlayerEv(c);
    *(unsigned int*)(c + 0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c + 0x500), 3, 0x170, (struct Vector3*)(c + 0x74), 0);
    if (player == 0) return 1;

    func_ov073_0211f144(c);

    {
        int pv = (int)(((int)player + 0x5c) & 0xFFFFFFFFFFFFFFFFull);
        msgpos[0].x = *(int*)pv;
        msgpos[0].y = *(int*)(pv + 4);
        msgpos[0].z = *(int*)(pv + 8);
    }
    cam = CAMERA;
    msgpos[1].x = *(int*)(c + 0x5c);
    msgpos[1].y = *(int*)(c + 0x60);
    msgpos[1].z = *(int*)(c + 0x64);

    _ZN6Camera9SetFlag_3Ev(cam);

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    la.x = *(int*)(c + 0x3d8);
    la.y = *(int*)(c + 0x3dc);
    la.z = *(int*)(c + 0x3e0);
    ps.x = *(int*)(c + 0x5c);
    ps.y = *(int*)(c + 0x60);
    ps.z = *(int*)(c + 0x64);

    la.y -= 0x300000;
    in.z = -0x55e000;

    Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, *(short*)(c + 0x8e));
    MulVec3Mat4x3(&in, MATRIX_SCRATCH_PAPER, &out);

    ps.x = ps.x + out.x;
    ps.y = ps.y + 0x200000;
    ps.z = ps.z + out.z;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &la);
    _ZN6Camera6SetPosERK7Vector3(cam, &ps);

    if (player != 0) {
        int msg;
        *(void**)(c + 0x3e4) = player;
        msg = (short)(*(int*)((char*)*(void**)(c + 0x3e4) + 8) + 0xe7);
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
        _ZN7Message11PrepareTalkEv();
        if (_ZN6Player9StartTalkER9ActorBaseb(*(void**)(c + 0x3e4), c, 1)) {
            _ZN6Camera9SetFlag_3Ev(cam);
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(void**)(c + 0x3e4), c, msg, &msgpos[1], 0, 2)) {
                func_02012694(0x12a, (void*)(c + 0x74));
                func_ov073_0212157c(c, &data_ov073_02123410);
            }
        }
    }
    return 1;
}
