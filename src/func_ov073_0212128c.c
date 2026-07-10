struct Vector3 { int x, y, z; };

extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, struct Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, struct Vector3* v);
extern int _ZN6Player12GetTalkStateEv(void* self);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int a);
extern void func_02011d08(void);
extern int func_ov073_0212157c(void* c, void* p);

extern void* data_0209f318;
extern void* data_ov073_02123360;

int func_ov073_0212128c(char* c)
{
    struct Vector3 la;
    struct Vector3 ps;
    void* cam;
    void* player;

    cam = data_0209f318;
    player = *(void**)(c + 0x3e4);
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

    if (_ZN6Player12GetTalkStateEv(player) == -1) {
        *(int*)(((int)cam + 0x154) & 0xFFFFFFFFFFFFFFFF) &= ~8;
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
        func_02011d08();
        func_ov073_0212157c(c, &data_ov073_02123360);
    }
    return 1;
}
