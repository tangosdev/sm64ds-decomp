//cpp
struct Vector3 { int x, y, z; };

extern "C" void* _ZN5Actor13ClosestPlayerEv(void*);
extern "C" void func_ov002_020d228c(void*);
extern "C" void _ZN6Camera9SetFlag_3Ev(void*);
extern "C" void _ZN6Camera9SetLookAtERK7Vector3(void*, const Vector3*);
extern "C" void _ZN6Camera6SetPosERK7Vector3(void*, const Vector3*);

extern void* CAMERA;

extern "C" int func_ov085_0212df84(char* c)
{
    Vector3 look, pos;
    void* cam;
    void* player;
    *(int*)(c + 0x2a4) = -0x5a0000;
    *(int*)(c + 0x2a8) = 0x1c0000;
    *(int*)(c + 0x2ac) = 0x1a66000;
    player = _ZN5Actor13ClosestPlayerEv(c);
    if (player) {
        *(short*)(c + 0x2d4) = *(short*)((char*)player + 0x8e);
        func_ov002_020d228c(player);
    }
    cam = CAMERA;
    _ZN6Camera9SetFlag_3Ev(cam);
    look.x = 0xff883000;
    look.y = 0x2ef000;
    look.z = 0x1a36000;
    pos.x = 0xffb18000;
    pos.y = 0x18c000;
    pos.z = 0x1a89000;
    _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
    _ZN6Camera6SetPosERK7Vector3(cam, &pos);
    *(short*)(c + 0x100) = 0x79;
    *(int*)(c + 0x5c) = *(int*)(c + 0x2a4);
    *(int*)(c + 0x60) = *(int*)(c + 0x2a8);
    *(int*)(c + 0x64) = *(int*)(c + 0x2ac);
    *(int*)(c + 0x2a0) = 0;
    return 1;
}
