struct Vector3 { int x, y, z; };

extern void* _ZN5Actor13ClosestPlayerEv(void* actor);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, struct Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, struct Vector3* v);
extern short Vec3_HorzAngle(const struct Vector3* a, const struct Vector3* b);
extern void _Z14ApproachLinearRsss(short* p, short a, short b);
extern int _ZN6Player9StartTalkER9ActorBaseb(void* self, void* actor, int b);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int a);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* self, void* actor, unsigned int msg, const struct Vector3* pos, unsigned int a, unsigned int b);
extern void func_02012694(int a, void* p);
extern int func_ov073_0212157c(void* c, void* p);

extern void* data_0209f318;
extern void* data_ov073_02123350;

int func_ov073_02121388(char* c) {
    struct Vector3 vplayer;
    struct Vector3 vmsg;
    struct Vector3 la;
    struct Vector3 ps;
    void* player;
    void* cam;
    int msg;
    char* p;

    player = _ZN5Actor13ClosestPlayerEv(c);
    if (player == 0) return 1;

    {
        int pv = (int)(((int)player + 0x5c) & 0xFFFFFFFFFFFFFFFFull);
        vplayer.x = *(int*)pv;
        vplayer.y = *(int*)(pv + 4);
        vplayer.z = *(int*)(pv + 8);
    }
    cam = data_0209f318;
    vmsg.x = *(int*)(c + 0x5c);
    vmsg.y = *(int*)(c + 0x60);
    vmsg.z = *(int*)(c + 0x64);

    _ZN6Camera9SetFlag_3Ev(cam);

    la.x = *(int*)(c + 0x5c);
    la.y = *(int*)(c + 0x60);
    la.z = *(int*)(c + 0x64);
    ps.x = *(int*)(c + 0x5c);
    ps.y = *(int*)(c + 0x60);
    ps.z = *(int*)(c + 0x64);
    la.y = la.y + 0x70000;
    la.z = la.z - 0x2a0000;
    ps.x = ps.x - 0x300000;
    ps.y = ps.y + 0x20000;
    ps.z = ps.z + 0xffa34000;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &la);
    _ZN6Camera6SetPosERK7Vector3(cam, &ps);

    vmsg.y = vmsg.y + 0x64000;
    _Z14ApproachLinearRsss((short*)(c + 0x94), Vec3_HorzAngle((struct Vector3*)(c + 0x5c), &vplayer), 0x800);

    *(void**)(c + 0x3e4) = player;
    p = *(char**)(c + 0x3e4);
    msg = (short)(*(int*)(p + 8) + 0xe3);
    if (_ZN6Player9StartTalkER9ActorBaseb(p, c, 1)) {
        if (*(unsigned char*)(c + 0x4c8) == 0) {
            _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2c);
            *(unsigned char*)(c + 0x4c8) = 0;
        }

        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(void**)(c + 0x3e4), c, msg, &vmsg, 0, 2)) {
            func_02012694(0x12a, c + 0x74);
            func_ov073_0212157c(c, &data_ov073_02123350);
        }
    }
    return 1;
}
